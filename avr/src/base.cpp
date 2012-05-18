#include "Blink.h"
#include "FSMVector.h" // defines MAX_FSM
#include "hardware_interface.h"

#include <Arduino.h>

FSMVector fsmv;
int fsm_delay[MAX_FSM];

/**
 * Arduino provides the main() function; it looks like this:
 * main() {
 *     setup();
 *     for (;;) {
 *         loop();
 *         do_something_with_hw_serial();
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
}

void loop()
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
