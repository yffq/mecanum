#include "AVRController.h"
#include "ParamServer.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
	AVRController arduino;

	std::string port = "/dev/ttyACM0";

	arduino.Open(port);

	ParamServer::ToggleSubscriberMsg toggle(LED_BATTERY_EMPTY, 2);
	string strMsg(reinterpret_cast<const char*>(toggle.GetBuffer()), toggle.GetLength());

	arduino.Send(strMsg);
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	arduino.Send(strMsg);
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	arduino.Send(strMsg);
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	return 0;
}
