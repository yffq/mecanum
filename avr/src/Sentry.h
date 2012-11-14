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

class Encoder
{
public:
	Encoder(uint8_t pin) : m_pin(pin), m_ticks(0), m_state(0) { }

	void Start();
	void Update();

	int Ticks() const { return m_ticks; }
	void Reset();

private:
	uint8_t m_pin;
	int     m_ticks;
	uint8_t m_state;
};


/**
 * Control the five colored LED arrays.
 *
 * Parameters:
 * ---
 * uint8  ID
 * ---
 */
class Sentry : public FiniteStateMachine, public ParamServer::Sentry
{
public:
	Sentry();

	static Sentry *NewFromArray(const TinyBuffer &params);

	virtual ~Sentry() { }

	virtual uint32_t Step();

};
