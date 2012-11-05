#!/bin/bash
cd `rospack find avr_controller`/bin

# Exit if the file doesn't exist
if [ -z "`ls -l gpio_export 2>/dev/null`" ]; then
	exit 0
fi

# Exit if the file already exists with the correct permissions
if [ -n "`ls -l gpio_export 2>/dev/null | grep \"\-rwsr\-xr\-x\"`" ]; then
	exit 0
fi

sudo chown root:root gpio_export
sudo chmod 4755 gpio_export
