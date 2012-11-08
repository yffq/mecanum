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

// TODO: Have CMake pull libraries from /usr/share/arduino/libraries
#include <Servo.h>
#include <stdint.h>

/**
 * Sweep a servo across its range of motion.
 *
 * Parameters:
 * ---
 * uint8  id
 * uint8  pin # IsDigital
 * utin32 period
 * uint32 delay
 * ---
 */
class ServoSweep : public FiniteStateMachine, public ParamServer::ServoSweep
{
public:
	enum Direction
	{
		UP,
		DOWN
	};

	/**
	 * Create a new servo sweeper on the specified pin.
	 *
	 * @param pin The pin, supposedly connected to an servo
	 * @param period (ms) The servo goes from 0 to 180 degrees in half a period
	 * @param delay The reciprocal of the update frequency, in ms. A delay
	 *     of 50ms would update 20 times per second.
	 */
	ServoSweep(uint8_t pin, uint32_t period, uint32_t delay);

	/**
	 * Performs parameter validation and instantiates a new object. If the
	 * parameters are invalid or allocation fails, this function returns NULL.
	 */
	static ServoSweep *NewFromArray(const TinyBuffer &params);

	virtual ~ServoSweep();

	virtual uint32_t Step();

private:
	Servo m_servo;

	Direction m_dir;
	int m_angle;
	int m_angleStep;
};
