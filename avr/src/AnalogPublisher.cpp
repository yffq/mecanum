#include "AnalogPublisher.h"
#include "AddressBook.h"

#include <Arduino.h>

#define PARAM_ID    0
#define PARAM_PIN   1
#define PARAM_DELAY 2

AnalogPublisher::AnalogPublisher(uint8_t pin, unsigned long delay) : m_delay(delay)
{
	//  Make the super class aware of our parameters
	m_params[PARAM_ID] = FSM_BLINK;
	m_params[PARAM_PIN] = pin;
	ByteArray::Serialize(m_delay, m_params + PARAM_DELAY);
	DeclareParameters(m_params, sizeof(m_params));
}

void AnalogPublisher::Step()
{
	int value = analogRead(m_params[PARAM_PIN]);
	uint8_t msg[5];
	msg[0] = sizeof(msg);
	msg[1] = FSM_ANALOGPUBLISHER;
	msg[2] = m_params[PARAM_PIN];
	msg[3] = value >> 8;
	msg[4] = value & 0xFF;
	Serial.write(msg, sizeof(msg));
}

bool AnalogPublisher::Message(const ByteArray &msg)
{
	// Verify that the message was intended for us
	return msg.Length() >= 1 && m_params[PARAM_PIN] == static_cast<uint8_t>(msg[0]);
}
