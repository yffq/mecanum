#include "Toggle.h"

#include "AddressBook.h"

#include <Arduino.h>

Toggle::Toggle(uint8_t pin, unsigned long delay)
	: FiniteStateMachine(FSM_TOGGLE), m_pin(pin), m_enabled(false), m_delay(delay)
{
	pinMode(pin, OUTPUT);
	digitalWrite(m_pin, LOW);
}

Toggle::~Toggle()
{
	digitalWrite(m_pin, LOW);
}

void Toggle::Step()
{
	digitalWrite(m_pin, m_enabled ? HIGH : LOW);
}

bool Toggle::Message(const char* msg, unsigned char length)
{
	// Verify that the message was intended for us
	if (length >= 1 && m_pin == static_cast<uint8_t>(msg[0]))
	{
		if (length >= 2)
			m_enabled = static_cast<bool>(msg[1]);
		else
			m_enabled = !m_enabled;
		return true;
	}
	return false;
}
