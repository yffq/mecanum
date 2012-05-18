#include "Blink.h"
#include "FSMVector.h"
#include "hardware_interface.h"

#include <Arduino.h>

FSMVector fsmv;
// Previously this was an int[]. The Arduino would crash after 32 seconds.
// 32,767 ms is half of 65,355. 65,355 is the upper limit of a 2-byte int.
// Coincidence??
unsigned long fsm_delay[FSMVector::MAX_FSM];


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
	// Test FSMs
	fsmv.PushBack(new Blink(LED_BATTERY_EMPTY, 250));
	fsmv.PushBack(new Blink(LED_BATTERY_LOW, 250));
	fsmv.PushBack(new Blink(LED_BATTERY_MEDIUM, 250));
	fsmv.PushBack(new Blink(LED_BATTERY_HIGH, 250));
	fsmv.Erase(0);
}

void loop()
{
	for (;;)
	{
		for (int i = 0; i < fsmv.GetSize(); ++i)
		{
			// Wait until the delay has elapsed
			if (fsm_delay[i] <= millis())
			{
				fsmv[i]->Step();
				fsm_delay[i] = fsmv[i]->Delay() + millis();
			}
		}
	}
}
