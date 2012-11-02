#include "Mimic.h"
#include "AddressBook.h"

#include <Arduino.h>

Mimic::Mimic(uint8_t source, uint8_t dest, unsigned long delay) :
	FiniteStateMachine(FSM_MIMIC, reinterpret_cast<uint8_t*>(&m_params), sizeof(m_params))
{
	SetSource(source);
	SetDest(dest);
	SetDelay(delay);

	pinMode(source, INPUT);
	pinMode(dest, OUTPUT);
}

Mimic *Mimic::NewFromArray(const TinyBuffer &params)
{
	if (Validate(params.Buffer(), params.Length()))
	{
		ParamServer::Mimic mimic(params.Buffer());
		return new Mimic(mimic.GetSource(), mimic.GetDest(), mimic.GetDelay());
	}
	return NULL;
}

uint32_t Mimic::Step()
{
	digitalWrite(GetDest(), digitalRead(GetSource()));
	return GetDelay();
}
