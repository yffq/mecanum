#ifndef PARAMSERVER_H
#define PARAMSERVER_H

#include "AddressBook.h"
#include <string.h> // for memcpy()

namespace ArduinoVerifier
{
	inline bool IsAnalog(uint8_t pin) { return pin <= 15; }
	inline bool IsDigital(uint8_t pin) { return 1 <= pin && pin <= 53; }
	inline bool IsPWM(uint8_t pin) { return pin <= 13 || (44 <= pin && pin <= 46); }
	inline bool IsBinary(uint8_t value) { return value == 0 || value == 1; }
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

// OK for a packed struct to begin with a single byte, because GCC will align
// the struct so that the rest of the members are aligned

class AnalogPublisher
{
public:
	AnalogPublisher() { m_params.id = FSM_ANALOGPUBLISHER; }
	AnalogPublisher(const uint8_t *bytes) { memcpy(&m_params, bytes, sizeof(Parameters)); }

	uint8_t GetPin() const { return m_params.pin; }
	void SetPin(uint8_t pin) { m_params.pin = pin; }

	uint32_t GetDelay() const { return m_params.delay; }
	void SetDelay(uint32_t delay) { m_params.delay = delay; }

	static bool Validate(const uint8_t *bytes, uint16_t length)
	{
		if (length == sizeof(Parameters))
		{
			Parameters params;
			memcpy(&params, bytes, sizeof(Parameters));
			bool valid = (params.id == FSM_ANALOGPUBLISHER);
			valid &= (ArduinoVerifier::IsAnalog(params.pin));
			return valid;
		}
		return false;
	}

	struct Parameters
	{
		uint8_t id;
		uint8_t pin;
		uint32_t delay;
	} __attribute__((packed));

protected:
	Parameters m_params;
};

class AnalogPublisherPublisherMsg
{
public:
	AnalogPublisherPublisherMsg(uint8_t pin, uint16_t value)
	{
		m_msg.length = sizeof(Message);
		m_msg.id = FSM_ANALOGPUBLISHER;
		m_msg.pin = pin;
		m_msg.value = value;
	}
	AnalogPublisherPublisherMsg(const uint8_t *bytes) { memcpy(&m_msg, bytes, sizeof(Message)); }

	static uint16_t GetLength() { return sizeof(Message); }
	uint8_t GetId() const { return m_msg.id; }
	uint8_t GetPin() const { return m_msg.pin; }
	uint16_t GetValue() const { return m_msg.value; }
	const uint8_t *GetBuffer() const { return reinterpret_cast<const uint8_t*>(&m_msg); }

private:
	struct Message
	{
		uint16_t length;
		uint8_t id;
		uint8_t pin;
		uint16_t value;
	} __attribute__((packed));

	Message m_msg;
};

class AnalogPublisherSubscriberMsg
{
public:
	AnalogPublisherSubscriberMsg(uint8_t pin)
	{
		m_msg.length = sizeof(Message);
		m_msg.id = FSM_ANALOGPUBLISHER;
		m_msg.pin = pin;
	}
	AnalogPublisherSubscriberMsg(const uint8_t *bytes) { memcpy(&m_msg, bytes, sizeof(Message)); }

	static uint16_t GetLength() { return sizeof(Message); }
	uint8_t GetId() const { return m_msg.id; }
	uint8_t GetPin() const { return m_msg.pin; }
	const uint8_t *GetBuffer() const { return reinterpret_cast<const uint8_t*>(&m_msg); }

private:
	struct Message
	{
		uint16_t length;
		uint8_t id;
		uint8_t pin;
	} __attribute__((packed));

	Message m_msg;
};




class BatteryMonitor
{
public:
	BatteryMonitor() { m_params.id = FSM_BATTERYMONITOR; }
	BatteryMonitor(const uint8_t *bytes) { memcpy(&m_params, bytes, sizeof(Parameters)); }

	static bool Validate(const uint8_t *bytes, uint16_t length)
	{
		if (length == sizeof(Parameters))
		{
			Parameters params;
			memcpy(&params, bytes, sizeof(Parameters));
			return (params.id == FSM_BATTERYMONITOR);
		}
		return false;
	}

	struct Parameters
	{
		uint8_t id;
	} __attribute__((packed));

protected:
	Parameters m_params;
};





class Blink
{
public:
	Blink() { m_params.id = FSM_BLINK; }
	Blink(const uint8_t *bytes) { memcpy(&m_params, bytes, sizeof(Parameters)); }

