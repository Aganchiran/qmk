@echo off

cd %~dp0

bash -c "cd ../../ && export NRFSDK15_ROOT=/mnt/c/SDK/nRF5_SDK_15.0.0_a53641a && make hhkb_ble" || exit

cd ..\..\.build

set file=hhkb_ble_rev1_default.hex

%~dp0/uf2conv.py %file% -c -f 0xADA52840

echo Waiting for the UF2 drive. Press Reset twice...

:loop
if not exist e:\ goto :loop

copy flash.uf2 e:\

