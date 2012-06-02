#include "AnalogPublisher.h"
#include "AddressBook.h"

#include <Arduino.h>

AnalogPublisher::AnalogPublisher(uint8_t pin, unsigned long delay)
	: FiniteStateMachine(FSM_ANALOGPUBLISHER), m_pin(pin), m_delay(delay)
{
}

void AnalogPublisher::Step()
{
	int value = analogRead(m_pin);
	uint8_t msg[5];
	msg[0] = sizeof(msg);
	msg[1] = FSM_ANALOGPUBLISHER;
	msg[2] = m_pin;
	msg[3] = value >> 8;
	msg[4] = value & 0xFF;
	Serial.write(msg, sizeof(msg));
}

bool AnalogPublisher::Message(const char* msg, unsigned char length)
{
	// Verify that the message was intended for us
	return length >= 1 && m_pin == static_cast<uint8_t>(msg[0]);
}
