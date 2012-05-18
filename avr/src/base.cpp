#include "Blink.h"
#include "FSMVector.h"
#include "hardware_interface.h"

#include <Arduino.h>
#include <pt.h>

static FSMVector fsmv;
static struct pt ptv[FSMVector::MAX_FSM];
// Previously this was an int[]. The Arduino would crash after 32 seconds.
// 32,767 ms is half of 65,355. 65,355 is the upper limit of a 2-byte int.
// Coincidence??
unsigned long fsm_delay[FSMVector::MAX_FSM];


/**
 * Entry point of each protothread. By treating each FSM as an array of data to
 * be operated upon, we give rise to a SIMD architecture and thus use a single
 * protothread function to process our numerous state machines.
 */
static int protothread(struct pt *pt, int i)
{
	PT_BEGIN(pt);
	while(1)
	{
		fsm_delay[i] = fsmv[i]->Delay() + millis();
		PT_WAIT_UNTIL(pt, fsm_delay[i] <= millis());
		fsmv[i]->Step();
	}
	PT_END(pt);
}

/**
 * Arduino provides the main() function; it looks like this:
 * main() {
 *     setup();
 *     for (;;) {
 *         loop();
 *         if (serialEventRun) serialEventRun();
 *     }
 * }
 */
void setup()
{
	for (int i = 0; i < sizeof(ptv) / sizeof(ptv[0]); ++i)
		PT_INIT(&ptv[i]);

	// Test FSMs
	fsmv.PushBack(new Blink(LED_BATTERY_EMPTY, 250));
	fsmv.PushBack(new Blink(LED_BATTERY_LOW, 250));
	fsmv.PushBack(new Blink(LED_BATTERY_MEDIUM, 250));
	fsmv.PushBack(new Blink(LED_BATTERY_HIGH, 250));
	fsmv.Erase(1);
}

void loop()
{
	for (int i = 0; i < fsmv.GetSize(); ++i)
		protothread(&ptv[i], i);
	/*
	// Basic version without protothreads
	for (int i = 0; i < fsmv.GetSize(); ++i)
	{
		// Wait until the delay has elapsed
		if (fsm_delay[i] <= millis())
		{
			fsmv[i]->Step();
			fsm_delay[i] = fsmv[i]->Delay() + millis();
		}
	}
	*/
}
