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
	void Message(TinyBuffer &msg);

	FSMVector fsmv;
	// Previously this was an int[]. The Arduino would crash after 32 seconds.
	// 32,767 ms is half of 65,355; 65,355 is the upper limit of a 2-byte int...
	// Coincidence??
	unsigned long fsmDelay[FSMVector::MAX_FSM];

	// Buffer to send and receive serial data. Must be <= 0xFE
	static const unsigned int BUFFERLENGTH = 512;
	unsigned char buffer_bytes[BUFFERLENGTH];
};

#endif // MECANUMMASTER_H
