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

#include "Fade.h"
#include "ArduinoAddressBook.h"

#include <Arduino.h>
#include <avr/pgmspace.h>

// Brightness lookup table stored in PROGMEM instead of SRAM
// Table is from http://arduino.cc/forum/index.php?topic=96839.0
const unsigned char luminace[256] PROGMEM =
{
	  0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   4,   4,
	  4,   4,   4,   5,   5,   5,   5,   6,   6,   6,   6,   7,   7,   7,   7,   8,
	  8,   8,   9,   9,   9,  10,  10,  10,  11,  11,  12,  12,  12,  13,  13,  14,
	 14,  15,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21,  22,
	 22,  23,  23,  24,  25,  25,  26,  26,  27,  28,  28,  29,  30,  30,  31,  32,
	 33,  33,  34,  35,  36,  36,  37,  38,  39,  40,  40,  41,  42,  43,  44,  45,
	 46,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,
	 61,  62,  63,  64,  65,  67,  68,  69,  70,  71,  72,  73,  75,  76,  77,  78,
	 80,  81,  82,  83,  85,  86,  87,  89,  90,  91,  93,  94,  95,  97,  98,  99,
	101, 102, 104, 105, 107, 108, 110, 111, 113, 114, 116, 117, 119, 121, 122, 124,
	125, 127, 129, 130, 132, 134, 135, 137, 139, 141, 142, 144, 146, 148, 150, 151,
	153, 155, 157, 159, 161, 163, 165, 166, 168, 170, 172, 174, 176, 178, 180, 182,
	184, 186, 189, 191, 193, 195, 197, 199, 201, 204, 206, 208, 210, 212, 215, 217,
	219, 221, 224, 226, 228, 231, 233, 235, 238, 240, 243, 245, 248, 250, 253, 255
};

Fade::Fade(uint8_t pin, uint32_t period, uint32_t delay, uint8_t curve /* = LINEAR */) :
	FiniteStateMachine(FSM_FADE, m_params.GetBuffer()),
	m_dir(UP), m_brightness(0), m_enabled(true)
{
	m_params.SetPin(pin);
	m_params.SetPeriod(period);
	m_params.SetDelay(delay);
	m_params.SetCurve(curve);

	// Use half the period to calculate brightness increments
	m_brightnessStep = 255 * delay * 2 / period;
	pinMode(pin, OUTPUT);
	analogWrite(pin, 0);
}

Fade *Fade::NewFromArray(const TinyBuffer &params)
{
	if (ParamServer::Fade::Validate(params))
	{
		ParamServer::Fade fade(params);
		return new Fade(fade.GetPin(), fade.GetPeriod(), fade.GetDelay(), fade.GetCurve());
	}
	return NULL;
}

Fade::~Fade()
{
	analogWrite(m_params.GetPin(), 0);
}

// So we have the option to avoid hitting the VTable
uint32_t Fade::StepAwayFromTheVTable()
{
	if (m_enabled)
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

		if (m_params.GetCurve() == LINEAR)
			analogWrite(m_params.GetPin(), pgm_read_byte_near(luminace + m_brightness));
		else
			analogWrite(m_params.GetPin(), m_brightness);
	}
	return m_params.GetDelay();
}
