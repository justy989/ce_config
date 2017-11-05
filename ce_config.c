#include "ce/ce_app.h"

#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

typedef struct{
     CeSyntaxDef_t syntax_defs[CE_SYNTAX_COLOR_COUNT];
}Config_t;

CeVimParseResult_t custom_vim_parse_verb_substitute(CeVimAction_t* action, CeRune_t key);
bool custom_vim_verb_substitute(CeVim_t* vim, const CeVimAction_t* action, CeRange_t motion_range, CeView_t* view,
                                CeVimBufferData_t* buffer_data, const CeConfigOptions_t* config_options);

CeCommandStatus_t command_hot_mark_set(CeCommand_t* command, void* user_data);
CeCommandStatus_t command_hot_mark_goto(CeCommand_t* command, void* user_data);

bool ce_init(CeApp_t* app){
     Config_t* config = malloc(sizeof(*config));
     app->user_config_data = config;

     // config options
     {
          CeConfigOptions_t* config_options = &app->config_options;
          config_options->tab_width = 5;
          config_options->horizontal_scroll_off = 10;
          config_options->vertical_scroll_off = 5;
          config_options->insert_spaces_on_tab = true;
          config_options->terminal_scroll_back = 1024; // I use this for urxvt and don't seem to have any problems
          config_options->line_number = CE_LINE_NUMBER_NONE;
     }

     // keybinds
     {
          CeKeyBindDef_t normal_mode_bind_defs[] = {
               {{'\\', 'q'}, "quit"},
               {{8}, "select_adjacent_layout left"}, // ctrl h
               {{12}, "select_adjacent_layout right"}, // ctrl l
               {{11}, "select_adjacent_layout up"}, // ctrl k
               {{10}, "select_adjacent_layout down"}, // ctrl j
               {{23}, "save_buffer"}, // ctrl w
               {{'g', 'b'}, "show_buffers"},
               {{22}, "split_layout horizontal"}, // ctrl s
               {{19}, "split_layout vertical"}, // ctrl v
               {{1}, "select_parent_layout"}, // ctrl a
               {{14}, "goto_next_destination"}, // ctrl n
               {{16}, "goto_prev_destination"}, // ctrl p
               {{KEY_CLOSE}, "delete_layout"}, // ctrl q
               {{6}, "load_file"}, // ctrl f
               {{20}, "new_tab"}, // ctrl t
               {{'/'}, "search forward"},
               {{'?'}, "search backward"},
               {{':'}, "command"},
               {{'g', 't'}, "select_adjacent_tab right"},
               {{'g', 'T'}, "select_adjacent_tab left"},
               {{'\\', '/'}, "regex_search forward"},
               {{'\\', '?'}, "regex_search backward"},
               {{'"', '?'}, "show_yanks"},
               {{'g', 'r'}, "redraw"},
               {{'\\', 'f'}, "reload_file"},
               {{24}, "switch_to_terminal"}, // ctrl x
               {{2}, "switch_buffer"}, // ctrl b
               {{343}, "goto_destination_in_line"}, // return
               {{9}, "jump_list previous"}, // ctrl + o
               {{15}, "jump_list next"}, // ctrl + i
               // {{5}, ""}, // ctrl + e
               {{'\\', 's', 'f'}, "slide_arg forward"},
               {{'\\', 's', 'b'}, "slide_arg backward"},
               {{'\\', 'b'}, "terminal_command ./build"},
               {{'\\', 'c'}, "terminal_command ./clean"},
               {{'\\', 'r'}, "terminal_command ./game"},
               {{'K'}, "man_page_on_word_under_cursor"},
               {{' '}, "hot_mark_set"},
               {{KEY_BACKSPACE}, "hot_mark_goto"},
               {{'\\', 't'}, "new_terminal"},
          };

          ce_convert_bind_defs(&app->key_binds, normal_mode_bind_defs, sizeof(normal_mode_bind_defs) / sizeof(normal_mode_bind_defs[0]));
     }

     // vim bindings
     {
          ce_set_vim_key_bind(app->vim.key_binds, &app->vim.key_bind_count, 'S', &custom_vim_parse_verb_substitute);
     }

     // syntax
     {
          config->syntax_defs[CE_SYNTAX_COLOR_NORMAL].fg = COLOR_DEFAULT;
          config->syntax_defs[CE_SYNTAX_COLOR_NORMAL].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_TYPE].fg = COLOR_BRIGHT_BLUE;
          config->syntax_defs[CE_SYNTAX_COLOR_TYPE].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_KEYWORD].fg = COLOR_BLUE;
          config->syntax_defs[CE_SYNTAX_COLOR_KEYWORD].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_CONTROL].fg = COLOR_YELLOW;
          config->syntax_defs[CE_SYNTAX_COLOR_CONTROL].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_CAPS_VAR].fg = COLOR_MAGENTA;
          config->syntax_defs[CE_SYNTAX_COLOR_CAPS_VAR].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_COMMENT].fg = COLOR_GREEN;
          config->syntax_defs[CE_SYNTAX_COLOR_COMMENT].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_STRING].fg = COLOR_RED;
          config->syntax_defs[CE_SYNTAX_COLOR_STRING].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_CHAR_LITERAL].fg = COLOR_RED;
          config->syntax_defs[CE_SYNTAX_COLOR_CHAR_LITERAL].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_NUMBER_LITERAL].fg = COLOR_MAGENTA;
          config->syntax_defs[CE_SYNTAX_COLOR_NUMBER_LITERAL].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_PREPROCESSOR].fg = COLOR_BRIGHT_MAGENTA;
          config->syntax_defs[CE_SYNTAX_COLOR_PREPROCESSOR].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_TRAILING_WHITESPACE].fg = COLOR_RED;
          config->syntax_defs[CE_SYNTAX_COLOR_TRAILING_WHITESPACE].bg = COLOR_RED;
          config->syntax_defs[CE_SYNTAX_COLOR_VISUAL].fg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_VISUAL].bg = COLOR_WHITE;
          config->syntax_defs[CE_SYNTAX_COLOR_MATCH].fg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_MATCH].bg = COLOR_WHITE;
          config->syntax_defs[CE_SYNTAX_COLOR_CURRENT_LINE].fg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_CURRENT_LINE].bg = COLOR_BLACK;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_ADD].fg = COLOR_GREEN;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_ADD].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_REMOVE].fg = COLOR_RED;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_REMOVE].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_HEADER].fg = COLOR_MAGENTA;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_HEADER].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_COMMENT].fg = COLOR_BLUE;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_COMMENT].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_COMPLETE_SELECTED].fg = CE_SYNTAX_USE_CURRENT_COLOR;;
          config->syntax_defs[CE_SYNTAX_COLOR_COMPLETE_SELECTED].bg = COLOR_WHITE;
          config->syntax_defs[CE_SYNTAX_COLOR_COMPLETE_MATCH].fg = COLOR_BRIGHT_CYAN;
          config->syntax_defs[CE_SYNTAX_COLOR_COMPLETE_MATCH].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_LINE_NUMBER].fg = COLOR_WHITE;
          config->syntax_defs[CE_SYNTAX_COLOR_LINE_NUMBER].bg = COLOR_DEFAULT;

          app->config_options.ui_fg_color = COLOR_DEFAULT;
          app->config_options.ui_bg_color = COLOR_BRIGHT_BLACK;

          app->syntax_defs = config->syntax_defs;
     }

     // custom commands
     {
          CeCommandEntry_t command_entries[] = {
               {command_hot_mark_set, "hot_mark_set", "set the hot mark"},
               {command_hot_mark_goto, "hot_mark_goto", "goto the hot mark"},
          };

          int64_t command_entry_count = sizeof(command_entries) / sizeof(command_entries[0]);
          ce_extend_commands(&app->command_entries, &app->command_entry_count, command_entries, command_entry_count);
     }

     return true;
}

