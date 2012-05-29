#include "MecanumMaster.h"

#include "AddressBook.h"
#include "Subscribers.h"
#include "Publishers.h"

// Finite state machines
#include "BatteryMonitor.h"
#include "Blink.h"
#include "ChristmasTree.h"
#include "Fade.h"
#include "Mimic.h"
#include "Toggle.h"

#include <Arduino.h> // for millis()


MecanumMaster::MecanumMaster()
{
	// fsmDelay[i] is initialized to 0s, which means far in the past

	// Test FSMs
	//fsmv.PushBack(new BatteryMonitor());
	fsmv.PushBack(new ChristmasTree());
	//fsmv.PushBack(new Blink(LED_STATUS_GREEN, 500)); // reference
	//fsmv.PushBack(new Blink(13, 1000)); // reference
	fsmv.PushBack(new Toggle(LED_BATTERY_EMPTY, 50));
	//fsmv.PushBack(new Mimic(BEAGLEBOARD_BRIDGE1, LED_STATUS_YELLOW, 50));
}

void MecanumMaster::SetupSerial()
{
	Serial.begin(9600);
	Serial.setTimeout(250); // ms
}

void MecanumMaster::Spin()
{
	for (;;)
	{
		while (Serial.available())
			SerialCallback();

		for (int i = 0; i < fsmv.GetSize(); ++i)
		{
			// Wait until the delay has elapsed
			if (fsmDelay[i] <= millis())
			{
				fsmv[i]->Step();
				fsmDelay[i] = fsmv[i]->Delay() + millis();
				Serial.print('x');
			}
		}
	}
}

void MecanumMaster::SerialCallback()
{
	// First character is the size of the subsequent message
	int msgSize = Serial.read();
	
	char msg[1];
	msg[0] = LED_BATTERY_EMPTY;
	if (fsmv.GetById(FSM_TOGGLE))
		fsmv.GetById(FSM_TOGGLE)->Message(msg, 1);
	
	// Block until advertised number of bytes is available (timeout set above)
	//size_t readSize = Serial.readBytes(buffer, msgSize);
	// Single byte is OK - the FSM just gets a message of length 0
	/**
	if (msgSize == 0)
	{
		
	}
	/**
	if (readSize >= 1 && readSize == msgSize)
	{
		// First byte is the ID of the FSM to message
		char fsmId = buffer[0];
		// Send the message to every instance of the FSM
		for (unsigned char i = 0; i < fsmv.GetSize(); ++i)
			fsmv[i]->Message(buffer + 1, msgSize - 1); // skip the ID byte
	}
	else
	{
		// Bad data, notify the host
	}
	/**/
}

