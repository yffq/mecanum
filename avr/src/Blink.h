#ifndef BLINK_H
#define BLINK_H

#include "FiniteStateMachine.h"

#include <stdint.h> // for uint8_t

/**
 * Blink a light by setting a digital pin high and low repeatedly.
 */
class Blink : public FiniteStateMachine
{
public:
	/**
	 * Create a new blinker.
	 *
	 * @param pin The digital pin, supposedly connected to an LED
	 * @param delay The delay -- the blinking period is twice the delay
	 */
	Blink(uint8_t pin, unsigned long delay /* ms */);

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

private:
	uint8_t m_pin;
	bool m_enabled;
	unsigned long m_delay; // ms
};

#endif // BLINK_H
