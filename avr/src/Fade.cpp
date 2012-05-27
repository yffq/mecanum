#include "Fade.h"

#include <Arduino.h>

Fade::Fade(uint8_t pin, unsigned long period, unsigned long delay) :
	m_pin(pin), m_dir(UP), m_brightness(0), m_delay(delay)
{
	// Use half the period to calculate brightness increments
	m_brightnessStep = 255 * delay * 2 / period;
	pinMode(pin, OUTPUT);
	analogWrite(m_pin, 0);
}

void Fade::Step()
{
	if (m_dir == UP)
	{
		m_brightness += m_brightnessStep;
		if (m_brightness >= 255)
		{
			m_brightness = 255;
			m_dir = DOWN;
		}
	}
	else
	{
		m_brightness -= m_brightnessStep;
		if (m_brightness <= 0)
		{
			m_brightness = 0;
			m_dir = UP;
		}
	}

	analogWrite(m_pin, m_brightness);
}
