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

	struct
	{
		uint16_t length;
		uint8_t  id; // FSM_MASTER
		uint8_t  command; // MSG_MASTER_LIST_FSM
	} __attribute__((packed)) listFsm = {4, FSM_MASTER, 2};

	string strMsg(reinterpret_cast<const char*>(&listFsm), sizeof(listFsm));
	string response;
	arduino.Query(strMsg, response);

	cout << "---------------------------" << endl;

	static const char *FSM_ID_MAP[] = {
			"Master",
			"BatteryMonitor",
			"Blink",
			"ChristmasTree",
			"Fade",
			"Mimic",
			"Toggle",
			"DigitalPublisher",
			"AnalogPublisher",
	};
	uint16_t length = *reinterpret_cast<const uint16_t*>(response.c_str());
	cout << "  [" << length << ", " << FSM_ID_MAP[(size_t)response[2]];
	for (size_t i = 3; i < response.length(); i++)
		cout << ", " << (unsigned int)response[i];
	cout << "]" << endl;


	/*
	ParamServer::ToggleSubscriberMsg toggle(LED_BATTERY_EMPTY, 2);
	string strMsg(reinterpret_cast<const char*>(toggle.GetBuffer()), toggle.GetLength());

	arduino.Send(strMsg);
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	arduino.Send(strMsg);
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	arduino.Send(strMsg);
	boost::this_thread::sleep(boost::posix_time::seconds(1));
	*/
	return 0;
}
