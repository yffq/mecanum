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

#include <stdint.h>

/**
 * Blink a light by setting a digital pin high and low repeatedly.
 *
 * Parameters:
 * ---
 * uint8  pin (IsDigital)
 * uint32 delay
 * ---
 */
class Blink : public FiniteStateMachine
{
public:
	/**
	 * Create a new blinker.
	 *
	 * @param pin The digital pin, supposedly connected to an LED
	 * @param delay The delay; the blinker's period is twice the delay
	 */
	Blink(uint8_t pin, uint32_t delay /* ms */);

	static Blink *NewFromArray(const TinyBuffer &params);

	/*
	 * When this blinker is destructed, the pin is pulled low as a post-
	 * condition.
	 */
	virtual ~Blink();

	virtual uint32_t Step();

private:
	bool m_enabled;

private:
	ParamServer::Blink m_params;
};
