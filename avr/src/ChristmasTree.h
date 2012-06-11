#ifndef CHRISTMASTREE_H
#define CHRISTMASTREE_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"
#include "Fade.h"

/**
 * Control the five colored LED arrays.
 *
 * Parameters:
 * ---
 * uint8  ID
 * ---
 */
class ChristmasTree : public FiniteStateMachine, public ParamServer::ChristmasTree
{
public:
	ChristmasTree();

	static ChristmasTree *NewFromArray(const ByteArray &params);

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
	// The brightness of fader[m_spinningTarget] is guaranteed to be less than 255
	int m_spinningTarget;
	unsigned long m_delay;

	// Starting from the Arduino, going clockwise
	Fade* fader[5];
};

#endif // CHRISTMASTREE_H
