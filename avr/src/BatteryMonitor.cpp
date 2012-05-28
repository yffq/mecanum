#include "BatteryMonitor.h"
#include "AddressBook.h"

#include <Arduino.h>

BatteryMonitor::BatteryMonitor() : FiniteStateMachine(FSM_BATTERYMONITOR), m_maxLevel(4), m_currentLevel(0)
{
	m_led[0] = LED_BATTERY_EMPTY;
	m_led[1] = LED_BATTERY_LOW;
	m_led[2] = LED_BATTERY_MEDIUM;
	m_led[3] = LED_BATTERY_HIGH;

	for (int i = 0; i < NUM_LED; ++i)
	{
		pinMode(m_led[i], OUTPUT);
		digitalWrite(m_led[i], LOW);
	}
}

void BatteryMonitor::Step()
{
	++m_currentLevel;
	if (m_currentLevel > m_maxLevel)
	{
		// Turn off all the LEDs
		m_currentLevel = 0;
		for (int i = 0; i < NUM_LED; ++i)
			digitalWrite(m_led[i], LOW);
	}
	else
	{
		// Turn on the new LED
		digitalWrite(m_led[m_currentLevel - 1], HIGH);
	}
}

unsigned long BatteryMonitor::Delay() const
{
	// Pause on the actual battery level
	if (m_currentLevel == m_maxLevel)
		return 900;
	else
		return 80;
}
