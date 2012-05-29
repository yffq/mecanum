#include "Blink.h"

#include "AddressBook.h"

#include <Arduino.h>

Blink::Blink(uint8_t pin, unsigned long delay)
	: FiniteStateMachine(FSM_BLINK), m_pin(pin), m_enabled(false), m_delay(delay)
{
	pinMode(pin, OUTPUT);
	digitalWrite(m_pin, LOW);
}

Blink::~Blink()
{
	digitalWrite(m_pin, LOW);
}

void Blink::Step()
{
	if (m_enabled)
	{
		digitalWrite(m_pin, LOW);
		m_enabled = false;
	}
	else
	{
		digitalWrite(m_pin, HIGH);
		m_enabled = true;
	}
}
