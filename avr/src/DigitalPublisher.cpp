#include "DigitalPublisher.h"
#include "AddressBook.h"

#include <Arduino.h>

#define PARAM_ID    0
#define PARAM_PIN   1
#define PARAM_DELAY 2

DigitalPublisher::DigitalPublisher(uint8_t pin, unsigned long delay) : m_delay(delay)
{
	//  Make the super class aware of our parameters
	m_params[PARAM_ID] = FSM_DIGITALPUBLISHER;
	m_params[PARAM_PIN] = pin;
	ByteArray::Serialize(m_delay, m_params + PARAM_DELAY);
	DeclareParameters(m_params, sizeof(m_params));

	pinMode(m_params[PARAM_PIN], INPUT);
}

DigitalPublisher *DigitalPublisher::NewFromArray(const ByteArray &params)
{
	if (params.Length() >= sizeof(m_params) && params[PARAM_ID] == FSM_DIGITALPUBLISHER)
	{
		unsigned long delay;
		ByteArray::Deserialize(&params[PARAM_DELAY], delay);
		return new DigitalPublisher(params[PARAM_PIN], delay);
	}
	return 0;
}

void DigitalPublisher::Step()
{
	uint8_t msg[4];
	msg[0] = sizeof(msg);
	msg[1] = FSM_DIGITALPUBLISHER;
	msg[2] = m_params[PARAM_PIN];
	msg[3] = digitalRead(m_params[PARAM_PIN]);
	Serial.write(msg, sizeof(msg));
}

bool DigitalPublisher::Message(const char* msg, unsigned char length)
{
	// Verify that the message was intended for us
	return length >= 1 && m_params[PARAM_ID] == static_cast<uint8_t>(msg[0]);
}
