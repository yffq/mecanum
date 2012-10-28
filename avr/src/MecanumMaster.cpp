#include "MecanumMaster.h"

#include "AddressBook.h"
//#include "Subscribers.h"
//#include "Publishers.h"

// Finite state machines
#include "AnalogPublisher.h"
#include "BatteryMonitor.h"
#include "Blink.h"
#include "ChristmasTree.h"
#include "DigitalPublisher.h"
#include "Fade.h"
#include "Mimic.h"
#include "Toggle.h"

#include <Arduino.h> // for millis()
#include <HardwareSerial.h> // for Serial
#include <limits.h> // for ULONG_MAX

#define FOREVER ULONG_MAX

extern HardwareSerial Serial;

MecanumMaster::MecanumMaster()
{
	// Test FSMs
	fsmv.PushBack(new ChristmasTree());
	//fsmv.PushBack(new AnalogPublisher(BATTERY_VOLTAGE, FOREVER));
	//fsmv.PushBack(new BatteryMonitor());
	fsmv.PushBack(new Toggle(LED_BATTERY_EMPTY));
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
	uint16_t msgSize;
	reinterpret_cast<uint8_t*>(&msgSize)[0] = buffer_bytes[0] = Serial.read();
	reinterpret_cast<uint8_t*>(&msgSize)[1] = buffer_bytes[1] = Serial.read();

	// If msgSize is too large, we have no choice but to drop data
	if (msgSize > BUFFERLENGTH + 2)
		msgSize = BUFFERLENGTH + 2;

	// Block until advertised number of bytes is available (timeout set above)
	size_t readSize = Serial.readBytes(reinterpret_cast<char*>(buffer_bytes + 2), msgSize - 2);

	TinyBuffer msg(buffer_bytes, msgSize);

	// Single byte is OK
	if (readSize >= 1 && readSize + 2 == msgSize)
	{
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
				// If Message() returns true, we should do a Step() and Delay()
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
		// Create a new FSM. msg is the parameters to be passed to the
		// FSM's constructor.
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

		// Repurpose buffer_bytes as a send buffer. Initial length = 3;
		// first byte is msg length, second byte is FSM ID (FSM_MASTER),
		// third byte is msg ID (MSG_MASTER_LIST_FSM)
		TinyBuffer sendBuffer(buffer_bytes, 3);
		sendBuffer[1] = FSM_MASTER;
		sendBuffer[2] = MSG_MASTER_LIST_FSM;

		// Currently, Serial.write() will block until enough data has been
		// written such that the buffer is full. The current buffer size is:
		// #define RX_BUFFER_SIZE 64 in HardwareSerial.cpp (line 44)
		// Note, the buffer size used to be 128 but was changed recently.
		// Also, Serial.write() may someday change to return a value less
		// than the number of bytes specified, indicating that some bytes
		// were dropped.

		for (unsigned char i = 0; i < fsmv.Size(); ++i)
		{
			TinyBuffer fsm(fsmv[i]->Describe());

			// Use unsigned int to avoid integer overflows
			// +1 because size is prepended to fsm byte array
			if (static_cast<unsigned int>(sendBuffer.Length()) +
				static_cast<unsigned int>(fsm.Length()) + 1 > BUFFERLENGTH)
			{
				// Message is too large. Send what we've got.
				sendBuffer[0] = sendBuffer.Length();
				Serial.write(static_cast<uint8_t*>(buffer_bytes), sendBuffer.Length());
				sendBuffer.SetLength(3); // Reset
			}

			fsm.Dump(buffer_bytes + sendBuffer.Length());
			sendBuffer << fsm.Length() + 1; // +1 because size is prepended
		}
		sendBuffer[0] = sendBuffer.Length();
		Serial.write(static_cast<uint8_t*>(buffer_bytes), sendBuffer.Length());

		// Delineate a multi-message list with an empty 3-character response
		sendBuffer[0] = 3;
		Serial.write(static_cast<uint8_t*>(buffer_bytes), 3);

		break;
	}
	default:
		break;
	}
}
