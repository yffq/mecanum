#include "DigitalPublisher.h"
#include "AddressBook.h"

#include <Arduino.h>

DigitalPublisher::DigitalPublisher(uint8_t pin, uint32_t delay) :
	FiniteStateMachine(FSM_DIGITALPUBLISHER, reinterpret_cast<uint8_t*>(&m_params), sizeof(m_params))
{
	SetPin(pin);
	SetDelay(delay);

	pinMode(pin, INPUT);
}

DigitalPublisher *DigitalPublisher::NewFromArray(const TinyBuffer &params)
{
	if (Validate(params.Buffer(), params.Length()))
	{
		ParamServer::DigitalPublisher dp(params.Buffer());
		return new DigitalPublisher(dp.GetPin(), dp.GetDelay());
	}
	return NULL;
}

uint32_t DigitalPublisher::Step()
{
	ParamServer::DigitalPublisherPublisherMsg msg(GetPin(), digitalRead(GetPin()));
	Serial.write(msg.GetBuffer(), msg.GetLength());
	return GetDelay();
}

bool DigitalPublisher::Message(const TinyBuffer &msg)
{
	if (msg.Length() == ParamServer::DigitalPublisherSubscriberMsg::GetLength())
	{
		ParamServer::DigitalPublisherSubscriberMsg message(msg.Buffer());
		return message.GetPin() == GetPin();
	}
	return false;
}
