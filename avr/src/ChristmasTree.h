#ifndef CHRISTMASTREE_H
#define CHRISTMASTREE_H

#include "FiniteStateMachine.h"
#include "Fade.h"

#include <stdint.h> // for uint8_t

class ChristmasTree : public FiniteStateMachine
{
public:
	ChristmasTree();

	virtual ~ChristmasTree();

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

private:
	enum State
	{
		Off,
		OffStart,
		Spinning,
		SpinningStart,
		Emergency,
		EmergencyStart
	};

	State m_state;
	uint8_t m_spinningTarget;
	unsigned long m_delay;

	// Starting from the Arduino, going clockwise
	Fade* fader[5];
};

#endif // CHRISTMASTREE_H
