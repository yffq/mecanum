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

#include "Blink.h"

#include "ArduinoAddressBook.h"

#include <Arduino.h>

Blink::Blink(uint8_t pin, uint32_t delay) :
	FiniteStateMachine(FSM_BLINK, m_params.GetBuffer()),
	m_enabled(false)
{
	m_params.SetPin(pin);
	m_params.SetDelay(delay);

	// Initialize the blinker
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
}

Blink *Blink::NewFromArray(const TinyBuffer &params)
{
	if (ParamServer::Blink::Validate(params))
	{
		ParamServer::Blink blink(params);
		return new Blink(blink.GetPin(), blink.GetDelay());
	}
	return NULL;
}

Blink::~Blink()
{
	digitalWrite(m_params.GetPin(), LOW);
}

uint32_t Blink::Step()
{
	if (m_enabled)
	{
		digitalWrite(m_params.GetPin(), LOW);
		m_enabled = false;
	}
	else
	{
		digitalWrite(m_params.GetPin(), HIGH);
		m_enabled = true;
	}
	return m_params.GetDelay();
}
