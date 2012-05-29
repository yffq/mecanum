#include "Mimic.h"

#include "AddressBook.h"

#include <Arduino.h>

Mimic::Mimic(uint8_t source, uint8_t dest, unsigned long delay)
	: FiniteStateMachine(FSM_MIMIC), m_source(source), m_dest(dest), m_delay(delay)
{
	pinMode(m_source, INPUT);
	pinMode(m_dest, OUTPUT);
	// Start mimicking immediately
	Step();
}

void Mimic::Step()
{
	digitalWrite(m_dest, digitalRead(m_source));
}
