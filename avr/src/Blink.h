#ifndef BLINK_H
#define BLINK_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <stdint.h>

/**
 * Blink a light by setting a digital pin high and low repeatedly.
 *
 * Parameters:
 * ---
 * uint8  id
 * uint8  pin (IsDigital)
 * uint32 delay
 * ---
 */
class Blink : public FiniteStateMachine, public ParamServer::Blink
{
public:
	/**
	 * Create a new blinker.
	 *
	 * @param pin The digital pin, supposedly connected to an LED
	 * @param delay The delay; the blinker's period is twice the delay
	 */
	Blink(uint8_t pin, uint32_t delay /* ms */);

	static Blink *NewFromArray(const TinyBuffer &params);

	/*
	 * When this blinker is destructed, the pin is pulled low as a post-
	 * condition.
	 */
	virtual ~Blink();

	virtual uint32_t Step();

private:
	bool m_enabled;
};

#endif // BLINK_H
