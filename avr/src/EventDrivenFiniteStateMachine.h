#ifndef EVENTDRIVENFINITESTATEMACHINE_H
#define EVENTDRIVENFINITESTATEMACHINE_H

#include "FiniteStateMachine.h"
#include <limits.h>

/**
 * An EventDrivenFiniteStateMachine is a FiniteStateMachine that can only
 * transition to the next state on an event by way of the Message() function.
 */
class EventDrivenFiniteStateMachine : public FiniteStateMachine
{
	EventDrivenFiniteStateMachine(unsigned char id) : FiniteStateMachine(id), m_react(false) { }

	/**
	 * Message() allows an external program to influence the current state of
	 * the FSM. This will only be called between Delay() and Step(), and not
	 * between Step() and Delay().
	 *
	 * If the return value is true, Step() will be called on this FSM
	 * immediately. This is useful for event-based FSMs that fire when a serial
	 * command is sent to the FSM's ID.
	 *
	 * As an alternative to frequent polling, a FSM could specify a large delay
	 * and return true from the Message function to have Step() take place on command.
	 *
	 * Currently, only serial commands can trigger a FSM. In the future, there
	 * may be a way added for FSMs to message each other.
	 * TODO: FSMs will need a reference to the MecanumMaster. This reference
	 * could be the global variable master (rename to Master or g_master or
	 * g_Master). A message-builder class should be created to hide the message
	 * parsing details from the FSM. This class could also be used in
	 * MecanumMaster::SerialCallback(); the leading argument against this is
	 * cpu-time and "data-oriented programming". A FSM should not contain a
	 * reference directly to another FSM, because that reference could be
	 * deleted by the master. Also, don't use a message class, use a FSM's
	 * SUBSCRIBER header.
	 */
	virtual bool Message(const char* msg, unsigned char length) = 0;

private:
	bool m_react;
};

#endif // EVENTDRIVENFINITESTATEMACHINE_H
