#!/bin/bash
WORKDIR="/home/pi/datoflasher"
FIRMWARE="$WORKDIR/dato_bootloader.elf"

OPENOCD_CONFIG="$WORKDIR/openocd.cfg"
 
echo "Dato DUO firmware flasher"
echo ""

if [ -e $FIRMWARE ]; then
	sudo openocd -f $OPENOCD_CONFIG -c "program $FIRMWARE reset exit"
else
	echo "Firmware file $FIRMWARE not found. Exiting..."
	echo ""
fi

