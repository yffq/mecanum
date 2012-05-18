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

	virtual void Step();

	virtual unsigned long Delay() const;

	float GetVoltage() { return 12.75; }

	int GetNumCells() { return GetVoltage() > 8.5 ? 3 : 2; }

private:
	static const int NUM_LED = 4;
	uint8_t m_led[NUM_LED];

	// Current battery level (between 1 and 4)
	int m_maxLevel;
	// Number of LEDs actually lit (between 0 and 4)
	int m_currentLevel;
};

#endif // BATTERYMONITOR_H
