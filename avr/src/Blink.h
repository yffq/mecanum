#ifndef BLINK_H
#define BLINK_H

#include "FiniteStateMachine.h"

#include <stdint.h>

//class FiniteStateMachine; // Can I get away with using this line instead of the #include?

class Blink : public FiniteStateMachine
{
public:
	Blink(uint8_t pin, int delay /* ms */);

	virtual ~Blink() { }

	virtual void Step();

	virtual int Delay() const { return m_delay; }

private:
	uint8_t m_pin;
	bool m_enabled;
	int m_delay; // ms
};

#endif // BLINK_H
