#include "Mimic.h"
#include "AddressBook.h"

#include <Arduino.h>

Mimic::Mimic(uint8_t source, uint8_t dest, unsigned long delay) :
	FiniteStateMachine(FSM_MIMIC, m_params, sizeof(m_params)), m_delay(delay)
{
	SetSource(source);
	SetDest(dest);
	SetDelay(delay);

	pinMode(source, INPUT);
	pinMode(dest, OUTPUT);
}

Mimic *Mimic::NewFromArray(const TinyBuffer &params)
{
	return Validate(params) ? new Mimic(GetSource(params), GetDest(params), GetDelay(params)) : NULL;
}

void Mimic::Step()
{
	digitalWrite(GetDest(), digitalRead(GetSource()));
}
