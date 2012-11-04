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

#include "FSMVector.h"

int FSMVector::PushBack(FiniteStateMachine *fsm)
{
	if (fsm && m_size < MAX_FSM)
	{
		m_fsmv[m_size] = fsm;
		return m_size++; // Return the pre-incremented m_size
	}
	else
	{
		// If fsm doesn't fit in the array, we trash it here because the
		// code invoking PushBack() shouldn't touch the object after it
		// hands it off.
		delete fsm;
		return -1; // Array is full
	}
}

void FSMVector::PopBack()
{
	if (m_size > 0)
		delete m_fsmv[--m_size];
}

void FSMVector::Erase(unsigned char i)
{
	if (0 <= i && i < m_size)
	{
		delete m_fsmv[i];
		while (++i < m_size)
			m_fsmv[i - 1] = m_fsmv[i];
		--m_size;
	}
}

void FSMVector::QuickErase(unsigned char i)
{
	if (0 <= i && i < m_size)
	{
		delete m_fsmv[i];
		// Only swap in the last element if we still have a last element
		if (--m_size > 0)
			m_fsmv[i] = m_fsmv[m_size];
	}
}

unsigned char FSMVector::GetIndex(const TinyBuffer &params) const
{
	for (unsigned char i = 0; i < m_size; ++i)
		if (m_fsmv[i]->Describe() == params)
			return i;
	return -1;
}

unsigned char FSMVector::GetIndex(const FiniteStateMachine &fsm) const
{
	for (unsigned char i = 0; i < m_size; ++i)
		if (*m_fsmv[i] == fsm)
			return i;
	return -1;
}
