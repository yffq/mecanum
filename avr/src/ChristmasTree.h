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
#include "Fade.h"

#include <stdint.h>

/**
 * Control the five colored LED arrays.
 *
 * Parameters:
 * ---
 * ---
 */
class ChristmasTree : public FiniteStateMachine, public ParamServer::ChristmasTree
{
public:
	ChristmasTree();

	static ChristmasTree *NewFromArray(const TinyBuffer &params);

	virtual ~ChristmasTree();

	virtual uint32_t Step();

private:
	enum State
	{
		Off,
		OffStart,
		Spinning,
		SpinningStart,
		Emergency,
		EmergencyStart
	};

	State m_state;
	// The brightness of fader[m_spinningTarget] is guaranteed to be less than 255
	int m_spinningTarget;
	unsigned long m_delay;

	// Starting from the Arduino, going clockwise
	Fade *fader[5];

private:
	ParamServer::ChristmasTree m_params;
};
