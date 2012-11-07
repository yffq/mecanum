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

#include "MecanumMaster.h"

#include "ArduinoAddressBook.h"

// Finite state machines
#include "AnalogPublisher.h"
#include "BatteryMonitor.h"
#include "Blink.h"
#include "ChristmasTree.h"
#include "DigitalPublisher.h"
#include "Fade.h"
#include "Mimic.h"
#include "MotorController.h"
#include "Toggle.h"

#include <Arduino.h> // for millis()
#include <HardwareSerial.h> // for Serial
#include <limits.h> // for ULONG_MAX

#define FOREVER (ULONG_MAX / 2) // ~25 days, need some space to add current time

extern HardwareSerial Serial;

MecanumMaster::MecanumMaster()
{
	// Test FSMs
	//fsmv.PushBack(new ChristmasTree());
	fsmv.PushBack(new Fade(LED_RED, 1500, 50));
	//fsmv.PushBack(new AnalogPublisher(BATTERY_VOLTAGE, FOREVER));
	//fsmv.PushBack(new BatteryMonitor());
	//fsmv.PushBack(new Toggle(LED_BATTERY_EMPTY));
	//fsmv.PushBack(new Mimic(BEAGLEBOARD_BRIDGE6, LED_BATTERY_HIGH, 50));
	//fsmv.PushBack(new Blink(LED_BATTERY_HIGH, 250));
	/*
	// Everything on full brightness
	uint8_t leds[] = {
		// PWM LEDs
		LED_GREEN,
		LED_YELLOW,
		LED_RED,
		LED_UV,
		LED_EMERGENCY,
		// Digital LEDs
		LED_STATUS_GREEN,
		LED_STATUS_YELLOW,
		LED_BATTERY_EMPTY,
		LED_BATTERY_LOW,
		LED_BATTERY_MEDIUM,
		LED_BATTERY_HIGH
	};
	for (char i = 0; i < sizeof(leds) / sizeof(leds[0]); ++i)
	{
		pinMode(leds[i], OUTPUT);
		digitalWrite(leds[i], HIGH);
	}
	*/
}

void MecanumMaster::SetupSerial()
{
	Serial.begin(115200);
	Serial.setTimeout(250); // ms
}

void MecanumMaster::Spin()
{
	for (;;)
	{
		// TODO: This needs to read length, and then only read if (length-2) is available
		while (Serial.available())
			SerialCallback();

		for (int i = 0; i < fsmv.Size(); ++i)
		{
			// Wait until the delay has elapsed
			if (fsmDelay[i] <= millis())
			{
				fsmDelay[i] = fsmv[i]->Step() + millis();
			}
		}
	}
}

void MecanumMaster::SerialCallback()
{
	// First word is the size of the entire message
	Serial.readBytes(reinterpret_cast<char*>(buffer_bytes), 2);
	uint16_t msgSize = *reinterpret_cast<uint16_t*>(buffer_bytes);

	// If msgSize is too large, we have no choice but to drop data
	if (msgSize > BUFFERLENGTH + 2)
		msgSize = BUFFERLENGTH + 2;

	// Block until advertised number of bytes is available (timeout set above)
	// Message payload must be at least 1 byte (msgSize >= 1 word + 1 byte)
	size_t readSize = 0;
	if (msgSize >= 3)
		readSize = Serial.readBytes(reinterpret_cast<char*>(buffer_bytes) + 2, msgSize - 2);

	// Single byte (readSize == 1) is OK
	if (msgSize >= 3 && readSize + 2 == msgSize)
	{
		TinyBuffer msg(buffer_bytes, msgSize);

		// First byte after the msg size is the ID of the FSM to message
		uint8_t fsmId = msg[2];
		if (fsmId == FSM_MASTER)
		{
			msg >> 3; // Skip the size and ID bytes
			Message(msg);
		}
		else
		{
			// Send the message to every instance of the FSM
			for (unsigned char i = 0; i < fsmv.Size(); ++i)
			{
				// If Message() returns true, we should do a Step()
				if (fsmv[i]->GetID() == fsmId && fsmv[i]->Message(msg))
				{
					fsmDelay[i] = fsmv[i]->Step() + millis();
				}
			}
		}
	}
	else
	{
		// Bad data, notify the host
	}
}

void MecanumMaster::Message(TinyBuffer &msg)
{
	if (!msg.Length())
		return;

	// Message IDs are defined in AddressBook.h
	unsigned char msgID = msg[0];
	msg >> 1;

	switch (msgID)
	{
	case MSG_MASTER_CREATE_FSM:
	{
		// Create a new FSM. msg is the parameters to be passed to the FSM's constructor
		if (msg.Length())
		{
			unsigned char fsm_id = msg[0];
			switch (fsm_id)
			{
			case FSM_ANALOGPUBLISHER:
				fsmv.PushBack(AnalogPublisher::NewFromArray(msg));
				break;
			case FSM_BATTERYMONITOR:
				fsmv.PushBack(BatteryMonitor::NewFromArray(msg));
				break;
			case FSM_BLINK:
				fsmv.PushBack(Blink::NewFromArray(msg));
				break;
			case FSM_CHRISTMASTREE:
				fsmv.PushBack(ChristmasTree::NewFromArray(msg));
				break;
			case FSM_DIGITALPUBLISHER:
				fsmv.PushBack(DigitalPublisher::NewFromArray(msg));
				break;
			case FSM_FADE:
				fsmv.PushBack(Fade::NewFromArray(msg));
				break;
			case FSM_MIMIC:
				fsmv.PushBack(Mimic::NewFromArray(msg));
				break;
			case FSM_MOTORCONTROLLER:
				fsmv.PushBack(MotorController::NewFromArray(msg));
			case FSM_TOGGLE:
				fsmv.PushBack(Toggle::NewFromArray(msg));
				break;
			}
		}
		break;
	}
	case MSG_MASTER_DESTROY_FSM:
	{
		// Annihilate a FSM. msg is the fingerprint of the FSM to delete
		fsmv.QuickErase(msg);
		break;
	}
	case MSG_MASTER_LIST_FSM:
	{
		// Dump a list of FSMs to the serial port.

		// Repurpose buffer_bytes as a send buffer. Initial length = 4; first
		// word is msg length, third byte is FSM ID (FSM_MASTER), fourth byte
		// is msg ID (MSG_MASTER_LIST_FSM)
		TinyBuffer sendBuffer(buffer_bytes, 4); // Length is filled in at the end
		sendBuffer[2] = FSM_MASTER;
		sendBuffer[3] = MSG_MASTER_LIST_FSM;

		for (unsigned char i = 0; i < fsmv.Size(); ++i)
		{
			TinyBuffer fsm(fsmv[i]->Describe());
			if (sendBuffer.Length() + 2 + fsm.Length() <= BUFFERLENGTH)
			{
				// Dump the fsm at the end of the buffer
				fsm.DumpBuffer(buffer_bytes + sendBuffer.Length());
				sendBuffer << fsm.Length() + 2; // +2 because size is prepended
			}
		}
		*reinterpret_cast<uint16_t*>(buffer_bytes) = sendBuffer.Length();
		Serial.write(buffer_bytes, sendBuffer.Length());
		break;
	}
	default:
		break;
	}
}
