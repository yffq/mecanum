#include "BatteryMonitor.h"
#include "AddressBook.h"

#include <Arduino.h>

BatteryMonitor::BatteryMonitor() :
	FiniteStateMachine(FSM_BATTERYMONITOR, reinterpret_cast<uint8_t*>(&m_params), sizeof(m_params)),
	m_maxLevel(4), m_currentLevel(0)
{
	m_led[0] = LED_BATTERY_EMPTY;
	m_led[1] = LED_BATTERY_LOW;
	m_led[2] = LED_BATTERY_MEDIUM;
	m_led[3] = LED_BATTERY_HIGH;

	for (int i = 0; i < BATTERYMONITOR_NUM_LED; ++i)
	{
		pinMode(m_led[i], OUTPUT);
		digitalWrite(m_led[i], LOW);
	}
}

BatteryMonitor *BatteryMonitor::NewFromArray(const TinyBuffer &params)
{
	return Validate(params.Buffer(), params.Length()) ? new BatteryMonitor() : NULL;
}

BatteryMonitor::~BatteryMonitor()
{
	for (char i = 0; i < BATTERYMONITOR_NUM_LED; ++i)
		digitalWrite(m_led[i], LOW);
}

uint32_t BatteryMonitor::Step()
{
	if (++m_currentLevel > m_maxLevel)
	{
		// Turn off all the LEDs
		m_currentLevel = 0;
		for (char i = 0; i < BATTERYMONITOR_NUM_LED; ++i)
			digitalWrite(m_led[i], LOW);
	}
	else
	{
		// Turn on the new LED
		digitalWrite(m_led[m_currentLevel - 1], HIGH);
	}

	// Calculate the delay (pause on the actual battery level)
	if (m_currentLevel == m_maxLevel)
		return 900;
	else
		return 80;
}
