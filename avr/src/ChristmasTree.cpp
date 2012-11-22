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

#include "ChristmasTree.h"

#include "ArduinoAddressBook.h"

#define SPEED 1200 // this gives a period of about 1285ms

ChristmasTree::ChristmasTree() : m_state(SpinningStart), m_spinningTarget(0), m_delay(50)
{
	Init(FSM_CHRISTMASTREE, m_params.GetBuffer());

	// TODO: Initialize these on the stack
	fader[0] = new Fade(LED_UV, SPEED, m_delay, Fade::LOGARITHMIC);
	fader[1] = new Fade(LED_RED, SPEED, m_delay, Fade::LOGARITHMIC);
	fader[2] = new Fade(LED_YELLOW, SPEED, m_delay, Fade::LOGARITHMIC);
	fader[3] = new Fade(LED_GREEN, SPEED, m_delay, Fade::LOGARITHMIC);
	fader[4] = new Fade(LED_EMERGENCY, SPEED, m_delay, Fade::LOGARITHMIC);
}

ChristmasTree *ChristmasTree::NewFromArray(const TinyBuffer &params)
{
	return ParamServer::ChristmasTree::Validate(params) ? new ChristmasTree() : (ChristmasTree*)0;
}

ChristmasTree::~ChristmasTree()
{
	// delete checks for null, so we don't need to explicitly check
	for (int i = 0; i < 5; ++i)
		delete fader[i];
}

uint32_t ChristmasTree::Step()
{
	// Look for state transitions
	switch (m_state)
	{
	case OffStart:
	{
		bool done = true;
		for (int i = 0; i < 5; ++i)
		{
			if (fader[i]->GetBrightness() == 0)
				fader[i]->Enable(false);
			else
			{
				// Not done yet, still have another fader to reach 0
				done = false;
				fader[i]->Enable(true);
				fader[i]->SetDirection(Fade::DOWN);
			}
		}
		if (done)
			m_state = Off;
		break;
	}
	case SpinningStart:
	{
		// Rev up the target, dim the rest, then we're ready to spin
		bool done = true;
		for (int i = 0; i < 5; ++i)
		{
			// Check to see if our fader is at the right brightness
			if (fader[i]->GetBrightness() == (i == m_spinningTarget ? 255 : 0))
				fader[i]->Enable(false); // don't go anywhere
			else
			{
				done = false;
				fader[i]->Enable(true);
				fader[i]->SetDirection(i == m_spinningTarget ? Fade::UP : Fade::DOWN);
			}
		}
		if (done)
		{
			fader[m_spinningTarget]->Enable(true);
			// Increment the target on this step, because next step the fader's
			// brightness will no longer be 255
			m_spinningTarget = (m_spinningTarget + 1) % 4;
			m_state = Spinning;
		}
		break;
	}
	case Spinning:
	{
		// Check to see if our target spinner reached full brightness
		if (fader[m_spinningTarget]->GetBrightness() == 255)
			m_spinningTarget = (m_spinningTarget + 1) % 4; // skip emergency fader
		
		// Disable faders that reach 0 (no need to check emergency fader)
		for (int i = 0; i < 4; ++i)
		{
			// Keep the target and non-dimmed faders going
			if (i == m_spinningTarget || fader[i]->GetBrightness() != 0)
				fader[i]->Enable(true);
			else
				fader[i]->Enable(false);
		}
		break;
	}
	case Off:
		// Stay off forever, mwhuahaha
		break;
	default:
		break;
	}

	// Temporary hack to have lights fade out slower than they fade in
	for (int i = 0; i < 5; ++i)
	{
		// We have a Fade pointer, so avoid the vtable where possible
		fader[i]->StepAwayFromTheVTable();
		// Double step to fade the LED in faster. Don't forget that
		// fader[m_spinningTarget] must be dimmer than 255.
		if (m_state == Spinning && i == m_spinningTarget && fader[i]->GetBrightness() < 255)
			fader[i]->StepAwayFromTheVTable();
	}
	return m_delay;
}
