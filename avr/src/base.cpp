#include <Arduino.h>

const int pwm_pins[] = {4, // Green
                        7, // Amber
                        8, // Red
                        9, // UV
                        10}; // Emergency
const int num_pwm = sizeof(pwm_pins) / sizeof(pwm_pins[0]);
const int led_pins[] = {46, 44, 48, 42, 50, 52};
const int num_leds = sizeof(led_pins) / sizeof(led_pins[0]);

int current_led = led_pins[0];

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
  */
  increment_led();
  delay(200);
}

