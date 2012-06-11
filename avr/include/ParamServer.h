#ifndef PARAMSERVER_H
#define PARAMSERVER_H


#include "AddressBook.h"

#ifdef __AVR__
  #include "ByteArray.h"
#endif

#ifndef NULL
  #define NULL (void*)0
#endif

/**
 * Utilities to serialize and unserialize some various variable types. The
 * functions defined in this namespace should be inlined to let the linker know
 * to not look for them in a C++ file. This avoids the error of having multiple
 * definitions show up when linking the executable, at the expense of a
 * slightly larger executable.
 */
namespace ByteUtils
{
	/**
	 * Store a long in a byte array in big-endian format.
	 *
	 * Pre-condition: buffer's size is at least 4. To achieve compatibility
	 * with 8-bit processors, only the lowest 4 bytes of n are serialized.
	 */
	inline void Serialize(unsigned long n, unsigned char *buffer)
	{
#ifndef __AVR__
		n &= 0xFFFFFFFF;
#endif
		buffer[0] = n >> 24; // big endian
		buffer[1] = n >> 16 & 0xFF;
		buffer[2] = n >> 8 & 0xFF;
		buffer[3] = n & 0xFF;
	}

	/**
	 * Recover a long (in big-endian format) from a byte array.
	 */
	inline unsigned long Deserialize(const unsigned char *buffer)
	{
		return static_cast<unsigned long>(buffer[0]) << 24 |
			   static_cast<unsigned long>(buffer[1]) << 16 |
			   static_cast<unsigned long>(buffer[2]) << 8 | buffer[3];
	}
}

namespace ArduinoVerifier
{
	inline bool IsAnalog(unsigned char pin) { return pin <= 15; }
	inline bool IsPWM(unsigned char pin) { return pin <= 13 || (44 <= pin && pin <= 46); }
}

/**
 * A centralized location to manage the parameters of FiniteStateMachines.
 * Originally, this was performed inside each individual class. While appeasing
 * encapsulation desires, the reality was that much of the code was copy/pasted
 * across files, leaving room for the copy-paste monster to satisfy his
 * appetite. A lot of copy/pasting is still being done, but at least now it's
 * all confined to a single file, making errors easy to catch and system-wide
 * changes easier to perform.
 *
 * Looking forward to the future, to fully accomplish scalability, the code
 * below should be generated using C-preprocessor macros (oh good lord) or
 * an alternative code-generation system, perhaps using Python.
 *
 * For now, copy/paste it is!
 */
namespace ParamServer
{


#ifdef __AVR__
  #define PARAM_T const ByteArray &
#else
  #define PARAM_T boost::asio::const_buffer
#endif


#ifdef __AVR__
  #define LENGTH(buf) (buf).Length()
#else
  #define LENGTH(buf) boost::asio::buffer_size(buf)
#endif





#define PARAM_ANALOGPUBLISHER_ID    0 // 1 byte
#define PARAM_ANALOGPUBLISHER_PIN   1 // 1 byte
#define PARAM_ANALOGPUBLISHER_DELAY 2 // 4 bytes

class AnalogPublisher
{
public:
	static unsigned char GetPin(PARAM_T params) { return params[PARAM_ANALOGPUBLISHER_PIN]; }
	unsigned char GetPin() const { return m_params[PARAM_ANALOGPUBLISHER_PIN]; }
	void SetPin(unsigned char pin) { m_params[PARAM_ANALOGPUBLISHER_PIN] = pin; }

	static unsigned long GetDelay(PARAM_T params) { return ByteUtils::Deserialize(&params[PARAM_ANALOGPUBLISHER_DELAY]); }
	unsigned long GetDelay() const { return ByteUtils::Deserialize(&m_params[PARAM_ANALOGPUBLISHER_DELAY]); }
	void SetDelay(unsigned long delay) { ByteUtils::Serialize(delay, &m_params[PARAM_ANALOGPUBLISHER_DELAY]); }

