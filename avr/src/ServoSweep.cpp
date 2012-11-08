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

#include "ServoSweep.h"
#include "ArduinoAddressBook.h"

#include <Arduino.h>
#include <avr/pgmspace.h>

ServoSweep::ServoSweep(uint8_t pin, uint32_t period, uint32_t delay) :
	FiniteStateMachine(FSM_FADE, reinterpret_cast<uint8_t*>(&m_params), sizeof(m_params)),
	m_dir(UP), m_angle(90), m_angleStep(0)
{
	SetPin(pin);
	SetPeriod(period);
	SetDelay(delay);

	// Use the period to calculate degree increments
	m_angleStep = 180 * delay / period;

	m_servo.attach(pin);
	m_servo.write(m_angle);
}

ServoSweep *ServoSweep::NewFromArray(const TinyBuffer &params)
{
	if (Validate(params.Buffer(), params.Length()))
	{
		ParamServer::ServoSweep servoSweep(params.Buffer());
		return new ServoSweep(servoSweep.GetPin(), servoSweep.GetPeriod(), servoSweep.GetDelay());
	}
	return NULL;
}

ServoSweep::~ServoSweep()
{
	m_servo.detach();
}

// So we have the option to avoid hitting the VTable
uint32_t ServoSweep::Step()
{
	if (m_dir == UP)
	{
		m_angle += m_angleStep;
		if (m_angle >= 180)
		{
			m_angle = 180;
			m_dir = DOWN;
		}
	}
	else
	{
		m_angle -= m_angleStep;
		if (m_angle <= 0)
		{
			m_angle = 0;
			m_dir = UP;
		}
	}

	m_servo.write(m_angle);

	return GetDelay();
}
