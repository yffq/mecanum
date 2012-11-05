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

