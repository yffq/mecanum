#include <stdlib.h> // for system()
#include <unistd.h> // for setuid()

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		cout << "Incorrect usage: gpio_export pin" << endl;
		return -1;
	}

	string pin(argv[1]);
	bool ret = true;

	ret &= 0 == setuid(0);
	ret &= 0 == system("chmod a+w /sys/class/gpio/export");
	ret &= 0 == system("chmod a+w /sys/class/gpio/unexport");
	ret &= 0 == system(("echo " + pin + " > /sys/class/gpio/export").c_str());
	ret &= 0 == system(("chmod a+w /sys/class/gpio/gpio" + pin + "/value "
                                  "/sys/class/gpio/gpio" + pin + "/direction "
                                  "/sys/class/gpio/gpio" + pin + "/edge").c_str());
	return ret ? 0 : 1;
}
