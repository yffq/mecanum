#include "Fade.h"

#include <Arduino.h>

Fade::Fade(uint8_t pin, unsigned long period, unsigned long updateFrequency) :
	m_pin(pin), m_dir(true), m_brightness(0), m_delay(1000 / updateFrequency)
{
	m_brightnessStep = 255 * (period / 2) * 1000 / m_delay;
	pinMode(pin, OUTPUT);
	analogWrite(m_pin, 0);
}

void Fade::Step()
{
	// Use half the period to calculate brightness increments
	if (m_dir)
	{
		m_brightness += m_brightnessStep;
		if (m_brightness > 255)
		{
			m_brightness = 255;
			m_dir = false; // down
		}
	}
	else
	{
		m_brightness -= m_brightnessStep;
		if (m_brightness < 0)
		{
			m_brightness = 0;
			m_dir = true; // up
		}
	}

	analogWrite(m_pin, m_brightness);
}
