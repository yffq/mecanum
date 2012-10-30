#ifndef MECANUMMASTER_H
#define MECANUMMASTER_H

#include "FSMVector.h"

/**
 * MecanumMaster is the FSM manager. It steps through each FSM, round-robin
 * style, checking serial traffic each on cycle.
 *
 * Messages begin with the 2-byte length (little endian), followed by
 * FSM_MASTER, the message ID, and then the payload (if any). The following
 * messages are available:
 *
 * MSG_MASTER_CREATE_FSM:
 * payload is the fingerprint of the FSM to create
 *
 * MSG_MASTER_DESTROY_FSM:
 * payload is the fingerprint of the FSM to delete
 *
 * MSG_MASTER_LIST_FSM:
 * empty payload, response is a message with the active FSMs as the payload,
 * listed serially. For example:
 *   [11, 0, FSM_MASTER, MSG_MASTER_LIST_FSM, 4, 0, FSM_TOGGLE, LED_BATTERY_FULL,
 *                                            3, 0, FSM_BATTERYMONITOR]
 */
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
	uint8_t buffer_bytes[BUFFERLENGTH];
};

#endif // MECANUMMASTER_H
