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
 * Toggle a digital pin using the Message() function.
 *
 * Parameters:
 * ---
 * uint8  pin # IsDigital
 * uint32 delay
 * ---
 *
 * Message state: 0 for off, 1 for on, 2 for toggle
 * Subscribe:
 * ---
 * uint8  pin
 * uint8  command
 * ---
 */
class Toggle : public FiniteStateMachine, public ParamServer::Toggle
{
public:
	/**
	 * Create a new toggle.
	 */
	Toggle(uint8_t pin);

	static Toggle *NewFromArray(const TinyBuffer &params);

	/*
	 * When this FSM is destructed, the pin is pulled low as a post-condition.
	 */
	virtual ~Toggle();

	virtual uint32_t Step();

	/**
	 * The state can be changed with a message. The first byte must be this
	 * FSM's pin. If provided, the second byte is a bool to enable/disable the
	 * pin. If the bool is omitted, the pin's state will be toggled.
	 */
	virtual bool Message(const TinyBuffer &msg);

private:
	bool m_enabled;
};
