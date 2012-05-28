#include "MecanumMaster.h"

#include "AddressBook.h"
#include "Subscribers.h"
#include "Publishers.h"

// Finite state machines
#include "BatteryMonitor.h"
#include "Blink.h"
#include "ChristmasTree.h"
#include "Fade.h"

#include <Arduino.h> // for millis()
#include <HardwareSerial.h> // for Serial
extern HardwareSerial Serial;

MecanumMaster::MecanumMaster()
{
	// Set up our serial communications
	Serial.begin(115200);
	Serial.setTimeout(250); // ms

	// Test FSMs
	//fsmv.PushBack(new BatteryMonitor());
	fsmv.PushBack(new ChristmasTree());
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
			}
		}
	}
}

void MecanumMaster::SerialCallback()
{
	// First character is the size of the subsequent message
	int msgSize = Serial.read();
	// Block until broadcasted number of bytes is available (timeout set above)
	size_t readSize = Serial.readBytes(buffer, msgSize);
	// Single byte is OK - the FSM just gets a message of length 0
	if (readSize >= 1 && readSize == msgSize)
	{
		// First byte is the ID of the FSM to message
		char fsmId = buffer[0];
		FiniteStateMachine* fsm = fsmv.GetById(fsmId);
		// The message comes after the ID
		if (fsm)
			fsm->Message(buffer + 1, msgSize - 1);
	}
	else
	{
		// Bad data, notify the host
	}
}
