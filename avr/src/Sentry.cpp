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
#include "ArduinoAddressBook.h"

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
	pinMode(m_pin, INPUT);
	pinMode(LED_BATTERY_EMPTY, OUTPUT);
	m_ticks = 0;
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

Sentry::Sentry() : m_encoder(ENCODER_PIN), m_state(SEEKING_MIDPOINT)//, m_servoMidpoint(INITIAL_MIDPOINT)
{
	Init(FSM_SENTRY, m_params.GetBuffer());

	m_encoder.Start();
	m_servo.attach(SERVO_PIN);
}

Sentry *Sentry::NewFromArray(const TinyBuffer &params)
{
	return ParamServer::Sentry::Validate(params) ? new Sentry() : (Sentry*)0;
}

uint32_t Sentry::Step()
{
	static int s_targetMicros = INITIAL_MIDPOINT;

	switch (m_state)
	{
	case SEEKING_MIDPOINT:
		{
			m_servo.writeMicroseconds(s_targetMicros);
			m_state = SEEKING_LEFT;
			// Allow 1 second to center the servo
			return 1000;
		}
	case SEEKING_LEFT:
	case SEEKING_RIGHT:
		{
			static int s_targetTicks = 0;

			if (s_targetMicros == INITIAL_MIDPOINT)
			{
				m_encoder.Reset();
			}
			else if (s_targetMicros < 1000 || s_targetMicros > 2000)
			{
				// Publish the number of ticks it took to get here
				ParamServer::SentryPublisherMsg msg;
				msg.SetTicks(m_encoder.Ticks());
				msg.SetMicroseconds(s_targetTicks * NOMINAL_uS_PER_TICK);
				Serial.write(msg.GetBytes(), msg.GetLength());

				s_targetTicks = 0;
				s_targetMicros = INITIAL_MIDPOINT;
				m_servo.writeMicroseconds(s_targetMicros);
				m_state = (m_state == SEEKING_LEFT ? SEEKING_RIGHT : FINISHED);
				return 1000; // 1 second
			}

			if (m_state == SEEKING_LEFT)
				s_targetTicks--;
			else
				s_targetTicks++;

			s_targetMicros = INITIAL_MIDPOINT + s_targetTicks * NOMINAL_uS_PER_TICK;

			m_servo.writeMicroseconds(s_targetMicros);
			return 50;
		}
	case FINISHED:
	default:
		break;
	}
	return 24L * 60L * 60L * 1000L;
}



