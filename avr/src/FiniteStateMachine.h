/*
 *        Copyright (C) 2112 Garrett Brown <gbruin@ucla.edu>
 *
 *  This Program is free software; you can redistribute it and/or modify it
 *  under the terms of the Modified BSD License.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the organization nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 *  This Program is distributed AS IS in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#pragma once

#include "TinyBuffer.h"

#include <stdint.h>

/**
 * A FiniteStateMachine represents a model of how a particular component should
 * function. Two functions are required: Step(), which transitions the FSM to
 * the next state, and Delay(), which says how long until the FSM is ready to
 * move to the next state.
 */
class FiniteStateMachine
{
public:
	FiniteStateMachine(uint8_t id, const TinyBuffer &params) : parameters(params)
	{
		parameters[0] = id;
	}

	/**
	 * The destructor is declared virtual so that subclasses can optionally
	 * override it to clean up their resources or fulfill postconditions.
	 */
	virtual ~FiniteStateMachine() { }

	/**
	 * ID of the FSM; used to determine the identify of the derived class.
	 */
	unsigned char GetID() const { return parameters[0]; }

	const TinyBuffer &Describe() const { return parameters; }

	/**
	 * A FSM is uniquely identified by its parameters. A FSM should not alter
	 * parameters after instantiation, because it would in effect transform
	 * itself into a dissimilar FSM.
	 */
	bool operator==(const FiniteStateMachine &other) const { return parameters == other.parameters; }

	/**
	 * Take action and/or transition to the next state.
	 *
	 * The return value is the amount of time that elapses before the next call
	 * to Step(). NOTE: this essentially predicts how long to remain on the
	 * current state. If the current state has a gate that is non-time-related,
	 * the delay will have to be some small number and Step() will have to
	 * check for a state transition until the event occurs.
	 *
	 * As an alternative, a FSM could specify a large delay and return true
	 * from the Message function to have Step() take place on command.
	 */
	virtual uint32_t Step() = 0;

	/**
	 * Message() allows an external program to influence the current state of
	 * the FSM. This will only be called between Delay() and Step(), and not
	 * between Step() and Delay().
	 *
	 * If the return value is true, Step() will be called on this FSM
	 * immediately. This is useful for event-based FSMs that fire when a serial
	 * command is sent to the FSM's ID.
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
	virtual bool Message(const TinyBuffer &msg) { return false; }

private:
	/**
	 * Because a FSM's parameters are an inherent property of the FSM, they
	 * form a sort of "fingerprint" that can be used to identify the FSM. As
	 * such, all FSMs must serialize their parameters into a byte array for
	 * identification purposes.
	 */
	TinyBuffer parameters;
};
