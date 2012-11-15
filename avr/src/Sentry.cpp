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

#define INITIAL_MIDPOINT     1500 // us
#define TICKS                100
// microseconds (pulse length) per tick, assuming 1000us = 120 degrees
#define NOMINAL_uS_PER_TICK  30 // (1000 us / 120 degrees) * (360 degrees / TICKS)


void Encoder::Start()
{
	m_ticks = 0;
	pinMode(m_pin, INPUT);
	pinMode(LED_BATTERY_HIGH, OUTPUT);
	pinMode(LED_BATTERY_EMPTY, OUTPUT);
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
		digitalWrite(LED_BATTERY_EMPTY, m_state);
	}
}

Sentry::Sentry() :
		FiniteStateMachine(FSM_SENTRY, reinterpret_cast<uint8_t*>(&m_params), sizeof(m_params)),
		m_encoder(ENCODER_PIN), m_state(SEEKING_MIDPOINT_1), m_servoMidpoint(INITIAL_MIDPOINT)
{
	m_encoder.Start();
	m_servo.attach(SERVO_PIN);
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
	case SEEKING_MIDPOINT_2:
		{
			m_target = INITIAL_MIDPOINT;
			m_servo.writeMicroseconds(m_target);
			m_state = (m_state == SEEKING_MIDPOINT_1 ? SEEKING_LEFT : SEEKING_RIGHT);
			// Allow 1 second to center the servo
			return 1000;
		}
	case SEEKING_LEFT:
	case SEEKING_RIGHT:
		{
			static int targetTicks = 0;

			if (m_target == INITIAL_MIDPOINT)
			{
				m_encoder.Reset();
				targetTicks++;
				if (m_state == SEEKING_LEFT)
					m_target = INITIAL_MIDPOINT - targetTicks * NOMINAL_uS_PER_TICK;
				else
					m_target = INITIAL_MIDPOINT + targetTicks * NOMINAL_uS_PER_TICK;
			}
			else
			{
				targetTicks++;
				if (!m_encoder.Ticks())
				{
					if (m_state == SEEKING_LEFT)
						m_target = INITIAL_MIDPOINT - targetTicks * NOMINAL_uS_PER_TICK;
					else
						m_target = INITIAL_MIDPOINT + targetTicks * NOMINAL_uS_PER_TICK;
				}
				else
				{
					if (m_state == SEEKING_LEFT)
						// Adjust conversion factor by (targetTicks - 1) / m_encoder.Ticks()
						m_target = INITIAL_MIDPOINT - targetTicks * NOMINAL_uS_PER_TICK * (targetTicks - 1) / m_encoder.Ticks();
					else
						m_target = INITIAL_MIDPOINT + targetTicks * NOMINAL_uS_PER_TICK * (targetTicks - 1) / m_encoder.Ticks();
				}

				if (m_target < 1000 || m_target > 2000)
				{
					digitalWrite(LED_BATTERY_HIGH, HIGH);
					// Outside the servo safe zone, proceed with caution
					static int prevTicks = 0;
					static int prevTarget = 0;

					if (m_encoder.Ticks() == prevTicks)
					{
						// Reached the servo's left limit
						m_state = (m_state == SEEKING_LEFT ? SEEKING_MIDPOINT_2 : FINISHED);
						if (m_state == SEEKING_LEFT)
							m_servoLeft = prevTarget;
						else
							m_servoRight = prevTarget;
						prevTicks = 0;
						targetTicks = 0;
						m_target = INITIAL_MIDPOINT;
						digitalWrite(LED_BATTERY_HIGH, LOW);
					}
					else
					{
						prevTicks = m_encoder.Ticks();
						prevTarget = m_target;
					}
				}
			}
			m_servo.writeMicroseconds(m_target);
			return 30;
		}
	case FINISHED:
	default:
		break;
	}
	return 24L * 60L * 60L * 1000L;
}






