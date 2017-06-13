#!/bin/bash
FIRMWARE="duo-firmware.hex"

while :
do
	echo -n "Flash $FIRMWARE (Y/n) "; read a
	if [ "$a" == "n" ]
	then
		break
	fi
	stty -F /dev/ttyACM0 speed 134
	./teensy_loader_cli -w --mcu=mk20dx256 -v $FIRMWARE 
done

