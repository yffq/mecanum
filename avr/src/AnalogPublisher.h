#ifndef ANALOGPUBLISHER_H
#define ANALOGPUBLISHER_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <stdint.h> // for uint8_t

/**
 * Broadcast the state of an analog pin over serial at the given frequency
 * (approximately).
 *
 * Parameters:
 * ---
 * uint8  ID
 * uint8  Pin (IsAnalog)
 * uint32 Delay
 * ---
 *
 * Message:
 * ---
 * uint8 Pin
 * uint8 HighByte  # value >> 8)
 * uint8 LowByte   # value & 0xFF)
 * ---
 */
class AnalogPublisher : public FiniteStateMachine, public ParamServer::AnalogPublisher
{
public:
	AnalogPublisher(uint8_t pin, unsigned long delay);

	static AnalogPublisher *NewFromArray(const TinyBuffer &params);

	virtual ~AnalogPublisher() { }

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

	/**
	 * By specifying a long delay, this publisher becomes a service. When a
	 * message is sent to it (and the message's pin matches its pin), it will
	 * emit the analog value to the serial port on command.
	 */
	virtual bool Message(const TinyBuffer &msg);

private:
	unsigned long m_delay; // ms
};

#endif // ANALOGPUBLISHER_H
