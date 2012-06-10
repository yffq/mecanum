#ifndef CHRISTMASTREE_H
#define CHRISTMASTREE_H

#include "FiniteStateMachine.h"
#include "Fade.h"

class ChristmasTree : public FiniteStateMachine
{
public:
	ChristmasTree();

	/**
	 * Performs parameter validation and instantiates a new object. If the
	 * parameters are invalid or allocation fails, this function returns 0.
	 */
	static ChristmasTree *NewFromArray(const ByteArray &params);

	virtual ~ChristmasTree();

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

private:
	// Only param is the ID (FSM_CHRISTMASTREE)
	unsigned char m_params[1];

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
	// The brightness of fader[m_spinningTarget] is guaranteed to be less than 255
	int m_spinningTarget;
	unsigned long m_delay;

	// Starting from the Arduino, going clockwise
	Fade* fader[5];
};

#endif // CHRISTMASTREE_H
