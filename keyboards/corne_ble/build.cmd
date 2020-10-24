@echo off

::::PARAMETERS::::

::Synthax:  build.cmd <role> <keymap> <keyboard> <project_path> <sdk_path>
set role=%1
set keymap=%2
set keyboard=%3

set project_path=%4
set sdk_path=%5

::Default values.
if "%5"=="" set sdk_path=D:\Proyectos\Corne-ble\nRF5_SDK_15.0.0_a53641a
if "%4"=="" set project_path=D:\Proyectos\Corne-ble\qmk

if "%3"=="" set keyboard=corne_ble
if "%2"=="" set keymap=default
if "%1"=="" set role=master

::::END PARAMETERS::::




::::CALCULATED PATHS::::

::Backslash path for Windows console interpretation.
set BS_PROJ_ROOT=%project_path%

::Forward slash paths for Linux console interpretation.
set FS_SDK_ROOT=%sdk_path:\=/%
set FS_PROJ_ROOT=%project_path:\=/%
::Change disk root format form D: to /D for Linux console interpretation.
set FS_SDK_ROOT=/%FS_SDK_ROOT:~0,1%%FS_SDK_ROOT:~2%
set FS_PROJ_ROOT=/%FS_PROJ_ROOT:~0,1%%FS_PROJ_ROOT:~2%

::Name of the generated .hex file.
set file=%keyboard%_%role%_%keymap%.hex

::Paths for .hex to .uf2 converter.
set CONVERT_TO_UF2_SCRIPT=%BS_PROJ_ROOT%\keyboards\%keyboard%\uf2conv.py
set FILE_TO_CONVERT=%BS_PROJ_ROOT%\.build\%file%
set DESTINATION_FILE=%BS_PROJ_ROOT%\keyboards\%keyboard%\precompiled\%role%.uf2

::::END CALCULATED PATHS::::




::::OPERATIONS::::

echo "Building with WSL2"
wsl --setdefault Ubuntu-20.04

::Create hex file.
echo Building role '%role%' into file '%file%'
bash -c "cd %FS_PROJ_ROOT% && export NRFSDK15_ROOT=%FS_SDK_ROOT% && make %keyboard%/%role%:%keymap%" || exit

::Convert the .hex file into .uf2 file.
echo Converting '%file%' into '%role%.uf2'
python %CONVERT_TO_UF2_SCRIPT% %FILE_TO_CONVERT% -o %DESTINATION_FILE% -c -f 0xADA52840

::::END OPERATIONS::::
