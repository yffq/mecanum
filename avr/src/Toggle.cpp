#include "Toggle.h"

#include "AddressBook.h"

#include <Arduino.h>

#define PARAM_ID    0
#define PARAM_PIN   1
#define PARAM_DELAY 2

Toggle::Toggle(uint8_t pin, unsigned long delay) : m_enabled(false), m_delay(delay)
{
	//  Make the super class aware of our parameters
	m_params[PARAM_ID] = FSM_TOGGLE;
	m_params[PARAM_PIN] = pin;
	ByteArray::Serialize(m_delay, m_params + PARAM_DELAY);
	DeclareParameters(m_params, sizeof(m_params));

	pinMode(m_params[PARAM_PIN], OUTPUT);
	digitalWrite(m_params[PARAM_PIN], LOW);
}

Toggle *Toggle::NewFromArray(const ByteArray &params)
{
	if (params.Length() >= sizeof(m_params) && params[PARAM_ID] == FSM_TOGGLE)
	{
		unsigned long delay;
		ByteArray::Deserialize(&params[PARAM_DELAY], delay);
		return new Toggle(params[PARAM_PIN], delay);
	}
	return 0;
}

Toggle::~Toggle()
{
	digitalWrite(m_params[PARAM_PIN], LOW);
}

void Toggle::Step()
{
	digitalWrite(m_params[PARAM_PIN], m_enabled ? HIGH : LOW);
}

bool Toggle::Message(const char* msg, unsigned char length)
{
	// Verify that the message was intended for us
	if (length >= 1 && m_params[PARAM_PIN] == static_cast<uint8_t>(msg[0]))
	{
		if (length >= 2)
			m_enabled = static_cast<bool>(msg[1]);
		else
			m_enabled = !m_enabled;
		return true;
	}
	return false;
}
