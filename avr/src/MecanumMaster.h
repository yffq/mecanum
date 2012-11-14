/*
 *        Copyright (C) 2112 Garrett Brown <gbruin@ucla.edu>
 *
 *  This Program is free software; you can redistribute it and/or modify it
 *  under the terms of the Modified BSD License.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the organization nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 *  This Program is distributed AS IS in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#pragma once

#include "FSMVector.h"

class Encoder;

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
	 * Set the baud rate and timeout duration for our serial communication, and
	 * create initial FSMs.
	 */
	void Init();

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

	// A pointer to an Encoder. The Update() function is called every frame.
	Encoder *m_encoder;

	// Buffer to send and receive serial data. Must be <= 0xFE
	static const unsigned int BUFFERLENGTH = 512;
	uint8_t buffer_bytes[BUFFERLENGTH];
};
