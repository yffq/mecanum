#!/bin/bash
cd `rospack find avr_controller`

# Test to see if the file exists with the correct permissions
if [ -n "`ls -l bin/gpio_export 2>/dev/null | grep \"\-rwsr\-xr\-x\"`" ]; then
	exit 0
fi

mkdir -p bin
gcc gpio_export/gpio_export.c -o bin/gpio_export
sudo chown root:root bin/gpio_export
sudo chmod 4755 bin/gpio_export
