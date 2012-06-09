#include "AVRController.h"

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
	AVRController arduino;

	std::string port = "/dev/ttyACM0";

	cout << "Using port " << port << endl;
	arduino.Open(port);

	cout << "Closing port..." << endl;

	return 0;
}
