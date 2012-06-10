#include "Blink.h"

#include "AddressBook.h"
#include "ByteUtils.h"

#include <Arduino.h>

#define PARAM_PIN 1

Blink::Blink(uint8_t pin, unsigned long delay) : m_delay(delay), m_enabled(false)
{
	m_params[0] = FSM_BLINK;
	m_params[1] = pin;
	ByteUtils::Serialize(delay, m_params + 2);

	//  Make the super class aware of our params array
	parameters = ByteArray(m_params, sizeof(m_params));

	Init();
}

Blink::Blink(const ByteArray &params) : m_enabled(false)
{
	for (unsigned char i = 0; i < params.Length(); ++i)
		m_params[i] = params[i];

	// Why do we need a temporary variable here?
	unsigned long n;
	ByteUtils::Deserialize(m_params + 2, n);
	m_delay = n;

	Init();
}

void Blink::Init()
{
	pinMode(m_params[PARAM_PIN], OUTPUT);
	digitalWrite(m_params[PARAM_PIN], LOW);
}

Blink::~Blink()
{
	digitalWrite(m_params[PARAM_PIN], LOW);
}

void Blink::Step()
{
	if (m_enabled)
	{
		digitalWrite(m_params[PARAM_PIN], LOW);
		m_enabled = false;
	}
	else
	{
		digitalWrite(m_params[PARAM_PIN], HIGH);
		m_enabled = true;
	}
}
