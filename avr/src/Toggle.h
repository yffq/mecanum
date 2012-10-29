#ifndef TOGGLE_H
#define TOGGLE_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <stdint.h>

/**
 * Toggle a digital pin using the Message() function.
 *
 * Parameters:
 * ---
 * uint8  id
 * uint8  pin # IsDigital
 * uint32 delay
 * ---
 *
 * Message state: 0 for off, 1 for on, 2 for toggle
 * Subscribe:
 * ---
 * uint16 length
 * uint8  id
 * uint8  pin
 * uint8  command
 * ---
 */
class Toggle : public FiniteStateMachine, public ParamServer::Toggle
{
public:
	/**
	 * Create a new toggle.
	 */
	Toggle(uint8_t pin);

	static Toggle *NewFromArray(const TinyBuffer &params);

	/*
	 * When this FSM is destructed, the pin is pulled low as a post-condition.
	 */
	virtual ~Toggle();

	virtual uint32_t Step();

	/**
	 * The state can be changed with a message. The first byte must be this
	 * FSM's pin. If provided, the second byte is a bool to enable/disable the
	 * pin. If the bool is omitted, the pin's state will be toggled.
	 */
	virtual bool Message(const TinyBuffer &msg);

private:
	bool m_enabled;
};

#endif // TOGGLE_H
