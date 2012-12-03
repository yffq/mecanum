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
#include <digitalWriteFast.h>

#define SERVO_PIN     53
#define ENCODER_PIN   35
#define PROXIMITY_PIN 2 // Analog

#define INITIAL_MIDPOINT     1500 // us
#define TICKS                100
// microseconds (pulse length) per tick, assuming 1000us = 120 degrees
#define NOMINAL_uS_PER_TICK  30 // (1000 us / 24 degrees) * (360 degrees / TICKS)


Encoder::Encoder() : m_ticks(0), m_state(0), m_sampleCount(0), m_enabled(false)
{
	pinMode(ENCODER_PIN, INPUT);
	//pinMode(LED_BATTERY_EMPTY, OUTPUT);

	// Save some CPU cycles later
	m_sampleMessage[1] = 0;
	m_sampleMessage[2] = FSM_ENCODER;
}

void Encoder::Start()
{
	m_ticks = 0;
	m_state = digitalReadFast(ENCODER_PIN);
	m_sampleCount = 0; // Redundant
	m_enabled = true;
}

void Encoder::Update()
{
	if (digitalReadFast(ENCODER_PIN) != m_state)
	{
		m_state = 1 - m_state;
		m_ticks++;
		//digitalWrite(LED_BATTERY_EMPTY, m_state);
	}
	// Clear the byte if newly accessed
	if (m_sampleCount % 8 == 0)
		m_sampleMessage[m_sampleCount / 8 + 4] = m_state;
	else
		m_sampleMessage[m_sampleCount / 8 + 4] |= m_state << (m_sampleCount % 8);

	if (++m_sampleCount == 8 * (sizeof(m_sampleMessage) - 4))
		Publish();
}

void Encoder::Disable()
{
	if (m_enabled)
	{
		m_enabled = false;
		if (m_sampleCount)
			Publish();
	}
}

void Encoder::Publish()
{
	// Only publish what's needed (1 extra byte for every 9th bit)
	m_sampleMessage[0] = 5 + (m_sampleCount - 1) / 8;
	//m_sampleMessage[1] = 0;           // Set in constructor
	//m_sampleMessage[2] = FSM_ENCODER; // Set in constructor
	m_sampleMessage[3] = m_sampleCount;
	Serial.write(m_sampleMessage, m_sampleMessage[0]);

	// Reset the samples array
	m_sampleCount = 0;
}

Sentry::Sentry() : m_state(SEEKING_MIDPOINT), m_targetMicros(INITIAL_MIDPOINT)
{
	Init(FSM_SENTRY, m_params.GetBuffer());
	m_servo.attach(SERVO_PIN);
}

Sentry::~Sentry()
{
	m_servo.detach();
}

Sentry *Sentry::NewFromArray(const TinyBuffer &params)
{
	return ParamServer::Sentry::Validate(params) ? new Sentry() : (Sentry*)0;
}

uint32_t Sentry::Step()
{
	switch (m_state)
	{
	case SEEKING_MIDPOINT:
		{
			m_servo.writeMicroseconds(m_targetMicros);
			m_state = SEEKING_LEFT;
			// Allow 1 second to center the servo
			return 1000;
		}
	case SEEKING_LEFT:
	case SEEKING_RIGHT:
		{
			// Assume we already arrived at the midpoint
			if (m_targetMicros == INITIAL_MIDPOINT)
			{
				m_encoder.Start();
			}

			if (m_targetMicros - NOMINAL_uS_PER_TICK < 800 || m_targetMicros + NOMINAL_uS_PER_TICK > 2200)
			{
				// Publish the number of ticks it took to get here
				ParamServer::SentryPublisherMsg msg;
				msg.SetTicks(m_encoder.Ticks());
				m_encoder.Disable();
				// Center around INITIAL_MIDPOINT
				msg.SetMicroseconds(m_targetMicros - INITIAL_MIDPOINT);
				//Serial.write(msg.GetBytes(), msg.GetLength());

				m_targetMicros = INITIAL_MIDPOINT;
				m_servo.writeMicroseconds(m_targetMicros);
				m_state = (m_state == SEEKING_LEFT ? SEEKING_RIGHT : FINISHED);
				return 1000; // 1 second
			}

			if (m_state == SEEKING_LEFT)
				m_targetMicros -= NOMINAL_uS_PER_TICK;
			else
				m_targetMicros += NOMINAL_uS_PER_TICK;

			m_servo.writeMicroseconds(m_targetMicros);
			return 50;
		}
	case FINISHED:
	default:
		break;
	}
	return 24L * 60L * 60L * 1000L;
}



