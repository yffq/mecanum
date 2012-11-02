/*
 *  	  Copyright (C) 2011 Garrett Brown
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
