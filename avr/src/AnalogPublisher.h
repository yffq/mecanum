#ifndef ANALOGPUBLISHER_H
#define ANALOGPUBLISHER_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <stdint.h>

/**
 * Broadcast the state of an analog pin over serial at the given frequency
 * (approximately).
 *
 * Parameters:
 * ---
 * uint8  id
 * uint8  pin # IsAnalog
 * uint32 delay
 * ---
 *
 * Publish:
 * ---
 * uint16 length
 * uint8  id
 * uint8  pin
 * uint16 value
 * ---
 *
 * Subscribe:
 * ---
 * uint16 length
 * uint8  id
 * uint8  pin
 * ---
 */
class AnalogPublisher : public FiniteStateMachine, public ParamServer::AnalogPublisher
{
public:
	AnalogPublisher(uint8_t pin, uint32_t delay);

	static AnalogPublisher *NewFromArray(const TinyBuffer &params);

	virtual ~AnalogPublisher() { }

	virtual uint32_t Step();

	/**
	 * By specifying a long delay, this publisher becomes a service. When a
	 * message is sent to it (and the message's pin matches its pin), it will
	 * emit the analog value to the serial port on command.
	 */
	virtual bool Message(const TinyBuffer &msg);
};

#endif // ANALOGPUBLISHER_H
