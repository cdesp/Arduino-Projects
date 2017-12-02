avrdude -Cavrdude.conf -v -patmega328p -carduino -PCOM8 -b57600 -D -Uflash:w:CtrlPlatform.ino.NANO.hex:i 
pause