	static bool Validate(PARAM_T params)
	{
		return LENGTH(params) >= sizeof(m_params) && params[PARAM_ANALOGPUBLISHER_ID] == FSM_ANALOGPUBLISHER && ArduinoVerifier::IsAnalog(params[PARAM_ANALOGPUBLISHER_PIN]);
	}

protected:
	unsigned char m_params[6];
};





#define PARAM_BATTERYMONITOR_ID  0  // 1 byte

class BatteryMonitor
{
public:
	static bool Validate(PARAM_T params)
	{
		return LENGTH(params) >= sizeof(m_params) && params[PARAM_BATTERYMONITOR_ID] == FSM_BATTERYMONITOR;
	}

protected:
	unsigned char m_params[1];
};



#define PARAM_BLINK_ID    0 // 1 byte
#define PARAM_BLINK_PIN   1 // 1 byte
#define PARAM_BLINK_DELAY 2 // 4 bytes

class Blink
{
public:
	static unsigned char GetPin(PARAM_T params) { return params[PARAM_BLINK_PIN]; }
	unsigned char GetPin() const { return m_params[PARAM_BLINK_PIN]; }
	void SetPin(unsigned char pin) { m_params[PARAM_BLINK_PIN] = pin; }

	// Note: These will not modify the delay of a running FSM, they only affect its "footprint"
	static unsigned long GetDelay(PARAM_T params) { return ByteUtils::Deserialize(&params[PARAM_BLINK_DELAY]); }
	unsigned long GetDelay() const { return ByteUtils::Deserialize(&m_params[PARAM_BLINK_DELAY]); }
	void SetDelay(unsigned long delay) { ByteUtils::Serialize(delay, &m_params[PARAM_BLINK_DELAY]); }

	static bool Validate(PARAM_T params)
	{
		return LENGTH(params) >= sizeof(m_params) && params[PARAM_BLINK_ID] == FSM_BLINK;
	}

protected:
	unsigned char m_params[6];
};






#define PARAM_CHRISTMASTREE_ID  0  // 1 byte

class ChristmasTree
{
public:
	static bool Validate(PARAM_T params)
	{
		return LENGTH(params) >= sizeof(m_params) && params[PARAM_CHRISTMASTREE_ID] == FSM_CHRISTMASTREE;
	}

protected:
	unsigned char m_params[1];
};






#define PARAM_DIGITALPUBLISHER_ID    0 // 1 byte
#define PARAM_DIGITALPUBLISHER_PIN   1 // 1 byte
#define PARAM_DIGITALPUBLISHER_DELAY 2 // 4 bytes

class DigitalPublisher
{
public:
	static bool Validate(PARAM_T params)
	{
		return LENGTH(params) >= sizeof(m_params) && params[PARAM_DIGITALPUBLISHER_ID] == FSM_DIGITALPUBLISHER;
	}

	static unsigned char GetPin(PARAM_T params) { return params[PARAM_DIGITALPUBLISHER_PIN]; }
	unsigned char GetPin() const { return m_params[PARAM_DIGITALPUBLISHER_PIN]; }
	void SetPin(unsigned char pin) { m_params[PARAM_DIGITALPUBLISHER_PIN] = pin; }

	static unsigned long GetDelay(PARAM_T params) { return ByteUtils::Deserialize(&params[PARAM_DIGITALPUBLISHER_DELAY]); }
	unsigned long GetDelay() const { return ByteUtils::Deserialize(&m_params[PARAM_DIGITALPUBLISHER_DELAY]); }
	void SetDelay(unsigned long delay) { ByteUtils::Serialize(delay, &m_params[PARAM_DIGITALPUBLISHER_DELAY]); }

protected:
	unsigned char m_params[6];
};


/**
 * Fade a light on a PWM pin.
 *
 * Parameters:
 * ---
 * uint8  ID
 * uint8  Pin (IsPWM)
 * uint8  Curve  # Linear (0) or Logarithmic (1)
 * utin32 Period
 * uint32 Delay
 * ---
 */
#define PARAM_FADE_ID     0 // 1 byte
#define PARAM_FADE_PIN    1 // 1 byte
#define PARAM_FADE_CURVE  2 // 1 byte
#define PARAM_FADE_PERIOD 3 // 4 bytes
#define PARAM_FADE_DELAY  7 // 4 bytes

class Fade
{
public:
	static bool Validate(PARAM_T params)
	{
		return LENGTH(params) >= sizeof(m_params) && params[PARAM_FADE_ID] == FSM_FADE && ArduinoVerifier::IsPWM(params[PARAM_FADE_PIN]);
	}

	static unsigned char GetPin(PARAM_T params) { return params[PARAM_FADE_PIN]; }
	unsigned char GetPin() const { return m_params[PARAM_FADE_PIN]; }
	void SetPin(unsigned char pin) { m_params[PARAM_FADE_PIN] = pin; }

