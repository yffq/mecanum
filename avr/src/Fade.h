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
	 *
	 * @param pin The PWM pin, supposedly connected to an LED
	 * @param delay The delay -- the blinking period is twice the delay
	 */
	Fade(uint8_t pin, unsigned long period /* ms */, unsigned long updateFrequency /* s^-1 */);

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

private:
	uint8_t m_pin;
	bool m_dir; // true = brighter
	uint8_t m_brightness; // 0 to 255
	uint8_t m_brightnessStep;
	unsigned long m_delay; // ms
};

#endif // FADE_H
