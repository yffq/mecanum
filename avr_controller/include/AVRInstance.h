#ifndef AVRINSTANCE_H
#define AVRINSTANCE_H

#include "FSMContainer.h"

#include <vector>

class AVRInstance
{
public:
	/**
	 * Returns true if fsmv contains fsm.
	 */
	bool Contains(const FSMContainer &fsm) const { return Contains(v_fsm, fsm); }
	static bool Contains(const std::vector<FSMContainer> &fsmv, const FSMContainer &fsm);

	void Add(const FSMContainer &fsm) { v_fsm.push_back(fsm); }
	void AddMany(const std::vector<FSMContainer> &fsmv);
	void Remove(const FSMContainer &fsm);

	size_t Size() const { return v_fsm.size(); }
	void Clear() { v_fsm.clear(); }

	const std::vector<FSMContainer> &GetAll() const { return v_fsm; }

	/**
	 * Get a (unique) list of FSMs registered for the specified pin (selected
	 * from subset of FSMs that can be registered to a pin).
	 *
	 * @return true if a FSM was added to fsmv
	 */
	bool GetByPin(unsigned char pin, std::vector<FSMContainer> &result) const;

	/**
	 * Get a (unique) list of FSMs by their ID.
	 *
	 * @return true if a FSM was added to fsmv
	 */
	bool GetByID(unsigned char fsm_id, std::vector<FSMContainer> &result) const;

	bool GetConflicts(const FSMContainer &fsm, std::vector<FSMContainer> &result) const;

private:
	std::vector<FSMContainer> v_fsm;
};


#endif // AVRINSTANCE_H
