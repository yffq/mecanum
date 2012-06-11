#ifndef ANALOGPUBLISHER_H
#define ANALOGPUBLISHER_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <stdint.h> // for uint8_t

/**
 * DigitalPublisher will broadcast the state of a digital pin over serial at
 * the given frequency (approximately).
 *
 * Message:
 * uint8 length (5)
 * uint8 ID (FSM_ANALOGPUBLISHER)
 * uint8 pin
 * uint8 highByte (value >> 8)
 * uint8 lowByte (value & 0xFF)
 */
class AnalogPublisher : public FiniteStateMachine, public ParamServer::AnalogPublisher
{
public:
	AnalogPublisher(uint8_t pin, unsigned long delay);

	static AnalogPublisher *NewFromArray(const ByteArray &params);

	virtual ~AnalogPublisher() { }

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

	/**
	 * By specifying a long delay, this publisher becomes a service. When a
	 * message is sent to it (and the message's pin matches its pin), it will
	 * emit the analog value to the serial port on command.
	 */
	virtual bool Message(const ByteArray &msg);

private:
	unsigned long m_delay; // ms
};

#endif // ANALOGPUBLISHER_H
