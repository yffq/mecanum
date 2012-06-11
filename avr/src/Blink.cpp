#include "Blink.h"

#include "AddressBook.h"

#include <Arduino.h>

Blink::Blink(uint8_t pin, unsigned long delay) :
	FiniteStateMachine(FSM_BLINK, m_params, sizeof(m_params)), m_delay(delay), m_enabled(false)
{
	SetPin(pin);
	SetDelay(m_delay);

	// Initialize the blinker
	pinMode(GetPin(), OUTPUT);
	digitalWrite(GetPin(), LOW);
}

Blink *Blink::NewFromArray(const ByteArray &params)
{
	return Validate(params) ? new Blink(GetPin(params), GetDelay(params)) : NULL;
}

Blink::~Blink()
{
	digitalWrite(GetPin(), LOW);
}

void Blink::Step()
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
}