bool ce_free(CeApp_t* app){
     Config_t* config = app->user_config_data;
     free(config);
     return true;
}

CeVimParseResult_t custom_vim_parse_verb_substitute(CeVimAction_t* action, CeRune_t key){
     action->verb.function = &custom_vim_verb_substitute;
     action->repeatable = true;
     action->visual_block_applies = true;
     return CE_VIM_PARSE_IN_PROGRESS;
}

bool custom_vim_verb_substitute(CeVim_t* vim, const CeVimAction_t* action, CeRange_t motion_range, CeView_t* view,
                                CeVimBufferData_t* buffer_data, const CeConfigOptions_t* config_options){
     char reg = action->verb.character;
     if(reg == 0) reg = '"';
     CeVimYank_t* yank = vim->yanks + ce_vim_yank_register_index(reg);
     if(!yank->text) return false;

     ce_range_sort(&motion_range);
     if(action->exclude_end){
          motion_range.end = ce_buffer_advance_point(view->buffer, motion_range.end, -1);
     }

     // delete the range
     int64_t delete_len = ce_buffer_range_len(view->buffer, motion_range.start, motion_range.end);
     char* removed_string = ce_buffer_dupe_string(view->buffer, motion_range.start, delete_len);
     if(!ce_buffer_remove_string(view->buffer, motion_range.start, delete_len)){
          free(removed_string);
          return false;
     }

     // commit the change
     CeBufferChange_t change = {};
     change.chain = false;
     change.insertion = false;
     change.string = removed_string;
     change.location = motion_range.start;
     change.cursor_before = view->cursor;
     change.cursor_after = motion_range.start;
     ce_buffer_change(view->buffer, &change);

     // insert the yank
     int64_t yank_len = ce_utf8_strlen(yank->text);
     if(!ce_buffer_insert_string(view->buffer, yank->text, motion_range.start)) return false;
     CePoint_t cursor_end = ce_buffer_advance_point(view->buffer, motion_range.start, yank_len);

     // commit the change
     change.chain = true;
     change.insertion = true;
     change.string = strdup(yank->text);
     change.location = motion_range.start;
     change.cursor_before = view->cursor;
     change.cursor_after = cursor_end;
     ce_buffer_change(view->buffer, &change);

     view->cursor = cursor_end;
     vim->chain_undo = action->chain_undo;

     return true;
}

