#include "MecanumMaster.h"

#include <Arduino.h>

/**
 * Called by Arduino's main() function before loop().
 */
void setup()
{
}

/**
 * master.spin() uses an infinite loop to bypass Arduino's main loop. The
 * advantages here are twofold: we avoid the overhead of a function call, and
 * we skip Arduino's call to "if (serialEventRun) serialEventRun();" betwixt
 * every call to loop(). This is desirable because we are using master to
 * process serial data instead of the serialEvent() callback provided by
 * Arduino.
 */
void loop()
{
	MecanumMaster master;
	master.Spin();
}
