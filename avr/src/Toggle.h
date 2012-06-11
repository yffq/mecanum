#ifndef TOGGLE_H
#define TOGGLE_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <stdint.h> // for uint8_t

/**
 * Toggle a digital pin using the Message() function.
 */
class Toggle : public FiniteStateMachine, public ParamServer::Toggle
{
public:
	/**
	 * Create a new toggle.
	 *
	 * @param delay The time between polling for messages.
	 */
	Toggle(uint8_t pin, unsigned long delay /* ms */);

	static Toggle *NewFromArray(const ByteArray &params);

	/*
	 * When this FSM is destructed, the pin is pulled low as a post-condition.
	 */
	virtual ~Toggle();

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

	/**
	 * The state can be changed with a message. The first byte must be this
	 * FSM's pin. If provided, the second byte is a bool to enable/disable the
	 * pin. If the bool is omitted, the pin's state will be toggled.
	 */
	virtual bool Message(const char* msg, unsigned char length);

private:
	bool m_enabled;
	unsigned long m_delay; // ms
};

#endif // TOGGLE_H
