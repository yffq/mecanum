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
#pragma once

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <Servo.h>

/**
 * Track a shaft encoder with an IR sensor.
 */
class Encoder
{
public:
	Encoder(uint8_t pin);

	void Start();
	void Update();

	int Ticks() const { return m_ticks; }

	void Disable();
	bool IsEnabled() const { return m_enabled; }

private:
	void Publish();

	uint8_t m_pin;
	int     m_ticks;
	uint8_t m_state;
	// Use <= 128 samples, otherwise we overflow
	uint8_t m_sampleMessage[16 + 4]; // 128 samples + 4 byte header
	uint8_t m_sampleCount;
	bool    m_enabled;
};


/**
 * Control the five colored LED arrays.
 *
 * Publish:
 * ---
 * uint8 ticks
 * int16 microseconds
 * ---
 */
class Sentry : public FiniteStateMachine
{
public:
	Sentry();

	static Sentry *NewFromArray(const TinyBuffer &params);

	virtual ~Sentry();

	virtual uint32_t Step();

	Encoder *GetEncoder() { return &m_encoder; }

private:
	Encoder m_encoder;
	Servo   m_servo;

	enum State
	{
		SEEKING_MIDPOINT,
		SEEKING_LEFT,
		SEEKING_RIGHT,
		FINISHED
	};
	State m_state;

	int m_target;        // microseconds (pulse length)
	int m_targetMicros;
	//int m_servoLeft;     // microseconds (pulse length)
	//int m_servoRight;    // microseconds (pulse length)
	//int m_servoMidpoint; // microseconds (pulse length)

private:
	ParamServer::Sentry m_params;
};
