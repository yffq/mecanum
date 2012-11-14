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

#include "Sentry.h"

#include <Arduino.h>

#define SERVO_PIN     35
#define ENCODER_PIN   53
#define PROXIMITY_PIN 8 // Analog

void Encoder::Start()
{
	m_ticks = 0;
	pinMode(m_pin, INPUT);
	m_state = digitalRead(m_pin);
}

void Encoder::Reset()
{
	m_ticks = 0;
	m_state = digitalRead(m_pin);
}

void Encoder::Update()
{
	if (digitalRead(m_pin) != m_state)
	{
		m_state = 1 - m_state;
		m_ticks++;
	}
}

Sentry::Sentry() :
		FiniteStateMachine(FSM_SENTRY, reinterpret_cast<uint8_t*>(&m_params), sizeof(m_params)),
		m_encoder(ENCODER_PIN), m_state(SEEKING_MIDPOINT_1), m_midpoint(1500)
{
	m_encoder.Start();
	m_servo.attach(SERVO_PIN);
	m_target = 1500;
	m_servo.writeMicroseconds(m_target);
}

Sentry *Sentry::NewFromArray(const TinyBuffer &params)
{
	return Validate(params.Buffer(), params.Length()) ? new Sentry() : (Sentry*)0;
}

uint32_t Sentry::Step()
{
	switch (m_state)
	{
	case SEEKING_MIDPOINT_1:
		m_state = SEEKING_LEFT;
		// Allow 1 second to center the servo
		return 1000;
	case SEEKING_LEFT:
		break;
	}
	return 24L * 60L * 60L * 1000L;
}
