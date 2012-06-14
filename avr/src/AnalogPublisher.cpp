#include "AnalogPublisher.h"
#include "AddressBook.h"

#include <Arduino.h>


AnalogPublisher::AnalogPublisher(uint8_t pin, unsigned long delay) :
	FiniteStateMachine(FSM_ANALOGPUBLISHER, m_params, sizeof(m_params)), m_delay(delay)
{
	SetPin(pin);
	SetDelay(m_delay);
}

AnalogPublisher *AnalogPublisher::NewFromArray(const TinyBuffer &params)
{
	return Validate(params) ? new AnalogPublisher(GetPin(params), GetDelay(params)) : NULL;
}

void AnalogPublisher::Step()
{
	int value = analogRead(GetPin());
	uint8_t msg[5];
	msg[0] = sizeof(msg);
	msg[1] = FSM_ANALOGPUBLISHER;
	msg[2] = GetPin();
	msg[3] = value >> 8;
	msg[4] = value & 0xFF;
	Serial.write(msg, sizeof(msg));
}

bool AnalogPublisher::Message(const TinyBuffer &msg)
{
	// Verify that the message was intended for us
	return msg.Length() >= 1 && msg[0] == GetPin();
}
