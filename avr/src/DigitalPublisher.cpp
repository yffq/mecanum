#include "DigitalPublisher.h"
#include "AddressBook.h"

#include <Arduino.h>

DigitalPublisher::DigitalPublisher(uint8_t pin, unsigned long delay)
	: FiniteStateMachine(FSM_DIGITALPUBLISHER), m_pin(pin), m_delay(delay)
{
	pinMode(m_pin, INPUT);
}

void DigitalPublisher::Step()
{
	uint8_t msg[4];
	msg[0] = sizeof(msg);
	msg[1] = FSM_DIGITALPUBLISHER;
	msg[2] = m_pin;
	msg[3] = digitalRead(m_pin);
	Serial.write(msg, sizeof(msg));
}

bool DigitalPublisher::Message(const char* msg, unsigned char length)
{
	// Verify that the message was intended for us
	return length >= 1 && m_pin == static_cast<uint8_t>(msg[0]);
}
