@echo off

::::PARAMETERS::::

::Synthax:  build_both.cmd <keymap> <keyboard> <project_path> <sdk_path>
set keymap=%1
set keyboard=%2

set project_path=%3
set sdk_path=%4

::Default values
if "%4"=="" set sdk_path=D:\Proyectos\Corne-ble\nRF5_SDK_15.0.0_a53641a
if "%3"=="" set project_path=D:\Proyectos\Corne-ble\qmk

if "%2"=="" set keyboard=corne_ble
if "%1"=="" set keymap=default

::::END PARAMETERS::::




::::OPERATIONS::::

call %project_path%/keyboards/%keyboard%/build.cmd master %keymap% %keyboard% %disk_root% %project_path% %sdk_path% 
call %project_path%/keyboards/%keyboard%/build.cmd slave %keymap% %keyboard% %disk_root% %project_path% %sdk_path%

::::END OPERATIONS::::
