#ifndef FADE_H
#define FADE_H

#include "FiniteStateMachine.h"

#include <stdint.h> // for uint8_t

/**
 * Fade a light on a PWM pin.
 */
class Fade : public FiniteStateMachine
{
public:
	/**
	 * Create a new fader on the specified pin.
	 *
	 * @param pin The PWM pin, supposedly connected to an LED
	 * @param period The brightness goes from 0 to 255 to 0 in one period
	 * @param delay The reciprocal of the update frequency, in ms. A delay of
	 *              50ms would update 20 times per second.
	 */
	Fade(uint8_t pin, unsigned long period /* ms */, unsigned long delay /* ms */);

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

private:
	enum Direction
	{
		UP, // brighter
		DOWN // dimmer
	};

	uint8_t m_pin;
	Direction m_dir; // true = brighter
	// Previously, this was uint8_t (which makes sense, as it can only be
	// 0-255). However, for whatever reason, analogWrite() expects an int,
	// causing overflow problems.
	int m_brightness;
	int m_brightnessStep;
	unsigned long m_delay; // ms
};

#endif // FADE_H
