#include "FSMContainer.h"

using namespace AVR;

FSMContainer::FSMContainer(unsigned char id) throw(std::runtime_error) :
		m_fsm(FSM::NewFromID(id))
{
	if (!m_fsm)
		throw std::runtime_error("Invalid FSM ID");
}

FSMContainer::FSMContainer(const TinyBuffer &buf) throw(std::runtime_error) :
		m_fsm(FSM::NewFromBuffer(buf))
{
	if (!m_fsm)
		throw std::runtime_error("Invalid FSM buffer");
}

FSMContainer::FSMContainer(const FSMContainer &other)
{
	// This shouldn't fail
	m_fsm = FSM::NewFromBuffer(other.m_fsm->GetParams());
	if (!m_fsm)
		throw std::runtime_error("Invalid FSM (something went wrong)");
}

FSMContainer &FSMContainer::operator=(const FSMContainer &src)
{
	if (this != &src)
	{
		// 1. Free all memory in this instance
		delete m_fsm;

		// 2. Reallocate memory from the source instance into this instance
		m_fsm = FSM::NewFromBuffer(src.m_fsm->GetParams());
		if (!m_fsm)
			 throw std::runtime_error("Invalid FSM (something went wrong)");

		// 3. Copy data from source into this instance
		// pass
	}
	return *this;
}
