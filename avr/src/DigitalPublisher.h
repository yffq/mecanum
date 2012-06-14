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
 * uint8  Pin (IsDigital)
 * uint32 Delay
 * ---
 *
 * Message:
 * ---
 * uint8 Pin
 * uint8 State  # 0 or 1
 * ---
 */
class DigitalPublisher : public FiniteStateMachine, ParamServer::DigitalPublisher
{
public:
	DigitalPublisher(uint8_t pin, unsigned long delay /* ms */);

	static DigitalPublisher *NewFromArray(const TinyBuffer &params);

	virtual ~DigitalPublisher() { }

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

	/**
	 * By specifying a long delay, this publisher becomes a service. When a
	 * message is sent to it (and the message's pin matches its pin), it will
	 * emit the digital value to the serial port on command.
	 */
	virtual bool Message(const TinyBuffer &msg);

private:
	unsigned long m_delay; // ms
};

#endif // DIGITALPUBLISHER_H
