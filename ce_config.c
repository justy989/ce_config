#include "ce/ce_app.h"
#include "ce/ce_key_defines.h"

#include <string.h>
#include <stdlib.h>

#if defined(DISPLAY_GUI)
    #include "ce/ce_draw_gui.h"
#elif defined(DISPLAY_TERM)
    #include <ncurses.h>
#endif

typedef struct{
     CeSyntaxDef_t syntax_defs[CE_SYNTAX_COLOR_COUNT];
}Config_t;

#if defined(PLATFORM_WINDOWS)
__declspec(dllimport)
#endif
bool ce_init(CeApp_t* app);

#if defined(PLATFORM_WINDOWS)
__declspec(dllimport)
#endif
bool ce_free(CeApp_t* app);

#if defined(PLATFORM_WINDOWS)
__declspec(dllimport)
#endif
bool ce_on_save_file(CeApp_t* app, CeBuffer_t* buffer);

CeVimParseResult_t custom_vim_parse_verb_substitute(CeVimAction_t* action, const CeVim_t* vim, CeRune_t key);
bool custom_vim_verb_substitute(CeVim_t* vim, const CeVimAction_t* action, CeRange_t motion_range, CeView_t* view,
                                CePoint_t* cursor, CeVimVisualData_t* visual, CeVimBufferData_t* buffer_data,
                                const CeConfigOptions_t* config_options);

CeCommandStatus_t command_hot_mark_set(CeCommand_t* command, void* user_data);
CeCommandStatus_t command_hot_mark_goto(CeCommand_t* command, void* user_data);
CeCommandStatus_t command_grep_word_under_cursor(CeCommand_t* command, void* user_data);

