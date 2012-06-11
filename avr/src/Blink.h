#ifndef BLINK_H
#define BLINK_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <stdint.h> // for uint8_t

/**
 * Blink a light by setting a digital pin high and low repeatedly.
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
	Blink(uint8_t pin, unsigned long delay /* ms */);

	static Blink *NewFromArray(const ByteArray &params);

	/*
	 * When this blinker is destructed, the pin is pulled low as a post-
	 * condition.
	 */
	virtual ~Blink();

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

private:
	unsigned long m_delay; // ms
	bool m_enabled;
};

#endif // BLINK_H
