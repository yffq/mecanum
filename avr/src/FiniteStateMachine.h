#ifndef FINITESTATEMACHINE_H
#define FINITESTATEMACHINE_H

/**
 * A FiniteStateMachine represents a model of how a particular component should
 * function. Two functions are required: Step(), which transitions the FSM to
 * the next state, and Delay(), which says how long until the FSM is ready to
 * move to the next state.
 */
class FiniteStateMachine
{
public:
	/**
	 *
	 */
	virtual void Step() = 0;

	/**
	 *
	 */
	virtual unsigned long Delay() const = 0;

	/**
	 * The destructor is declared virtual so that subclasses can optionally
	 * override it to clean up their resources.
	 */
	virtual ~FiniteStateMachine() { }
};

#endif // FINITESTATEMACHINE_H
