#ifndef MECANUMMASTER_H
#define MECANUMMASTER_H

#include "FSMVector.h"

class MecanumMaster
{
public:
	MecanumMaster();

	void Spin();

private:
	void SerialCallback();

	FSMVector fsmv;
	// Previously this was an int[]. The Arduino would crash after 32 seconds.
	// 32,767 ms is half of 65,355; 65,355 is the upper limit of a 2-byte int...
	// Coincidence??
	unsigned long fsmDelay[FSMVector::MAX_FSM];

	// Buffer to receive serial data
	static const int bufferLength = 20;
	char buffer[bufferLength];
};

#endif // MECANUMMASTER_H
