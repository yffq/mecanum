#include "DigitalPublisher.h"
#include "AddressBook.h"

#include <Arduino.h>

DigitalPublisher::DigitalPublisher(uint8_t pin, unsigned long delay) :
	FiniteStateMachine(FSM_DIGITALPUBLISHER, m_params, sizeof(m_params)), m_delay(delay)
{
	SetPin(pin);
	SetDelay(m_delay);

	pinMode(pin, INPUT);
}

DigitalPublisher *DigitalPublisher::NewFromArray(const ByteArray &params)
{
	return Validate(params) ? new DigitalPublisher(GetPin(params), GetDelay(params)) : NULL;
}

void DigitalPublisher::Step()
{
	uint8_t msg[4];
	msg[0] = sizeof(msg);
	msg[1] = FSM_DIGITALPUBLISHER;
	msg[2] = GetPin();
	msg[3] = digitalRead(GetPin());
	Serial.write(msg, sizeof(msg));
}

bool DigitalPublisher::Message(const ByteArray &msg)
{
	// Verify that the message was intended for us
	return msg.Length() >= 1 && msg[0] == GetPin();
}
