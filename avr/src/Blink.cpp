#include "Blink.h"

#include "AddressBook.h"

#include <Arduino.h>

#define PARAM_ID    0
#define PARAM_PIN   1
#define PARAM_DELAY 2

Blink::Blink(uint8_t pin, unsigned long delay) : m_delay(delay), m_enabled(false)
{
	//  Make the super class aware of our parameters
	m_params[PARAM_ID] = FSM_BLINK;
	m_params[PARAM_PIN] = pin;
	ByteArray::Serialize(m_delay, m_params + PARAM_DELAY);
	DeclareParameters(m_params, sizeof(m_params));

	// Initialize the blinker
	pinMode(m_params[PARAM_PIN], OUTPUT);
	digitalWrite(m_params[PARAM_PIN], LOW);
}

Blink *Blink::NewFromArray(const ByteArray &params)
{
	if (params.Length() >= sizeof(m_params) && params[PARAM_ID] == FSM_BLINK)
	{
		unsigned long delay;
		ByteArray::Deserialize(&params[PARAM_DELAY], delay);
		return new Blink(params[PARAM_PIN], delay);
	}
	return 0;
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
