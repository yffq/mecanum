#ifndef BATTERYMONITOR_H
#define BATTERYMONITOR_H

#include "FiniteStateMachine.h"

#include <stdint.h> // for uint8_t


/**
 *
 */
class BatteryMonitor : public FiniteStateMachine
{
public:
	/**
	 *
	 */
	BatteryMonitor();

	static BatteryMonitor *NewFromArray(const ByteArray &params);

	/*
	 * When this FSM is destructed the battery LEDs are left in an off state.
	 */
	virtual ~BatteryMonitor();

	virtual void Step();

	virtual unsigned long Delay() const;

	float GetVoltage() { return 12.75; }

	int GetNumCells() { return GetVoltage() > 8.5 ? 3 : 2; }

private:
	unsigned char m_params[1];

	static const unsigned char NUM_LED = 4;
	uint8_t m_led[NUM_LED];

	// Current battery level (between 1 and 4)
	int m_maxLevel;
	// Number of LEDs actually lit (between 0 and 4)
	int m_currentLevel;
};

#endif // BATTERYMONITOR_H