	static unsigned char GetCurve(PARAM_T params) { return params[PARAM_FADE_CURVE]; }
	unsigned char GetCurve() const { return m_params[PARAM_FADE_CURVE]; }
	void SetCurve(unsigned char curve) { m_params[PARAM_FADE_CURVE] = curve; }

	// Note: These will not modify the period of a running FSM, they only affect its "footprint"
	static unsigned long GetPeriod(PARAM_T params) { return ByteUtils::Deserialize(&params[PARAM_FADE_PERIOD]); }
	unsigned long GetPeriod() const { return ByteUtils::Deserialize(&m_params[PARAM_FADE_PERIOD]); }
	void SetPeriod(unsigned long period) { ByteUtils::Serialize(period, &m_params[PARAM_FADE_PERIOD]); }

	// Note: These will not modify the delay of a running FSM, they only affect its "footprint"
	static unsigned long GetDelay(PARAM_T params) { return ByteUtils::Deserialize(&params[PARAM_FADE_DELAY]); }
	unsigned long GetDelay() const { return ByteUtils::Deserialize(&m_params[PARAM_FADE_DELAY]); }
	void SetDelay(unsigned long delay) { ByteUtils::Serialize(delay, &m_params[PARAM_FADE_DELAY]); }

protected:
	unsigned char m_params[11];
};





#define PARAM_MIMIC_ID     0 // 1 byte
#define PARAM_MIMIC_SOURCE 1 // 1 byte
#define PARAM_MIMIC_DEST   1 // 1 byte
#define PARAM_MIMIC_DELAY  2 // 4 bytes

class Mimic
{
public:
	static bool Validate(PARAM_T params)
	{
		return LENGTH(params) >= sizeof(m_params) && params[PARAM_MIMIC_ID] == FSM_MIMIC;
	}

	static unsigned char GetSource(PARAM_T params) { return params[PARAM_MIMIC_SOURCE]; }
	unsigned char GetSource() const { return m_params[PARAM_MIMIC_SOURCE]; }
	void SetSource(unsigned char source) { m_params[PARAM_MIMIC_SOURCE] = source; }

	static unsigned char GetDest(PARAM_T params) { return params[PARAM_MIMIC_DEST]; }
	unsigned char GetDest() const { return m_params[PARAM_MIMIC_DEST]; }
	void SetDest(unsigned char dest) { m_params[PARAM_MIMIC_DEST] = dest; }

	// Note: These will not modify the delay of a running FSM, they only affect its "footprint"
	static unsigned long GetDelay(PARAM_T params) { return ByteUtils::Deserialize(&params[PARAM_MIMIC_DELAY]); }
	unsigned long GetDelay() const { return ByteUtils::Deserialize(&m_params[PARAM_MIMIC_DELAY]); }
	void SetDelay(unsigned long delay) { ByteUtils::Serialize(delay, &m_params[PARAM_MIMIC_DELAY]); }

protected:
	unsigned char m_params[7];
};






#define PARAM_TOGGLE_ID    0 // 1 byte
#define PARAM_TOGGLE_PIN   1 // 1 byte
#define PARAM_TOGGLE_DELAY 2 // 4 bytes

class Toggle
{
public:
	static bool Validate(PARAM_T params)
	{
		return LENGTH(params) >= sizeof(m_params) && params[PARAM_TOGGLE_ID] == FSM_TOGGLE;
	}

	static unsigned char GetPin(PARAM_T params) { return params[PARAM_TOGGLE_PIN]; }
	unsigned char GetPin() const { return m_params[PARAM_TOGGLE_PIN]; }
	void SetPin(unsigned char pin) { m_params[PARAM_TOGGLE_PIN] = pin; }

	static unsigned long GetDelay(PARAM_T params) { return ByteUtils::Deserialize(&params[PARAM_TOGGLE_DELAY]); }
	unsigned long GetDelay() const { return ByteUtils::Deserialize(&m_params[PARAM_TOGGLE_DELAY]); }
	void SetDelay(unsigned long delay) { ByteUtils::Serialize(delay, &m_params[PARAM_TOGGLE_DELAY]); }

protected:
	unsigned char m_params[6];
};



} // namespace ParamServer



#endif // PARAMSERVER_H
