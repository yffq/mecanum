#include "ChristmasTree.h"

#include "hardware_interface.h"

#ifndef NULL
#define NULL 0
#endif

ChristmasTree::ChristmasTree() : m_state(Spinning), m_delay(50)/*
	led({
		LED_UV,
		LED_RED,
		LED_YELLOW,
		LED_GREEN,
		LED_EMERGENCY
	})*/
{
}

void ChristmasTree::Step()
{
	// Look for state transfers
	switch (m_state)
	{
	case Spinning:
		m_state = Spinning;
		break;
	case Off:
	default:
		m_state = Off;
		break;
	}


	for (int i = 0; fader[i] && i < 5; ++i)
	{
		fader[i]->Step();
	}
}