	uint8_t GetPin() const { return m_params.pin; }
	void SetPin(uint8_t pin) { m_params.pin = pin; }

	uint32_t GetDelay() const { return m_params.delay; }
	void SetDelay(uint32_t delay) { m_params.delay = delay; }

	static bool Validate(const uint8_t *bytes, uint16_t length)
	{
		if (length == sizeof(Parameters))
		{
			Parameters params;
			memcpy(&params, bytes, sizeof(Parameters));
			return (params.id == FSM_BLINK) && (ArduinoVerifier::IsDigital(params.pin));
		}
		return false;
	}

	struct Parameters
	{
		uint8_t id;
		uint8_t pin;
		uint32_t delay;
	} __attribute__((packed));

protected:
	Parameters m_params;
};




class ChristmasTree
{
public:
	ChristmasTree() { m_params.id = FSM_CHRISTMASTREE; }
	ChristmasTree(const uint8_t *bytes) { memcpy(&m_params, bytes, sizeof(Parameters)); }

	static bool Validate(const uint8_t *bytes, uint16_t length)
	{
		if (length == sizeof(Parameters))
		{
			Parameters params;
			memcpy(&params, bytes, sizeof(Parameters));
			return (params.id == FSM_CHRISTMASTREE);
		}
		return false;
	}

	struct Parameters
	{
		uint8_t id;
	} __attribute__((packed));

protected:
	Parameters m_params;
};




class DigitalPublisher
{
public:
	DigitalPublisher() { m_params.id = FSM_DIGITALPUBLISHER; }
	DigitalPublisher(const uint8_t *bytes) { memcpy(&m_params, bytes, sizeof(Parameters)); }

	uint8_t GetPin() const { return m_params.pin; }
	void SetPin(uint8_t pin) { m_params.pin = pin; }

	uint32_t GetDelay() const { return m_params.delay; }
	void SetDelay(uint32_t delay) { m_params.delay = delay; }

	static bool Validate(const uint8_t *bytes, uint16_t length)
	{
		if (length == sizeof(Parameters))
		{
			Parameters params;
			memcpy(&params, bytes, sizeof(Parameters));
			return (params.id == FSM_DIGITALPUBLISHER) && (ArduinoVerifier::IsDigital(params.pin));
		}
		return false;
	}

	struct Parameters
	{
		uint8_t id;
		uint8_t pin;
		uint32_t delay;
	} __attribute__((packed));

protected:
	Parameters m_params;
};

class DigitalPublisherPublisherMsg
{
public:
	DigitalPublisherPublisherMsg(uint8_t pin, uint16_t value)
	{
		m_msg.length = sizeof(Message);
		m_msg.id = FSM_DIGITALPUBLISHER;
		m_msg.pin = pin;
		m_msg.value = value;
	}
	DigitalPublisherPublisherMsg(const uint8_t *bytes) { memcpy(&m_msg, bytes, sizeof(Message)); }

	static uint16_t GetLength() { return sizeof(Message); }
	uint8_t GetId() const { return m_msg.id; }
	uint8_t GetPin() const { return m_msg.pin; }
	uint16_t GetValue() const { return m_msg.value; }
	const uint8_t *GetBuffer() const { return reinterpret_cast<const uint8_t*>(&m_msg); }

private:
	struct Message
	{
		uint16_t length;
		uint8_t id;
		uint8_t pin;
		uint16_t value;
	} __attribute__((packed));

	Message m_msg;
};

class DigitalPublisherSubscriberMsg
{
public:
	DigitalPublisherSubscriberMsg(uint8_t pin)
	{
		m_msg.length = sizeof(Message);
		m_msg.id = FSM_DIGITALPUBLISHER;
		m_msg.pin = pin;
	}
	DigitalPublisherSubscriberMsg(const uint8_t *bytes) { memcpy(&m_msg, bytes, sizeof(Message)); }

	static uint16_t GetLength() { return sizeof(Message); }
	uint8_t GetId() const { return m_msg.id; }
	uint8_t GetPin() const { return m_msg.pin; }
	const uint8_t *GetBuffer() const { return reinterpret_cast<const uint8_t*>(&m_msg); }

private:
	struct Message
	{
		uint16_t length;
		uint8_t id;
		uint8_t pin;
	} __attribute__((packed));

