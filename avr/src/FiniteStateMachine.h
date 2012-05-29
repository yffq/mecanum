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
	 * ID of the FSM; used to determine the identify of the derived class.
	 */
	const unsigned char ID;

	/**
	 * The constructor must be called with the FSM's ID. For a list of IDs,
	 * see AddressBook.h.
	 */
	FiniteStateMachine(unsigned char id) : ID(id) { }

	/**
	 * The destructor is declared virtual so that subclasses can optionally
	 * override it to clean up their resources.
	 */
	virtual ~FiniteStateMachine() { }

	/**
	 * Take action and/or transition to the next state.
	 */
	virtual void Step() = 0;

	/**
	 * The amount of time that elapses before the next call to Step(). NOTE:
	 * this essentially predicts how long to remain on the current state. If
	 * the current state has a gate that is non-time-related, the delay will
	 * have to be some small number and Step() will have to check for a state
	 * transition until the event occurs.
	 */
	virtual unsigned long Delay() const = 0;

	/**
	 * Message() allows an external program to influence the current state of
	 * the FSM. This will only be called between Delay() and Step(), and not
	 * between Step() and Delay().
	 */
	virtual void Message(const char* msg, unsigned char count) { }
};

#endif // FINITESTATEMACHINE_H
