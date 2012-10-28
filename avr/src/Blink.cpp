#include "Blink.h"

#include "AddressBook.h"

#include <Arduino.h>

Blink::Blink(uint8_t pin, uint32_t delay) :
	FiniteStateMachine(FSM_BLINK, reinterpret_cast<uint8_t*>(&m_params), sizeof(m_params)),
	m_enabled(false)
{
	SetPin(pin);
	SetDelay(delay);

	// Initialize the blinker
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
}

Blink *Blink::NewFromArray(const TinyBuffer &params)
{
	if (Validate(params.Buffer(), params.Length()))
	{
		ParamServer::Blink blink(params.Buffer());
		new Blink(blink.GetPin(), blink.GetDelay());
	}
	return NULL;
}

Blink::~Blink()
{
	digitalWrite(GetPin(), LOW);
}

uint32_t Blink::Step()
{
	if (m_enabled)
	{
		digitalWrite(GetPin(), LOW);
		m_enabled = false;
	}
	else
	{
		digitalWrite(GetPin(), HIGH);
		m_enabled = true;
	}
	return GetDelay();
}
