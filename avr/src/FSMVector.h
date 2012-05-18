#ifndef FSMVECTOR_H
#define FSMVECTOR_H

#include "FiniteStateMachine.h"


// can I get away with declaring FiniteStateMachine as a class?

#define MAX_FSM 100

class FSMVector
{
public:
	FSMVector() { }

	/**
	 *
	 */
	FiniteStateMachine* &operator[] (const int i) { return m_fsmv[i]; }

	/**
	 *
	 */
	unsigned int GetSize() const { return m_size; }

	/**
	 *
	 */
	unsigned int GetMaxSize() const { return MAX_FSM; }

	/*
	 *
	 */
	void PushBack(FiniteStateMachine *fsm);

	/**
	 *
	 */
	void PopBack();

	/**
	 *
	 */
	void Erase(unsigned int i);

	/**
	 *
	 */
	void QuickErase(unsigned int i);

	/**
	 *
	 */
	void Empty();

private:
	FiniteStateMachine* m_fsmv[MAX_FSM];
	unsigned int m_size;
};

#endif // FSMVECTOR_H
