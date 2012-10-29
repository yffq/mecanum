#ifndef BATTERYMONITOR_H
#define BATTERYMONITOR_H

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

#endif // BATTERYMONITOR_H
