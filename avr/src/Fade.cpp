#include "Fade.h"
#include "AddressBook.h"

#include <Arduino.h>
#include <avr/pgmspace.h>

// Brightness lookup table stored in PROGMEM instead of SRAM
// Table is from http://arduino.cc/forum/index.php?topic=96839.0
const unsigned char luminace[256] PROGMEM =
{
	  0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   4,   4,
	  4,   4,   4,   5,   5,   5,   5,   6,   6,   6,   6,   7,   7,   7,   7,   8,
	  8,   8,   9,   9,   9,  10,  10,  10,  11,  11,  12,  12,  12,  13,  13,  14,
	 14,  15,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21,  22,
	 22,  23,  23,  24,  25,  25,  26,  26,  27,  28,  28,  29,  30,  30,  31,  32,
	 33,  33,  34,  35,  36,  36,  37,  38,  39,  40,  40,  41,  42,  43,  44,  45,
	 46,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,
	 61,  62,  63,  64,  65,  67,  68,  69,  70,  71,  72,  73,  75,  76,  77,  78,
	 80,  81,  82,  83,  85,  86,  87,  89,  90,  91,  93,  94,  95,  97,  98,  99,
	101, 102, 104, 105, 107, 108, 110, 111, 113, 114, 116, 117, 119, 121, 122, 124,
	125, 127, 129, 130, 132, 134, 135, 137, 139, 141, 142, 144, 146, 148, 150, 151,
	153, 155, 157, 159, 161, 163, 165, 166, 168, 170, 172, 174, 176, 178, 180, 182,
	184, 186, 189, 191, 193, 195, 197, 199, 201, 204, 206, 208, 210, 212, 215, 217,
	219, 221, 224, 226, 228, 231, 233, 235, 238, 240, 243, 245, 248, 250, 253, 255
};

#define PARAM_ID     0
#define PARAM_PIN    1
#define PARAM_CURVE  2
#define PARAM_PERIOD 3
#define PARAM_DELAY  7

Fade::Fade(uint8_t pin, unsigned long period, unsigned long delay, LuminanceCurve curve /* = LINEAR */) :
	m_dir(UP), m_brightness(0), m_delay(delay), m_enabled(true)
{
	m_params[PARAM_ID] = FSM_FADE;
	m_params[PARAM_PIN] = pin;
	m_params[PARAM_CURVE] = curve;
	ByteArray::Serialize(period, m_params + PARAM_PERIOD);
	ByteArray::Serialize(m_delay, m_params + PARAM_DELAY);
	DeclareParameters(m_params, sizeof(m_params));

	// Use half the period to calculate brightness increments
	m_brightnessStep = 255 * delay * 2 / period;
	pinMode(m_params[PARAM_PIN], OUTPUT);
	analogWrite(m_params[PARAM_PIN], 0);
}

Fade *Fade::NewFromArray(const ByteArray &params)
{
	if (params.Length() >= sizeof(m_params) && params[PARAM_ID] == FSM_FADE)
	{
		// Only 0-13 and 44-46 are valid PWM pins
		if ((0 <= params[PARAM_PIN] && params[PARAM_PIN] <= 13) ||
		    (44 <= params[PARAM_PIN] && params[PARAM_PIN] <= 46))
		{
			unsigned long period, delay;
			ByteArray::Deserialize(&params[PARAM_PERIOD], period);
			ByteArray::Deserialize(&params[PARAM_DELAY], delay);
			return new Fade(params[PARAM_PIN], period, delay, static_cast<LuminanceCurve>(params[PARAM_CURVE]));
		}
	}
	return 0;
}

Fade::~Fade()
{
	analogWrite(m_params[PARAM_PIN], 0);
}

// So we have the option to avoid hitting the VTable
void Fade::StepAwayFromTheVTable()
{
	if (m_enabled)
	{
		if (m_dir == UP)
		{
			m_brightness += m_brightnessStep;
			if (m_brightness >= 255)
			{
				m_brightness = 255;
				m_dir = DOWN;
			}
		}
		else
		{
			m_brightness -= m_brightnessStep;
			if (m_brightness <= 0)
			{
				m_brightness = 0;
				m_dir = UP;
			}
		}

		if (m_params[PARAM_CURVE] == LINEAR)
			analogWrite(m_params[PARAM_PIN], pgm_read_byte_near(luminace + m_brightness));
		else
			analogWrite(m_params[PARAM_PIN], m_brightness);
	}
}
