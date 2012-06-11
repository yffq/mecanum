#ifndef MIMIC_H
#define MIMIC_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <stdint.h> // for uint8_t

/**
 * Mirror the state of a source pin to a destination pin.
 *
 * Parameters:
 * ---
 * uint8  ID
 * uint8  Source
 * uint8  Dest
 * uint32 Delay
 * ---
 */
class Mimic : public FiniteStateMachine, public ParamServer::Mimic
{
public:
	Mimic(uint8_t source, uint8_t dest, unsigned long delay /* ms */);

	static Mimic *NewFromArray(const ByteArray &params);

	/*
	 * When this FSM is destructed, the destination pin is left at whatever
	 * value the source pin was on the previous step.
	 */
	virtual ~Mimic() { }

	virtual void Step();

	virtual unsigned long Delay() const { return m_delay; }

private:
	unsigned long m_delay; // ms
};

#endif // MIMIC_H
