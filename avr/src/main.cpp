#include "MecanumMaster.h"

#include <Arduino.h>

/**
 * We need to declare master as a global variable. When it was declared locally
 * right before master.Spin() below, the Arduino would glitch out hardcore and
 * turn random LEDs on and off.
 */
MecanumMaster master;

/**
 * Called by Arduino's main() function before loop().
 */
void setup()
{
	// Set up our serial communications
	master.SetupSerial();
}

/**
 * master.Spin() uses an infinite loop to bypass Arduino's main loop. The
 * advantages here are twofold: we avoid the overhead of a function call, and
 * we skip Arduino's call to "if (serialEventRun) serialEventRun();" betwixt
 * every call to loop(). This is desirable because we are using master to
 * process serial data instead of the serialEvent() callback provided by
 * Arduino.
 */
void loop()
{
	master.Spin();
}

