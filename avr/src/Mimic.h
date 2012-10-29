#ifndef MIMIC_H
#define MIMIC_H

#include "FiniteStateMachine.h"
#include "ParamServer.h"

#include <stdint.h>

/**
 * Mirror the state of a source pin to a destination pin.
 *
 * Parameters:
 * ---
 * uint8  id
 * uint8  source # IsDigital
 * uint8  dest # IsDigital
 * uint32 delay
 * ---
 */
class Mimic : public FiniteStateMachine, public ParamServer::Mimic
{
public:
	Mimic(uint8_t source, uint8_t dest, unsigned long delay /* ms */);

	static Mimic *NewFromArray(const TinyBuffer &params);

	/*
	 * When this FSM is destructed, the destination pin is left at whatever
	 * value the source pin was on the previous step.
	 */
	virtual ~Mimic() { }

	virtual uint32_t Step();
};

#endif // MIMIC_H
