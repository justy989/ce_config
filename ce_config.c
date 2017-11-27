#include "ce/ce_app.h"

#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

typedef struct{
     CeSyntaxDef_t syntax_defs[CE_SYNTAX_COLOR_COUNT];
}Config_t;

CeVimParseResult_t custom_vim_parse_verb_substitute(CeVimAction_t* action, const CeVim_t* vim, CeRune_t key);
bool custom_vim_verb_substitute(CeVim_t* vim, const CeVimAction_t* action, CeRange_t motion_range, CeView_t* view,
                                CeVimBufferData_t* buffer_data, const CeConfigOptions_t* config_options);

CeCommandStatus_t command_hot_mark_set(CeCommand_t* command, void* user_data);
CeCommandStatus_t command_hot_mark_goto(CeCommand_t* command, void* user_data);
CeCommandStatus_t command_grep_word_under_cursor(CeCommand_t* command, void* user_data);
CeCommandStatus_t command_cscope_symbol_under_cursor(CeCommand_t* command, void* user_data);
CeCommandStatus_t command_cscope_caller_under_cursor(CeCommand_t* command, void* user_data);

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
          // config_options->visual_line_display_type = CE_VISUAL_LINE_DISPLAY_TYPE_EXCLUDE_NEWLINE;
          config_options->visual_line_display_type = CE_VISUAL_LINE_DISPLAY_TYPE_FULL_LINE;
          config_options->completion_line_limit = 15;
          config_options->message_display_time_usec = 5000000; // 5 seconds
          config_options->apply_completion_key = CE_TAB;
          config_options->cycle_next_completion_key = ce_ctrl_key('n');
          config_options->cycle_prev_completion_key = ce_ctrl_key('p');
     }

     // keybinds
     {
          CeKeyBindDef_t normal_mode_bind_defs[] = {
               {{'\\', 'q'},        "quit"},
               {{ce_ctrl_key('h')}, "select_adjacent_layout left"},
               {{ce_ctrl_key('l')}, "select_adjacent_layout right"},
               {{ce_ctrl_key('k')}, "select_adjacent_layout up"},
               {{ce_ctrl_key('j')}, "select_adjacent_layout down"},
               {{ce_ctrl_key('w')}, "save_buffer"},
               {{'g', 'b'},         "show_buffers"},
               {{ce_ctrl_key('v')}, "split_layout horizontal"},
               {{ce_ctrl_key('s')}, "split_layout vertical"},
               {{ce_ctrl_key('a')}, "select_parent_layout"},
               {{ce_ctrl_key('n')}, "goto_next_destination"},
               {{ce_ctrl_key('p')}, "goto_prev_destination"},
               {{KEY_CLOSE},        "delete_layout"},
               {{ce_ctrl_key('f')}, "load_file"},
               {{ce_ctrl_key('t')}, "new_tab"},
               {{'/'},              "search forward"},
               {{'?'},              "search backward"},
               {{':'},              "command"},
               {{'g', 't'},         "select_adjacent_tab right"},
               {{'g', 'T'},         "select_adjacent_tab left"},
               {{'\\', '/'},        "regex_search forward"},
               {{'\\', '?'},        "regex_search backward"},
               {{'"', '?'},         "show_yanks"},
               {{'g', 'r'},         "redraw"},
               {{'\\', 'f'},        "reload_file"},
               {{ce_ctrl_key('x')}, "switch_to_terminal"},
               {{ce_ctrl_key('b')}, "switch_buffer"},
               {{343},              "goto_destination_in_line"},
               {{ce_ctrl_key('o')}, "jump_list previous"},
               {{ce_ctrl_key('i')}, "jump_list next"},
               // {{ce_ctrl_key('e')}, ""},
               {{'\\', 'b'},        "shell_command ./build"},
               {{'\\', 'c'},        "shell_command ./clean"},
               {{'\\', 'g'},        "shell_command ./game"},
               {{'\\', 'r'},        "replace_all"},
               {{'K'},              "man_page_on_word_under_cursor"},
               {{' '},              "hot_mark_set"},
               {{KEY_BACKSPACE},    "hot_mark_goto"},
               {{'\\', 't'},        "new_terminal"},
               {{'\\', 'w'},        "grep_word_under_cursor"},
               {{'\\', 's'},        "cscope_symbol_under_cursor"},
               {{'\\', 'a'},        "cscope_caller_under_cursor"},
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
          app->config_options.message_fg_color = COLOR_RED;
          app->config_options.message_bg_color = COLOR_BRIGHT_BLACK;

          app->syntax_defs = config->syntax_defs;
     }

     // custom commands
     {
          CeCommandEntry_t command_entries[] = {
               {command_hot_mark_set, "hot_mark_set", "set the hot mark"},
               {command_hot_mark_goto, "hot_mark_goto", "goto the hot mark"},
               {command_grep_word_under_cursor, "grep_word_under_cursor", "run 'fgrep -n <word> *' on the word under the cursor"},
               {command_cscope_symbol_under_cursor, "cscope_symbol_under_cursor", "run 'cscope -L1<word>' on the word under the cursor"},
               {command_cscope_caller_under_cursor, "cscope_caller_under_cursor", "run 'cscope -L3<word>' on the word under the cursor"},
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

CeVimParseResult_t custom_vim_parse_verb_substitute(CeVimAction_t* action, const CeVim_t* vim, CeRune_t key){
     action->verb.function = &custom_vim_verb_substitute;
     action->repeatable = true;
     return CE_VIM_PARSE_IN_PROGRESS;
}

bool custom_vim_verb_substitute(CeVim_t* vim, const CeVimAction_t* action, CeRange_t motion_range, CeView_t* view,
                                CeVimBufferData_t* buffer_data, const CeConfigOptions_t* config_options){
     char reg = action->verb.character;
     if(reg == 0) reg = '"';
     CeVimYank_t* yank = vim->yanks + ce_vim_register_index(reg);
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

     if(app->input_complete_func) return false;

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
     CePoint_t* destination = vim_buffer_data->marks + ce_vim_register_index(' ');
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
     CePoint_t* destination = vim_buffer_data->marks + ce_vim_register_index(' ');
     view->cursor = *destination;

     return CE_COMMAND_SUCCESS;
}

CeCommandStatus_t run_command_on_word_under_cursor(CeCommand_t* command, void* user_data, const char* format_string){
     if(command->arg_count != 0) return CE_COMMAND_PRINT_HELP;

     CeApp_t* app = (CeApp_t*)(user_data);
     CeView_t* view = NULL;
     CeLayout_t* tab_layout = NULL;

     if(!get_layout_and_view(app, &view, &tab_layout)) return CE_COMMAND_NO_ACTION;

     CeRange_t range = ce_vim_find_little_word_boundaries(view->buffer, view->cursor); // returns -1
     char* word = ce_buffer_dupe_string(view->buffer, range.start, (range.end.x - range.start.x) + 1);
     if(!word) return CE_COMMAND_NO_ACTION;
     char cmd[128];
     snprintf(cmd, 128, format_string, word);
     free(word);

     ce_app_run_shell_command(app, cmd, tab_layout, view);
     return CE_COMMAND_SUCCESS;
}

CeCommandStatus_t command_grep_word_under_cursor(CeCommand_t* command, void* user_data){
     return run_command_on_word_under_cursor(command, user_data, "fgrep -n %s *");
}

CeCommandStatus_t command_cscope_symbol_under_cursor(CeCommand_t* command, void* user_data){
     return run_command_on_word_under_cursor(command, user_data, "cscope -L1%s");
}

CeCommandStatus_t command_cscope_caller_under_cursor(CeCommand_t* command, void* user_data){
     return run_command_on_word_under_cursor(command, user_data, "cscope -L3%s");
}
