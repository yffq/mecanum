#ifndef FSMVECTOR_H
#define FSMVECTOR_H

#include "FiniteStateMachine.h"

class FSMVector
{
public:
	FSMVector() { }

	~FSMVector() { Empty(); }
	
	/**
	 * Get the FSM by its index in the array. If nothing is erased from the
	 * array in the interim, the order of elements accessed by this operator
	 * will be maintained.
	 */
	FiniteStateMachine* &operator[] (const int i) { return m_fsmv[i]; }

	/**
	 * Get the number of FSMs in the array.
	 */
	unsigned int GetSize() const { return m_size; }

	/**
	 * Add a FSM to the end of the array.
	 *
	 * FSMVector is in charge of managing the lifetime of the FSM. Once the FSM
	 * is added to the array, the pointer should fall out of scope. E.g.:
	 *
	 * fsmv.PushBack(new Blink(LED_BATTERY_EMPTY, 250));
	 *
	 * If the array is full (or fsm is NULL), the FSM will be deleted
	 * immediately by PushBack(). Therefore, the existence of the FSM and the
	 * validity of the pointer cannot be guaranteed after this function
	 * returns.
	 *
	 * The return value is the FSM's index in the array (equal to the new size
	 * minus 1). If the array is full or fsm is NULL, -1 is returned.
	 */
	int PushBack(FiniteStateMachine *fsm);

	/**
	 * Erase the last element at the end of the array.
	 */
	void PopBack();

	/**
	 * Erase the element at the given index. This will shift all subsequent
	 * elements over by 1, giving an average runtime of O(N) but maintaining
	 * the FSM ordering.
	 */
	void Erase(unsigned int i);

	/**
	 * Erase the given element in O(1) time. The erased element is simply
	 * replaced by the FSM at the end of the array.
	 */
	void QuickErase(unsigned int i);

	/**
	 * Clear the array. Each FSM is deleted and their destructor is called.
	 */
	void Empty();

	/**
	 * A constant specifying the maximum number of FSMs this class can store.
	 */
	static const int MAX_FSM = 100;

private:
	// The array
	FiniteStateMachine* m_fsmv[MAX_FSM];
	// The current size
	unsigned int m_size;
};

#endif // FSMVECTOR_H
