#ifndef FSMCONTAINER_H
#define FSMCONTAINER_H

#include "FSM.h"
#include "TinyBuffer.h"

#include <stdexcept>


class FSMContainer
{
public:
	FSMContainer(unsigned char id) throw(std::runtime_error);

	FSMContainer(const TinyBuffer &buf) throw(std::runtime_error);

	FSMContainer(const FSMContainer &other);

	~FSMContainer() { delete m_fsm; }

	FSMContainer &operator=(const FSMContainer &src);

	/**
	 * Two FSMContainers are equal if their member's parameters are equal (note
	 * that the member can point to two different instances, and these will
	 * still be considered equal).
	 */
	bool operator==(const FSMContainer &other) const { return (*m_fsm) == (*other.m_fsm); }

	/**
	 * The ID of the container is the ID of its member.
	 */
	unsigned char GetID() const { return m_fsm->GetID(); }

	bool IsValid() const { return m_fsm && m_fsm->IsValid(); }

	/**
	 * Retrieves the specified parameter from the FSM's byte array. If the
	 * index exceeds the FSM's length, 0 will be returned, so make sure that
	 * your application properly screens the FSM ID before calling this
	 * function.
	 */
	unsigned char operator[](unsigned int i) const { return m_fsm->GetSize() >= i ? (*m_fsm)[i] : 0; }

	AVR::FSM *GetFSM() const { return m_fsm; }

private:
	AVR::FSM *m_fsm;
};




#endif // FSMCONTAINER_H
