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

#include "Thumbwheel.h"

#include <unistd.h> // for usleep()

#define SETTLING_TIME 1000 // ms
#define SETTLING_STEP 100 // ms

bool Thumbwheel::Open()
{
	if (!(m_pin1.Open() && m_pin2.Open() && m_pin4.Open()))
		return false;
	m_pin1.SetDirection(GPIO::IN);
	m_pin2.SetDirection(GPIO::IN);
	m_pin4.SetDirection(GPIO::IN);
	m_pin1.SetEdge(GPIO::BOTH);
	m_pin2.SetEdge(GPIO::BOTH);
	m_pin4.SetEdge(GPIO::BOTH);
	return true;
}

unsigned int Thumbwheel::GetValue()
{
	return (1 - m_pin1.GetValue()) << 0 | (1 - m_pin2.GetValue()) << 1 | (1 - m_pin4.GetValue()) << 2;
}

bool Thumbwheel::Poll(unsigned long timeout, unsigned int &value)
{
	// Poll on pin 1, because the 1's bit changes on every value
	unsigned long duration;
	if (!m_pin1.Poll(timeout, duration))
		return false;
	(void)duration; // silence compiler warning

	// We want to go SETTLING_TIME milliseconds without seeing a new value
	unsigned int oldValue = GetValue();
	unsigned int newValue;
	int timeLeft = SETTLING_TIME;
	do
	{
		usleep(SETTLING_STEP * 1000);
		timeLeft -= SETTLING_STEP;
		newValue = GetValue();
		// If the value changed, use it as the old value and start over
		if (newValue != oldValue)
		{
			oldValue = newValue;
			timeLeft = SETTLING_TIME;
		}

	}
	while (timeLeft > 0);

	value = newValue;
	return true;
}