bool ce_init(CeApp_t* app){
     Config_t* config = malloc(sizeof(*config));
     app->user_config_data = config;

     // config options
     {
          CeConfigOptions_t* config_options = &app->config_options;
          config_options->tab_width = 4;
          config_options->horizontal_scroll_off = 10;
          config_options->vertical_scroll_off = 5;
          config_options->insert_spaces_on_tab = true;
          config_options->line_number = CE_LINE_NUMBER_ABSOLUTE;
          config_options->visual_line_display_type = CE_VISUAL_LINE_DISPLAY_TYPE_EXCLUDE_NEWLINE;
          config_options->completion_line_limit = 15;
          config_options->message_display_time_usec = 5000000; // 5 seconds
          config_options->popup_view_height = 10;
          config_options->apply_completion_key = CE_TAB;
          config_options->cycle_next_completion_key = ce_ctrl_key('n');
          config_options->cycle_prev_completion_key = ce_ctrl_key('p');
          config_options->clangd_trigger_completion_key = ce_ctrl_key('y');

          config_options->color_defs[CE_COLOR_BLACK].red = 36;
          config_options->color_defs[CE_COLOR_BLACK].green = 36;
          config_options->color_defs[CE_COLOR_BLACK].blue = 36;

          config_options->color_defs[CE_COLOR_RED].red = 137;
          config_options->color_defs[CE_COLOR_RED].green = 56;
          config_options->color_defs[CE_COLOR_RED].blue = 56;

          config_options->color_defs[CE_COLOR_GREEN].red = 69;
          config_options->color_defs[CE_COLOR_GREEN].green = 123;
          config_options->color_defs[CE_COLOR_GREEN].blue = 77;

          config_options->color_defs[CE_COLOR_YELLOW].red = 150;
          config_options->color_defs[CE_COLOR_YELLOW].green = 111;
          config_options->color_defs[CE_COLOR_YELLOW].blue = 78;

          config_options->color_defs[CE_COLOR_BLUE].red = 70;
          config_options->color_defs[CE_COLOR_BLUE].green = 107;
          config_options->color_defs[CE_COLOR_BLUE].blue = 138;

          config_options->color_defs[CE_COLOR_MAGENTA].red = 116;
          config_options->color_defs[CE_COLOR_MAGENTA].green = 90;
          config_options->color_defs[CE_COLOR_MAGENTA].blue = 160;

          config_options->color_defs[CE_COLOR_CYAN].red = 55;
          config_options->color_defs[CE_COLOR_CYAN].green = 125;
          config_options->color_defs[CE_COLOR_CYAN].blue = 108;

          config_options->color_defs[CE_COLOR_WHITE].red = 48;
          config_options->color_defs[CE_COLOR_WHITE].green = 48;
          config_options->color_defs[CE_COLOR_WHITE].blue = 48;

          config_options->color_defs[CE_COLOR_BRIGHT_BLACK].red = 42;
          config_options->color_defs[CE_COLOR_BRIGHT_BLACK].green = 42;
          config_options->color_defs[CE_COLOR_BRIGHT_BLACK].blue = 42;

          config_options->color_defs[CE_COLOR_BRIGHT_RED].red = 157;
          config_options->color_defs[CE_COLOR_BRIGHT_RED].green = 110;
          config_options->color_defs[CE_COLOR_BRIGHT_RED].blue = 127;

          config_options->color_defs[CE_COLOR_BRIGHT_GREEN].red = 110;
          config_options->color_defs[CE_COLOR_BRIGHT_GREEN].green = 137;
          config_options->color_defs[CE_COLOR_BRIGHT_GREEN].blue = 106;

          config_options->color_defs[CE_COLOR_BRIGHT_YELLOW].red = 156;
          config_options->color_defs[CE_COLOR_BRIGHT_YELLOW].green = 148;
          config_options->color_defs[CE_COLOR_BRIGHT_YELLOW].blue = 95;

          config_options->color_defs[CE_COLOR_BRIGHT_BLUE].red = 114;
          config_options->color_defs[CE_COLOR_BRIGHT_BLUE].green = 151;
          config_options->color_defs[CE_COLOR_BRIGHT_BLUE].blue = 179;

          config_options->color_defs[CE_COLOR_BRIGHT_MAGENTA].red = 147;
          config_options->color_defs[CE_COLOR_BRIGHT_MAGENTA].green = 108;
          config_options->color_defs[CE_COLOR_BRIGHT_MAGENTA].blue = 151;

          config_options->color_defs[CE_COLOR_BRIGHT_CYAN].red = 124;
          config_options->color_defs[CE_COLOR_BRIGHT_CYAN].green = 166;
          config_options->color_defs[CE_COLOR_BRIGHT_CYAN].blue = 145;

          config_options->color_defs[CE_COLOR_BRIGHT_WHITE].red = 255;
          config_options->color_defs[CE_COLOR_BRIGHT_WHITE].green = 255;
          config_options->color_defs[CE_COLOR_BRIGHT_WHITE].blue = 255;

          config_options->color_defs[CE_COLOR_FOREGROUND].red = 218;
          config_options->color_defs[CE_COLOR_FOREGROUND].green = 218;
          config_options->color_defs[CE_COLOR_FOREGROUND].blue = 218;

          config_options->color_defs[CE_COLOR_BACKGROUND].red = 28;
          config_options->color_defs[CE_COLOR_BACKGROUND].green = 28;
          config_options->color_defs[CE_COLOR_BACKGROUND].blue = 28;

          // GUI options
          config_options->gui_window_width = 1980;
          config_options->gui_window_height = 1024;
          config_options->gui_font_size = 22;
          config_options->gui_font_line_separation = 1;
          config_options->mouse_wheel_line_scroll = 5;
#if defined(PLATFORM_WINDOWS)
          strncpy(config_options->gui_font_path, "C:\\Users\\jtiff\\Documents\\Github\\ce_config\\build\\Inconsolata-SemiBold.ttf", MAX_PATH_LEN);
          strncpy(config_options->clangd_path, "C:\\Users\\jtiff\\Desktop\\clang+llvm-18.1.8-x86_64-pc-windows-msvc\\bin\\clangd", MAX_PATH_LEN);
          strncpy(config_options->clang_format_path, "C:\\Users\\jtiff\\Desktop\\clang+llvm-18.1.8-x86_64-pc-windows-msvc\\bin\\clang-format.exe", MAX_PATH_LEN);
#else
          strncpy(config_options->gui_font_path, "/home/jtiff/font/Inconsolata-SemiBold.ttf", MAX_PATH_LEN);
          strncpy(config_options->clangd_path, "/home/jtardiff/clangd_18.1.3/bin/clangd", MAX_PATH_LEN);
#endif
     }

     // keybinds
     {
          CeKeyBindDef_t normal_mode_bind_defs[] = {
               {{'\\', 'q'},        "quit"},
               {{ce_ctrl_key('V')}, "paste_clipboard"},
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
               {{ce_ctrl_key('q')}, "delete_layout"},
               {{KEY_CLOSE},        "delete_layout"},
               {{KEY_LEFT_ARROW},   "resize_layout expand left 1"},
               {{393},              "resize_layout shrink right 1"},
               {{KEY_RIGHT_ARROW},  "resize_layout expand right 1"},
               {{402},              "resize_layout shrink left 1"},
               {{KEY_UP_ARROW},     "resize_layout expand up 1"},
               {{337},              "resize_layout shrink down 1"},
               {{KEY_DOWN_ARROW},   "resize_layout expand down 1"},
               {{336},              "resize_layout shrink up 1"},
               {{ce_ctrl_key('f')}, "load_file"},
               {{ce_ctrl_key('t')}, "new_tab"},
               {{'/'},              "search forward"},
               {{'?'},              "search backward"},
               {{':'},              "command"},
               {{'='},              "open_popup_view"},
               {{'-'},              "close_popup_view"},
               {{'g', 't'},         "select_adjacent_tab right"},
               {{'g', 'T'},         "select_adjacent_tab left"},
               {{'\\', '/'},        "regex_search forward"},
               {{'\\', '?'},        "regex_search backward"},
               {{'"', '?'},         "show_yanks"},
               {{'g', 'r'},         "redraw"},
               {{ce_ctrl_key('b')}, "switch_buffer"},
               {{343},              "goto_destination_in_line"},
               {{ce_ctrl_key('o')}, "jump_list previous"},
               {{ce_ctrl_key('i')}, "jump_list next"},
               // {{ce_ctrl_key('e')}, ""},
               {{'\\', 'b'},        "shell_command build.bat"},
               {{'\\', 'c'},        "shell_command ./clean"},
               {{'\\', 'g'},        "shell_command build\\crate.exe"},
               {{'K'},              "man_page_on_word_under_cursor"},
               {{' '},              "hot_mark_set"},
               {{KEY_ONLY_BACKSPACE},"hot_mark_goto"},
               {{'\\', 'w'},        "grep_word_under_cursor"},
               {{268},              "shell_command make"},
               {{'\\', '-'},        "font_adjust_size -2"},
               {{'\\', '+'},        "font_adjust_size +2"},
               {{'\\', 'd'},        "clang_goto_def"},
               {{'\\', 'l'},        "clang_goto_decl"},
               {{'\\', 't'},        "clang_goto_type_def"},
               {{'\\', 'r'},        "clang_find_references"},
               {{'\\', 'f'},        "clang_format_file"},
               {{'\\', 's'},        "clang_format_selection"},
               {{'\\', 'e'},        "open_popup_view \"[clangd diagnostics]\""},
          };

          ce_convert_bind_defs(&app->key_binds, normal_mode_bind_defs, sizeof(normal_mode_bind_defs) / sizeof(normal_mode_bind_defs[0]));
     }

     // vim bindings
     {
          ce_set_vim_key_bind(app->vim.key_binds, &app->vim.key_bind_count, 'S', &custom_vim_parse_verb_substitute);
     }

     // syntax
     {
          config->syntax_defs[CE_SYNTAX_COLOR_NORMAL].fg = CE_COLOR_DEFAULT;
          config->syntax_defs[CE_SYNTAX_COLOR_NORMAL].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_TYPE].fg = CE_COLOR_BRIGHT_BLUE;
          config->syntax_defs[CE_SYNTAX_COLOR_TYPE].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_KEYWORD].fg = CE_COLOR_BLUE;
          config->syntax_defs[CE_SYNTAX_COLOR_KEYWORD].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_CONTROL].fg = CE_COLOR_YELLOW;
          config->syntax_defs[CE_SYNTAX_COLOR_CONTROL].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_CAPS_VAR].fg = CE_COLOR_MAGENTA;
          config->syntax_defs[CE_SYNTAX_COLOR_CAPS_VAR].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_COMMENT].fg = CE_COLOR_GREEN;
          config->syntax_defs[CE_SYNTAX_COLOR_COMMENT].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_STRING].fg = CE_COLOR_RED;
          config->syntax_defs[CE_SYNTAX_COLOR_STRING].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_CHAR_LITERAL].fg = CE_COLOR_RED;
          config->syntax_defs[CE_SYNTAX_COLOR_CHAR_LITERAL].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_NUMBER_LITERAL].fg = CE_COLOR_MAGENTA;
          config->syntax_defs[CE_SYNTAX_COLOR_NUMBER_LITERAL].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_PREPROCESSOR].fg = CE_COLOR_BRIGHT_MAGENTA;
          config->syntax_defs[CE_SYNTAX_COLOR_PREPROCESSOR].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_TRAILING_WHITESPACE].fg = CE_COLOR_RED;
          config->syntax_defs[CE_SYNTAX_COLOR_TRAILING_WHITESPACE].bg = CE_COLOR_RED;
          config->syntax_defs[CE_SYNTAX_COLOR_VISUAL].fg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_VISUAL].bg = CE_COLOR_WHITE;
          config->syntax_defs[CE_SYNTAX_COLOR_CURRENT_LINE].fg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_CURRENT_LINE].bg = CE_COLOR_BLACK;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_ADD].fg = CE_COLOR_GREEN;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_ADD].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_REMOVE].fg = CE_COLOR_RED;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_REMOVE].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_HEADER].fg = CE_COLOR_MAGENTA;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_HEADER].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_COMMENT].fg = CE_COLOR_BLUE;
          config->syntax_defs[CE_SYNTAX_COLOR_DIFF_COMMENT].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_COMPLETE_SELECTED].fg = CE_SYNTAX_USE_CURRENT_COLOR;;
          config->syntax_defs[CE_SYNTAX_COLOR_COMPLETE_SELECTED].bg = CE_COLOR_WHITE;
          config->syntax_defs[CE_SYNTAX_COLOR_COMPLETE_MATCH].fg = CE_COLOR_BRIGHT_CYAN;
          config->syntax_defs[CE_SYNTAX_COLOR_COMPLETE_MATCH].bg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_LINE_NUMBER].fg = CE_COLOR_WHITE;
          config->syntax_defs[CE_SYNTAX_COLOR_LINE_NUMBER].bg = CE_COLOR_DEFAULT;
          config->syntax_defs[CE_SYNTAX_COLOR_MULTIPLE_CURSOR_INACTIVE].fg = CE_COLOR_DEFAULT;
          config->syntax_defs[CE_SYNTAX_COLOR_MULTIPLE_CURSOR_INACTIVE].bg = CE_COLOR_RED;
          config->syntax_defs[CE_SYNTAX_COLOR_MULTIPLE_CURSOR_ACTIVE].fg = CE_SYNTAX_USE_CURRENT_COLOR;
          config->syntax_defs[CE_SYNTAX_COLOR_MULTIPLE_CURSOR_ACTIVE].bg = CE_COLOR_GREEN;

          app->config_options.ui_fg_color = CE_COLOR_DEFAULT;
          app->config_options.ui_bg_color = CE_COLOR_BRIGHT_BLACK;
          app->config_options.message_fg_color = CE_COLOR_RED;
          app->config_options.message_bg_color = CE_COLOR_BRIGHT_BLACK;

          app->syntax_defs = config->syntax_defs;
     }

     // custom commands
     {
          CeCommandEntry_t command_entries[] = {
               {command_hot_mark_set, "hot_mark_set", "set the hot mark"},
               {command_hot_mark_goto, "hot_mark_goto", "goto the hot mark"},
               {command_grep_word_under_cursor, "grep_word_under_cursor", "run 'fgrep -n <word> *' on the word under the cursor"}
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

static bool string_ends_with(const char* str, const char* pattern){
     int64_t str_len = strlen(str);
     int64_t pattern_len = strlen(pattern);
     if(str_len < pattern_len) return false;
     return strncmp(str + (str_len - pattern_len), pattern, pattern_len) == 0;
}

bool ce_on_save_file(CeApp_t* app, CeBuffer_t* buffer){
#if 0
     if(string_ends_with(buffer->name, ".c") ||
        string_ends_with(buffer->name, ".h") ||
        string_ends_with(buffer->name, ".cpp") ||
        string_ends_with(buffer->name, ".hpp")){
         return ce_clang_format_buffer(app->config_options.clang_format_path,
                                       buffer,
                                       (CePoint_t){0, 0});
     }
#endif
     return true;
}

CeVimParseResult_t custom_vim_parse_verb_substitute(CeVimAction_t* action, const CeVim_t* vim, CeRune_t key){
     action->verb.function = &custom_vim_verb_substitute;
     action->repeatable = true;
     return CE_VIM_PARSE_IN_PROGRESS;
}

bool custom_vim_verb_substitute(CeVim_t* vim, const CeVimAction_t* action, CeRange_t motion_range, CeView_t* view,
                                CePoint_t* cursor, CeVimVisualData_t* visual, CeVimBufferData_t* buffer_data,
                                const CeConfigOptions_t* config_options){
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
     if(destination->x || destination->y) view->cursor = *destination;

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

     ce_app_run_shell_command(app, cmd, tab_layout, view, true);
     return CE_COMMAND_SUCCESS;
}

CeCommandStatus_t command_grep_word_under_cursor(CeCommand_t* command, void* user_data){
     return run_command_on_word_under_cursor(command, user_data, "fgrep -n %s *");
}