	Message m_msg;
};




class Fade
{
public:
	Fade() { m_params.id = FSM_FADE; }
	Fade(const uint8_t *bytes) { memcpy(&m_params, bytes, sizeof(Parameters)); }

	uint8_t GetPin() const { return m_params.pin; }
	void SetPin(uint8_t pin) { m_params.pin = pin; }

	uint8_t GetCurve() const { return m_params.curve; }
	void SetCurve(uint8_t curve) { m_params.curve = curve; }

	uint32_t GetPeriod() const { return m_params.period; }
	void SetPeriod(uint32_t period) { m_params.period = period; }

	uint32_t GetDelay() const { return m_params.delay; }
	void SetDelay(uint32_t delay) { m_params.delay = delay; }

	static bool Validate(const uint8_t *bytes, uint16_t length)
	{
		if (length == sizeof(Parameters))
		{
			Parameters params;
			memcpy(&params, bytes, sizeof(Parameters));
			return (params.id == FSM_FADE) && (ArduinoVerifier::IsPWM(params.pin)) && (ArduinoVerifier::IsBinary(params.curve));
		}
		return false;
	}

	struct Parameters
	{
		uint8_t id;
		uint8_t pin;
		uint8_t curve;
		uint32_t period;
		uint32_t delay;
	} __attribute__((packed));

protected:
	Parameters m_params;
};





class Mimic
{
public:
	Mimic() { m_params.id = FSM_MIMIC; }
	Mimic(const uint8_t *bytes) { memcpy(&m_params, bytes, sizeof(Parameters)); }

	uint8_t GetSource() const { return m_params.source; }
	void SetSource(uint8_t source) { m_params.source = source; }

	uint8_t GetDest() const { return m_params.dest; }
	void SetDest(uint8_t dest) { m_params.dest = dest; }

	uint32_t GetDelay() const { return m_params.delay; }
	void SetDelay(uint32_t delay) { m_params.delay = delay; }

	static bool Validate(const uint8_t *bytes, uint16_t length)
	{
		if (length == sizeof(Parameters))
		{
			Parameters params;
			memcpy(&params, bytes, sizeof(Parameters));
			return (params.id == FSM_MIMIC) && (ArduinoVerifier::IsDigital(params.source)) && (ArduinoVerifier::IsDigital(params.dest));
		}
		return false;
	}

	struct Parameters
	{
		uint8_t id;
		uint8_t source;
		uint8_t dest;
		uint32_t delay;
	} __attribute__((packed));

protected:
	Parameters m_params;
};




class Toggle
{
public:
	Toggle() { m_params.id = FSM_TOGGLE; }
	Toggle(const uint8_t *bytes) { memcpy(&m_params, bytes, sizeof(Parameters)); }

	uint8_t GetPin() const { return m_params.pin; }
	void SetPin(uint8_t pin) { m_params.pin = pin; }

	static bool Validate(const uint8_t *bytes, uint16_t length)
	{
		if (length == sizeof(Parameters))
		{
			Parameters params;
			memcpy(&params, bytes, sizeof(Parameters));
			return (params.id == FSM_TOGGLE) && (ArduinoVerifier::IsDigital(params.pin));
		}
		return false;
	}

	struct Parameters
	{
		uint8_t id;
		uint8_t pin;
	} __attribute__((packed));

protected:
	Parameters m_params;
};


class ToggleSubscriberMsg
{
public:
	ToggleSubscriberMsg(uint8_t pin, uint8_t command)
	{
		m_msg.length = sizeof(Message);
		m_msg.id = FSM_TOGGLE;
		m_msg.pin = pin;
		m_msg.command = command;
	}
	ToggleSubscriberMsg(const uint8_t *bytes) { memcpy(&m_msg, bytes, sizeof(Message)); }

	static uint16_t GetLength() { return sizeof(Message); }
	uint8_t GetId() const { return m_msg.id; }
	uint8_t GetPin() const { return m_msg.pin; }
	uint8_t GetCommand() const { return m_msg.command; }
	const uint8_t *GetBuffer() const { return reinterpret_cast<const uint8_t*>(&m_msg); }

private:
	struct Message
	{
		uint16_t length;
		uint8_t id;
		uint8_t pin;
		uint8_t command;
	} __attribute__((packed));

	Message m_msg;
};


} // namespace ParamServer



#endif // PARAMSERVER_H
