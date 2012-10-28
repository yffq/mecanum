#include "AVRController.h"
#include "ParamServer.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
	AVRController arduino;

	std::string port = "/dev/ttyACM0";

	cout << "Using port " << port << endl;
	arduino.Open(port);

	ParamServer::ToggleSubscriberMsg toggle(LED_BATTERY_EMPTY, 2);
	string strMsg(reinterpret_cast<const char*>(toggle.GetBuffer()), toggle.GetLength());
	arduino.Send(strMsg);

	boost::this_thread::sleep(boost::posix_time::seconds(4000));

	cout << "Closing port..." << endl;

	return 0;
}
