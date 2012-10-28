#ifndef DIGITALPUBLISHER_H
#define DIGITALPUBLISHER_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <stdint.h> // for uint8_t

/**
 * Broadcast the state of a digital pin over serial at the given frequency
 * (approximately).
 *
 * Parameters:
 * ---
 * uint8  ID
 * uint8  Pin # IsDigital
 * uint32 Delay
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
class DigitalPublisher : public FiniteStateMachine, ParamServer::DigitalPublisher
{
public:
	DigitalPublisher(uint8_t pin, uint32_t delay /* ms */);

	static DigitalPublisher *NewFromArray(const TinyBuffer &params);

	virtual ~DigitalPublisher() { }

	virtual uint32_t Step();

	/**
	 * By specifying a long delay, this publisher becomes a service. When a
	 * message is sent to it (and the message's pin matches its pin), it will
	 * emit the digital value to the serial port on command.
	 */
	virtual bool Message(const TinyBuffer &msg);
};

#endif // DIGITALPUBLISHER_H
