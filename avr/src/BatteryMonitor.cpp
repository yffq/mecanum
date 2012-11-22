/*
 *        Copyright (C) 2112 Garrett Brown <gbruin@ucla.edu>
 *
 *  This Program is free software; you can redistribute it and/or modify it
 *  under the terms of the Modified BSD License.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the organization nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 *  This Program is distributed AS IS in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "BatteryMonitor.h"
#include "ArduinoAddressBook.h"

#include <Arduino.h>

BatteryMonitor::BatteryMonitor() : m_maxLevel(4), m_currentLevel(0)
{
	Init(FSM_BATTERYMONITOR, m_params.GetBuffer());

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
	return ParamServer::BatteryMonitor::Validate(params) ? new BatteryMonitor() : NULL;
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
