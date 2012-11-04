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

#define BATTERYMONITOR_NUM_LED 4

/**
 * Flash the battery LEDs to indicate battery level.
 *
 * Parameters:
 * ---
 * uint8 id
 * ---
 */
class BatteryMonitor : public FiniteStateMachine, ParamServer::BatteryMonitor
{
public:
	/**
	 * BatteryMonitor cycles battery LEDs to show the current battery level.
	 */
	BatteryMonitor();

	static BatteryMonitor *NewFromArray(const TinyBuffer &params);

	/*
	 * When this FSM is destructed the battery LEDs are left in an off state.
	 */
	virtual ~BatteryMonitor();

	virtual uint32_t Step();

	float GetVoltage() { return 12.75; }

	int GetNumCells() { return GetVoltage() > 8.5 ? 3 : 2; }

private:
	uint8_t m_led[BATTERYMONITOR_NUM_LED];

	// Current battery level (between 1 and 4)
	int m_maxLevel;
	// Number of LEDs actually lit (between 0 and 4)
	int m_currentLevel;
};
