#ifndef MECANUMMASTER_H
#define MECANUMMASTER_H

#include "FSMVector.h"

class MecanumMaster
{
public:
	MecanumMaster();

	/**
	 * Set the baud rate and timeout duration for our serial communication.
	 */
	void SetupSerial();

	/**
	 * Don't get dizzy.
	 */
	void Spin();

private:
	/**
	 * Fired when serial data is available.
	 */
	void SerialCallback();

	/**
	 * Fired when a new message received.
	 */
	void Message(const char* msg, unsigned char length);

	FSMVector fsmv;
	// Previously this was an int[]. The Arduino would crash after 32 seconds.
	// 32,767 ms is half of 65,355; 65,355 is the upper limit of a 2-byte int...
	// Coincidence??
	unsigned long fsmDelay[FSMVector::MAX_FSM];

	// Buffer to receive serial data
	static const int BUFFERLENGTH = 20;
	char buffer[BUFFERLENGTH];
};

#endif // MECANUMMASTER_H
