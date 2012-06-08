#include "MecanumMaster.h"

#include "AddressBook.h"
//#include "Subscribers.h"
//#include "Publishers.h"

// Finite state machines
//#include "AnalogPublisher.h"
//#include "BatteryMonitor.h"
#include "Blink.h"
//#include "ChristmasTree.h"
//#include "DigitalPublisher.h"
//#include "Fade.h"
//#include "Mimic.h"
//#include "Toggle.h"

#include <Arduino.h> // for millis()
#include <HardwareSerial.h> // for Serial

#define FOREVER 1000L * 60L * 60L * 24L * 7L // 1 week

extern HardwareSerial Serial;

MecanumMaster::MecanumMaster()
{
	// Test FSMs
	//fsmv.PushBack(new ChristmasTree());
	//fsmv.PushBack(new AnalogPublisher(BATTERY_VOLTAGE, FOREVER));
	//fsmv.PushBack(new BatteryMonitor());
	//fsmv.PushBack(new Toggle(LED_BATTERY_EMPTY, FOREVER));
	//fsmv.PushBack(new Mimic(BEAGLEBOARD_BRIDGE6, LED_BATTERY_HIGH, 50));
	fsmv.PushBack(new Blink(LED_EMERGENCY, 500));
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
				fsmv[i]->Step();
				fsmDelay[i] = fsmv[i]->Delay() + millis();
			}
		}
	}
}

void MecanumMaster::SerialCallback()
{
	// First character is the size of the entire message
	int msgSize = Serial.read();

	// If msgSize is too large, we have no choice but to drop data
	if (msgSize > BUFFERLENGTH + 1)
		msgSize = BUFFERLENGTH + 1;

	// Block until advertised number of bytes is available (timeout set above)
	size_t readSize = Serial.readBytes(reinterpret_cast<char*>(buffer_bytes), msgSize - 1);

	ByteArray msg(buffer_bytes, static_cast<unsigned char>(readSize));

	// Single byte is OK - the FSM just gets a message of length 0
	if (readSize && readSize == msgSize - 1)
	{
		// First byte is the ID of the FSM to message
		char fsmId = msg[0];
		if (fsmId == FSM_MASTER)
			Message(msg >> 1); // Skip the ID byte
		else
		{
			// Send the message to every instance of the FSM
			for (unsigned char i = 0; i < fsmv.Size(); ++i)
			{
				// If Message() returns true, we should do a Step() and Delay()
				if (fsmv[i]->ID() == fsmId && fsmv[i]->Message(msg))
				{
					fsmv[i]->Step();
					fsmDelay[i] = fsmv[i]->Delay() + millis();
				}
			}
		}
	}
	else
	{
		// Bad data, notify the host
	}
}

void MecanumMaster::Message(ByteArray &msg)
{
	if (!msg.Length())
		return;

	unsigned char msgID = msg[0];
	msg >> 1;

	// Message 0 is create FSM
	// Message 1 is delete FSM
	// Message 2 is dump FSMs
	if (msg.Length() >= 1)
	{
		switch (msgID)
		{
		case 0:
		{
			// Create a new FSM. msg is the parameters to be passed to the
			// FSM's constructor.
			// TODO: Allow deletion of multiple FSMs
			switch (msg[0])
			{
			case FSM_BLINK:
				if (msg.Length() >= 6)
					fsmv.PushBack(new Blink(msg));
				break;
			}
			break;
		}
		case 1:
		{
			// Annihilate a FSM. msg is the fingerprint of the FSM to delete.
			fsmv.QuickErase(msg);
			break;
		}
		case 2:
		{
			// Dump a list of FSMs to the serial port
			// Repurpose buffer_bytes as a send buffer (msg is no longer consistent now)
			ByteArray sendBuffer(buffer_bytes, 1); // first byte is msg length (set later)

			for (unsigned char i = 0; i < fsmv.Size(); ++i)
			{
				// Remember, the first byte is parameter count (length)
				unsigned int peekAhead = fsmv[i]->Describe().Length() + 1;
				// Avoid buffer and integer overflows
				if (static_cast<unsigned int>(sendBuffer.Length()) + peekAhead > BUFFERLENGTH)
				{
					// Clear the buffer and start fresh
					sendBuffer[0] = sendBuffer.Length();
					Serial.flush();
					Serial.write(static_cast<uint8_t*>(buffer_bytes), sendBuffer.Length());
					sendBuffer.SetLength(1); // Reset to 1
				}
				// We have the go-ahead to copy our params into the send buffer
				sendBuffer << peekAhead;
				fsmv[i]->Describe().PrependLength(buffer_bytes + sendBuffer.Length());
			}
			sendBuffer[0] = sendBuffer.Length();
			// Flush the buffer so that our next write doesn't overflow
			Serial.flush();
			Serial.write(static_cast<uint8_t*>(buffer_bytes), sendBuffer.Length());
			break;
		}
		default:
			break;
		}
	}
}
