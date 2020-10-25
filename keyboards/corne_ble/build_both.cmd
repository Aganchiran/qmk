@echo off
setlocal enabledelayedexpansion

::::PARAMETERS::::

::Synthax:  build_both.cmd <keymap> <keyboard> <project_path> <sdk_path>
set batch_file_directory=%~dp0

set keymap=%1
set keyboard=%2

set project_path=%3
set sdk_path=%4

::::END PARAMETERS::::




::::OPERATIONS::::

call %batch_file_directory%/build.cmd master %keymap% %keyboard% %disk_root% %project_path% %sdk_path%
call %batch_file_directory%/build.cmd slave %keymap% %keyboard% %disk_root% %project_path% %sdk_path%

::::END OPERATIONS::::
