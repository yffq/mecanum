#include <Arduino.h>
#include <pt.h>

#include "hardware_interface.h"

static struct pt pt1, pt2;

static int protothread1(struct pt *pt);
//static int protothread2(struct pt *pt);

void setup()
{
	// Configure our pins
	pinMode(LED_BATTERY_EMPTY, OUTPUT);
	pinMode(LED_BATTERY_LOW, OUTPUT);
	pinMode(LED_BATTERY_MEDIUM, OUTPUT);
	pinMode(LED_BATTERY_HIGH, OUTPUT);

	/*
	digitalWrite(LED_BATTERY_EMPTY, LOW);
	digitalWrite(LED_BATTERY_LOW, LOW);
	digitalWrite(LED_BATTERY_MEDIUM, LOW);
	digitalWrite(LED_BATTERY_HIGH, LOW);
	*/

	// Set up our protothreads
	PT_INIT(&pt1);
	//PT_INIT(&pt2);
}

void loop()
{
	protothread1(&pt1);
	//protothread2(&pt2);
}

/*
// protothread1 "local" variables
static int batteryLedsOn[4];
static int batteryLevel = 3; // High
static int batteryState;
static int batteryMillis;
static int i;

static int protothread1(struct pt *pt)
{
	PT_BEGIN(pt);
	while(1)
	{
		// Calculate the battery state
		for (batteryState = 0; batteryState < 4; ++batteryState)
			if (!batteryLedsOn[batteryState])
				break;

		if (batteryState == batteryLevel)
			batteryMillis = millis() + 2000;
		else
			batteryMillis = millis() + 200;

		// Block until the amount of time has elapsed
		PT_WAIT_UNTIL(pt, batteryMillis < millis());

		for (i = 0; i < 4; ++i)
		{
			if (i <= batteryState)
			{
				// Turn on the
			}
			if (batteryLedsOn[i])
		}
		// Update our LEDs
		if (batteryState == batteryLeds)
		{
			// batteryLevel might have changed, so disable all LEDs
			digitalWrite(LED_BATTERY_LOW, LOW);
			digitalWrite(LED_BATTERY_MEDIUM, LOW);
			digitalWrite(LED_BATTERY_HIGH, LOW);
		}
		else if (batteryState == BatteryLow)
		{
			&
		}
	}
	PT_END(pt);
}
*/

static int ledMillis;
static int ledState = 0;

static int protothread1(struct pt *pt)
{
	PT_BEGIN(pt);
	while(1)
	{
		ledMillis = millis() + 1000;

		PT_WAIT_UNTIL(pt, ledMillis < millis());

		//if (ledState == 1)
		//	ledState = 0;
		//else
			ledState = 1;

		digitalWrite(LED_BATTERY_EMPTY, ledState);
		digitalWrite(LED_BATTERY_LOW, ledState);
		digitalWrite(LED_BATTERY_MEDIUM, ledState);
		digitalWrite(LED_BATTERY_HIGH, ledState);

		PT_END(pt);
	}
}

/*
static int protothread2(struct pt *pt)
{
	PT_BEGIN(pt);
	while(1)
	{
		PT_WAIT_UNTIL(pt, function_returns_true() );
		do_something();
	}
	PT_END(pt);
}

/*
static int protothread1(struct pt *pt) {
    PT_BEGIN(pt);
    while(1) {
        PT_WAIT_UNTIL(pt, function_returns_true() );
        do_something();
    }
    PT_END(pt);}
*/


/*

void setup()
{
  for (int i = 0; i < num_pwm; ++i)
  {
    pinMode(pwm_pins[i], OUTPUT);
    digitalWrite(pwm_pins[i], HIGH);
  }
  // But not pin 10 (emergency LEDs)
  digitalWrite(10, LOW);
  
  for (int i = 0; i < num_leds; ++i)
  {
    pinMode(led_pins[i], OUTPUT);
    //digitalWrite(led_pins[i], HIGH);
  }
  // Pins 12 and 13 are always on
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
}

void increment_led()
{
  // Disable the led before selecting a new one
  digitalWrite(current_led, LOW);
  
  // get the new led pin number
  for (int i = 0; i < num_leds; ++i)
  {
    if (current_led == led_pins[i])
    {
      if (i + 1 < num_leds)
        current_led = led_pins[i + 1];
      else
        current_led = led_pins[0];
      break;
    }
  }
  digitalWrite(current_led, HIGH);
}

void loop()
{
  /*
  for (int brightness = 0; brightness < 255; ++brightness)
  {
    for (int i = 0; i < num_pwm; ++i)
    {
      analogWrite(pwm_pins[i], brightness);
    }
    delay(2);
  }
  increment_led();
  for (int brightness = 255; brightness > 0; --brightness)
  {
    for (int i = 0; i < num_pwm; ++i)
    {
      analogWrite(pwm_pins[i], brightness);
    }
    delay(2);
  }
  /**
  increment_led();
  delay(200);
}
*/
