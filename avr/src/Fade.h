#ifndef FADE_H
#define FADE_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <stdint.h> // for uint8_t

/**
 * Fade a light on a PWM pin.
 *
 * Parameters:
 * ---
 * uint8  ID
 * uint8  Pin (IsPWM)
 * uint8  Curve  # Linear (0) or Logarithmic (1)
 * utin32 Period
 * uint32 Delay
 * ---
 */
class Fade : public FiniteStateMachine, public ParamServer::Fade
{
public:
	enum Direction
	{
		UP, // brighter
		DOWN // dimmer
	};

	/**
	 * Personally, I prefer LOGARITHMIC because it has a bolder appearance.
	 */
	// enum LuminanceCurve
	static const unsigned char LINEAR = 0; // luminance increases linearly
	static const unsigned char LOGARITHMIC = 1; // voltage increases linearly

	/**
	 * Create a new fader on the specified pin.
	 *
	 * @param pin The PWM pin, supposedly connected to an LED
	 * @param period (ms) The brightness goes from 0 to 255 to 0 in one period
	 * @param delay The reciprocal of the update frequency, in ms. A delay
	 *     of 50ms would update 20 times per second.
	 * @param curve If LINEAR, the luminance will gradually increase. If
	 *     LOGARITHMIC, the luminance will increase faster near 0 and slower
	 *     near full brightness.
	 */
	Fade(uint8_t pin, unsigned long period, unsigned long delay, unsigned char curve = LINEAR);

	/**
	 * Performs parameter validation and instantiates a new object. If the
	 * parameters are invalid or allocation fails, this function returns 0.
	 */
	static Fade *NewFromArray(const TinyBuffer &params);

	/**
	 * When this fader is destructed, the pin is pulled low as a post-
	 * condition.
	 */
	virtual ~Fade();

	/**
	 * StepNoVTable() allows us to bypass the VTable if we have a Fade pointer.
	 */
	inline virtual void Step() { StepAwayFromTheVTable(); }
	void StepAwayFromTheVTable();

	virtual unsigned long Delay() const { return m_delay; }

	/**
	 * Calling Enable(false) will freeze the fader (Step() does nothing).
	 */
	void Enable(bool enable = true) { m_enabled = enable; }

	bool IsEnabled() { return m_enabled; }

	/**
	 * This will return the current "position" of the fader. The position runs
	 * on a linear scale from 0 to 255. At 255, the direction reverses and the
	 * fader's position decreases to 0. Because apparent brightness doesn't
	 * correspond identically to voltage, the actual value being written to the
	 * LED might be different than this linear value.
	 *
	 * Before reversing directions, the fader is guaranteed to visit 0 and 255
	 * at least once. If none of the Set*() functions are called, this will be
	 * exactly once per change in direction.
	 *
	 * @return 0-255
	 */
	int GetBrightness() { return m_brightness; }

	/**
	 * The direction will reverse if and only if a brightness of 0 or 255 is
	 * visited.
	 */
	Direction GetDirection() { return m_dir; }

	/**
	 * This will not set the brightness of the LED. It will set the position of
	 * the fader; after Step() is called, the brightness will be one step
	 * higher or one step lower than this brightness.
	 */
	void SetBrightness(int brightness) { m_brightness = brightness; }

	/**
	 * This controls the direction that the brightness is incremented on the
	 * next Step(). If the brightness goes out of bounds, the direction will be
	 * reversed (possibly canceling out this function).
	 */
	void SetDirection(Direction dir) { m_dir = dir; }

private:
	Direction m_dir;
	// Previously, this was uint8_t (which makes sense, as it can only be
	// 0-255). However, for whatever reason, analogWrite() expects an int,
	// causing overflow problems.
	int m_brightness;
	int m_brightnessStep;
	unsigned long m_delay; // ms
	bool m_enabled;
};

#endif // FADE_H
