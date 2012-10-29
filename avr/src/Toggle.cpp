#include "Toggle.h"

#include "AddressBook.h"

#include <Arduino.h>
#include <limits.h> // for ULONG_MAX

#define FOREVER (ULONG_MAX / 2) // ~25 days, need some space to add current time

Toggle::Toggle(uint8_t pin) :
	FiniteStateMachine(FSM_TOGGLE, reinterpret_cast<uint8_t*>(&m_params), sizeof(m_params)), m_enabled(false)
{
	SetPin(pin);

	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
}

Toggle *Toggle::NewFromArray(const TinyBuffer &params)
{
	if (Validate(params.Buffer(), params.Length()))
	{
		ParamServer::Toggle toggle(params.Buffer());
		new Toggle(toggle.GetPin());
	}
	return NULL;
}

Toggle::~Toggle()
{
	digitalWrite(GetPin(), LOW);
}

uint32_t Toggle::Step()
{
	digitalWrite(GetPin(), m_enabled ? HIGH : LOW);
	return FOREVER;
}

bool Toggle::Message(const TinyBuffer &msg)
{
	if (msg.Length() == ParamServer::ToggleSubscriberMsg::GetLength())
	{
		ParamServer::ToggleSubscriberMsg message(msg.Buffer());
		if (message.GetPin() == GetPin())
		{
			switch (message.GetCommand())
			{
			case 0:
				m_enabled = false;
				break;
			case 1:
				m_enabled = true;
				break;
			case 2:
			default:
				m_enabled = !m_enabled;
				break;
			}
			return true;
		}
	}
	return false;
}
