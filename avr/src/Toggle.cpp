#include "Toggle.h"

#include "AddressBook.h"

#include <Arduino.h>

Toggle::Toggle(uint8_t pin, unsigned long delay) :
	FiniteStateMachine(FSM_TOGGLE, m_params, sizeof(m_params)), m_enabled(false), m_delay(delay)
{
	SetPin(pin);
	SetDelay(delay);

	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
}

Toggle *Toggle::NewFromArray(const ByteArray &params)
{
	return Validate(params) ? new Toggle(GetPin(params), GetDelay(params)) : NULL;
}

Toggle::~Toggle()
{
	digitalWrite(GetPin(), LOW);
}

void Toggle::Step()
{
	digitalWrite(GetPin(), m_enabled ? HIGH : LOW);
}

bool Toggle::Message(const char* msg, unsigned char length)
{
	// Verify that the message was intended for us
	if (length >= 1 && msg[0] == GetPin())
	{
		if (length >= 2)
			m_enabled = static_cast<bool>(msg[1]);
		else
			m_enabled = !m_enabled;
		return true;
	}
	return false;
}