bool get_layout_and_view(CeApp_t* app, CeView_t** view, CeLayout_t** tab_layout){
     *tab_layout = app->tab_list_layout->tab_list.current;

     if(app->input_mode) return false;

     if((*tab_layout)->tab.current->type == CE_LAYOUT_TYPE_VIEW){
          *view = &(*tab_layout)->tab.current->view;
          return true;
     }

     return true;
}

CeCommandStatus_t command_hot_mark_set(CeCommand_t* command, void* user_data){
     if(command->arg_count != 0) return CE_COMMAND_PRINT_HELP;

     CeApp_t* app = user_data;
     CeView_t* view = NULL;
     CeLayout_t* tab_layout = NULL;

     if(!get_layout_and_view(app, &view, &tab_layout)) return CE_COMMAND_NO_ACTION;

     CeAppBufferData_t* buffer_data = view->buffer->app_data;
     CeVimBufferData_t* vim_buffer_data = &buffer_data->vim;
     CePoint_t* destination = vim_buffer_data->marks + ce_vim_yank_register_index(' ');
     *destination = view->cursor;

     return CE_COMMAND_SUCCESS;
}

CeCommandStatus_t command_hot_mark_goto(CeCommand_t* command, void* user_data){
     if(command->arg_count != 0) return CE_COMMAND_PRINT_HELP;

     CeApp_t* app = user_data;
     CeView_t* view = NULL;
     CeLayout_t* tab_layout = NULL;

     if(!get_layout_and_view(app, &view, &tab_layout)) return CE_COMMAND_NO_ACTION;

     CeAppBufferData_t* buffer_data = view->buffer->app_data;
     CeVimBufferData_t* vim_buffer_data = &buffer_data->vim;
     CePoint_t* destination = vim_buffer_data->marks + ce_vim_yank_register_index(' ');
     view->cursor = *destination;

     return CE_COMMAND_SUCCESS;
}
