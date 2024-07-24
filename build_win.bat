@echo off
mkdir build
pushd build
cl ^
  /D_AMD64_ ^
  /DDISPLAY_GUI ^
  /DPLATFORM_WINDOWS ^
  /I "..\ce\external\include" ^
  ..\ce\ce.c ^
  ..\ce\ce_app.c ^
  ..\ce\ce_clangd.c ^
  ..\ce\ce_command.c ^
  ..\ce\ce_commands.c ^
  ..\ce\ce_complete.c ^
  ..\ce\ce_draw_gui.c ^
  ..\ce\ce_json.c ^
  ..\ce\ce_layout.c ^
  ..\ce\ce_macros.c ^
  ..\ce\ce_regex_windows.cpp ^
  ..\ce\ce_subprocess.c ^
  ..\ce\ce_syntax.c ^
  ..\ce\ce_vim.c ^
  ..\ce_config.c ^
  "SDL2.lib" "SDL2_ttf.lib" "SDL2main.lib" "shell32.lib" ^
  /link ^
  "/OUT:ce_config_gui.dll" ^
  "/LIBPATH:..\ce\external\lib" ^
  "/SUBSYSTEM:CONSOLE" ^
  "/DLL"
popd
