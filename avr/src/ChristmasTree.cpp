#include "ChristmasTree.h"

#include "hardware_interface.h"

#ifndef NULL
#define NULL 0
#endif

ChristmasTree::ChristmasTree() : m_state(SpinningStart), m_spinningTarget(0), m_delay(50)
{
	fader[0] = new Fade(LED_UV, 1000, m_delay);
	fader[1] = new Fade(LED_RED, 1000, m_delay);
	fader[2] = new Fade(LED_YELLOW, 1000, m_delay);
	fader[3] = new Fade(LED_GREEN, 1000, m_delay);
	fader[4] = new Fade(LED_EMERGENCY, 1000, m_delay);
}

ChristmasTree::~ChristmasTree()
{
	// delete checks for null, so we don't need to explicitly check
	for (int i = 0; i < 5; ++i)
		delete fader[i];
}

void ChristmasTree::Step()
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
			m_state = Spinning;
		}
		break;
	}
	case Spinning:
	{
		// Check to see if our target spinner reached full brightness
		if (fader[m_spinningTarget]->GetBrightness() == 255)
		{
			// Move on to the next LED (skip emergency fader)
			m_spinningTarget = (m_spinningTarget + 1) % 4;
			fader[m_spinningTarget]->Enable(true);
		}
		// Disable faders that reach 0 (no need to check emergency fader)
		for (int i = 0; 0 < 4; ++i)
		{
			if (i != m_spinningTarget && fader[i]->GetBrightness() == 0)
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

	for (int i = 0; i < 5; ++i)
	{
		// We have a Fade pointer, so avoid the vtable where possible
		fader[i]->StepNoVTable();
	}
}
