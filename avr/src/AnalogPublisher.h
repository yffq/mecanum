#ifndef ANALOGPUBLISHER_H
#define ANALOGPUBLISHER_H

#include "FiniteStateMachine.h"

#include <stdint.h> // for uint8_t

/**
 * DigitalPublisher will broadcast the state of a digital pin over serial at
 * the given frequency (approximately).
 *
 * Message:
 * uint8 length (5)
 * uint8 ID (FSM_ANALOGPUBLISHER)
 * uint8 pin
 * uint8 high byte (value >> 8)
 * uint8 low byte (value & 0xFF)
 */
class AnalogPublisher : public FiniteStateMachine
{
public:
	AnalogPublisher(uint8_t pin, unsigned long delay);

	virtual ~AnalogPublisher() { }

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

	/**
	 * By specifying a long delay, this publisher becomes a service. When a
	 * message is sent to it (and the message's pin matches its pin), it will
	 * emit the analog value to the serial port on command.
	 */
	virtual bool Message(const char* msg, unsigned char length);

private:
	uint8_t m_pin;
	unsigned long m_delay; // ms
};

#endif // ANALOGPUBLISHER_H
