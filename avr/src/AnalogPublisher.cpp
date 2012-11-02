#include "AnalogPublisher.h"
#include "AddressBook.h"

#include <Arduino.h>


AnalogPublisher::AnalogPublisher(uint8_t pin, uint32_t delay) :
	FiniteStateMachine(FSM_ANALOGPUBLISHER, reinterpret_cast<uint8_t*>(&m_params), sizeof(m_params))
{
	SetPin(pin);
	SetDelay(delay);
}

AnalogPublisher *AnalogPublisher::NewFromArray(const TinyBuffer &params)
{
	if (Validate(params.Buffer(), params.Length()))
	{
		ParamServer::AnalogPublisher ap(params.Buffer());
		return new AnalogPublisher(ap.GetPin(), ap.GetDelay());
	}
	return NULL;
}

uint32_t AnalogPublisher::Step()
{
	ParamServer::AnalogPublisherPublisherMsg msg(GetPin(), analogRead(GetPin()));
	Serial.write(msg.GetBuffer(), msg.GetLength());
	return GetDelay();
}

bool AnalogPublisher::Message(const TinyBuffer &msg)
{
	if (msg.Length() == ParamServer::AnalogPublisherSubscriberMsg::GetLength())
	{
		ParamServer::AnalogPublisherSubscriberMsg message(msg.Buffer());
		return message.GetPin() == GetPin();
	}
	return false;
}
