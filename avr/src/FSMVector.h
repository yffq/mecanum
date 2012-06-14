#ifndef FSMVECTOR_H
#define FSMVECTOR_H

#include "FiniteStateMachine.h"

class FSMVector
{
public:
	FSMVector() { }

	~FSMVector() { Clear(); }
	
	/**
	 * Get the FSM by its index in the array. If nothing is erased from the
	 * array in the interim, the order of elements accessed by this operator
	 * will be maintained.
	 */
	FiniteStateMachine *const &operator[] (unsigned char i) const { return m_fsmv[i]; }

	/**
	 * Get the number of FSMs in the array.
	 */
	unsigned char Size() const { return m_size; }

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
	void Erase(unsigned char i);
	void Erase(const TinyBuffer &params) { Erase(GetIndex(params)); }
	void Erase(const FiniteStateMachine &fsm) { Erase(GetIndex(fsm)); }

	/**
	 * Erase the given element in O(1) time. The erased element is simply
	 * replaced by the FSM at the end of the array.
	 */
	void QuickErase(unsigned char i);
	void QuickErase(const TinyBuffer &params) { QuickErase(GetIndex(params)); }
	void QuickErase(const FiniteStateMachine &fsm) { QuickErase(GetIndex(fsm)); }

	/**
	 * Clear the array. Each FSM is deleted and their destructor is called.
	 */
	void Clear() { while (m_size) PopBack(); }

	/**
	 * A constant specifying the maximum number of FSMs this class can store.
	 */
	static const int MAX_FSM = 20;

protected:
	unsigned char GetIndex(const TinyBuffer &params) const;
	unsigned char GetIndex(const FiniteStateMachine &fsm) const;

private:
	// The array
	FiniteStateMachine* m_fsmv[MAX_FSM];
	// The current size
	unsigned char m_size;
};

#endif // FSMVECTOR_H
