#include "Blink.h"
#include "FSMVector.h" // defines MAX_FSM
#include "hardware_interface.h"

#include <Arduino.h>
#include <pt.h>


static FSMVector fsmv;
static struct pt ptv[MAX_FSM];
static int fsm_delay[MAX_FSM];

/**
 * Entry point of each protothread. By treating each FSM as an array of data to
 * be operated upon, we give rise to a SIMD architecture and thus use a single
 * protothread function to process our numerous state machines.
 */
static int protothread(struct pt *pt)
{
	PT_BEGIN(pt);
	while(1)
	{
		fsm_delay[0] = millis() + fsmv[0]->Delay();
		PT_WAIT_UNTIL(pt, fsm_delay[0] <= millis());
		fsmv[0]->Step();
		PT_END(pt);
	}
}

/**
 * Arduino provides the main() function; it looks like this:
 * main() {
 *     setup();
 *     for (;;) {
 *         loop();
 *         do_something_with_hw_serial();
 *     }
 * }
 * In setup(), we initialize our protothreads.
 */
void setup()
{
	/*
	for (int i = 0; i < sizeof(ptv) / sizeof(ptv[0]); ++i)
		PT_INIT(&ptv[i]);
	/**/
	PT_INIT(&ptv[0]);

	// Test FSMs
	fsmv.PushBack(new Blink(LED_BATTERY_EMPTY, 250));
}

void loop()
{
	/*
	for (int i = 0; i < fsmv.GetSize(); ++i)
		protothread(&ptv[i]);
	/**/
	protothread(&ptv[0]);
}
