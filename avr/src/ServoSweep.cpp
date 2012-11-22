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

ServoSweep::ServoSweep(uint8_t pin, uint32_t delay) :
	FiniteStateMachine(FSM_SERVOSWEEP, GetBuffer()),
	m_dir(UP)
{
	SetPin(pin);
	SetDelay(delay);

	m_servo.attach(pin, 1000, 2000);
}

ServoSweep *ServoSweep::NewFromArray(const TinyBuffer &params)
{
	if (Validate(params))
	{
		ParamServer::ServoSweep servoSweep(params);
		return new ServoSweep(servoSweep.GetPin(), servoSweep.GetDelay());
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
		m_dir = DOWN;
		m_servo.write(180);
	}
	else
	{
		m_dir = UP;
		m_servo.write(0);
	}
	return GetDelay();
}
