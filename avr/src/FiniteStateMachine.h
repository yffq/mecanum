#ifndef FINITESTATEMACHINE_H
#define FINITESTATEMACHINE_H

/**
 * A FiniteStateMachine represents
 */
class FiniteStateMachine
{
public:
	virtual ~FiniteStateMachine() { }

	/**
	 *
	 */
	virtual void Step() = 0;

	/**
	 *
	 */
	virtual unsigned long Delay() const = 0;
};

#endif // FINITESTATEMACHINE_H
