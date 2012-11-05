#!/bin/bash
cd `rospack find avr_controller`/gpio_export # Location of this script

# Test to see if the file exists with the correct permissions
if [ `ls -l gpio_export 2>/dev/null | grep "\-rwsr\-xr\-x"` ]; then
	echo "Program gpio_export already exists with the correct permissions"
	exit 0
fi

cat > gpio_export.c <<EOF
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define CMD "`rospack find avr_controller`/gpio_export/gpio_export.sh %d"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Incorrect usage. Call this program with a gpio pin number\n");
		return -1;
	}
	setuid(0);
	char export_cmd[sizeof(CMD) + 3]; // Allow 5 digits
	snprintf(export_cmd, sizeof(export_cmd), CMD, atoi(argv[1]));
	system(export_cmd);
	return 0;
}
EOF
gcc gpio_export.c -o gpio_export
sudo chown root:root gpio_export
sudo chmod 4755 gpio_export
rm gpio_export.c
