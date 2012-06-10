#include "Mimic.h"
#include "AddressBook.h"

#include <Arduino.h>

#define PARAM_ID     0
#define PARAM_SOURCE 1
#define PARAM_DEST   2
#define PARAM_DELAY  3

Mimic::Mimic(uint8_t source, uint8_t dest, unsigned long delay) : m_delay(delay)
{
	//  Make the super class aware of our parameters
	m_params[PARAM_ID] = FSM_MIMIC;
	m_params[PARAM_SOURCE] = source;
	m_params[PARAM_DEST] = dest;
	ByteArray::Serialize(m_delay, m_params + PARAM_DELAY);
	DeclareParameters(m_params, sizeof(m_params));

	pinMode(source, INPUT);
	pinMode(dest, OUTPUT);
}

Mimic *Mimic::NewFromArray(const ByteArray &params)
{
	if (params.Length() >= sizeof(m_params) && params[PARAM_ID] == FSM_MIMIC)
	{
		unsigned long delay;
		ByteArray::Deserialize(&params[PARAM_DELAY], delay);
		return new Mimic(params[PARAM_SOURCE], params[PARAM_DEST], delay);
	}
	return 0;
}

void Mimic::Step()
{
	digitalWrite(m_params[PARAM_DEST], digitalRead(m_params[PARAM_SOURCE]));
}
