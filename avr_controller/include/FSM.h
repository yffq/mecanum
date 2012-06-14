#ifndef FSM_H
#define FSM_H

#include "ParamServer.h"
#include "TinyBuffer.h"

namespace AVR
{

class AnalogPublisher;
class BatteryMonitor;
class Blink;
class ChristmasTree;
class DigitalPublisher;
class Fade;
class Mimic;
class Toggle;

class FSM
{
protected:
	/**
	 * On the Arduino, the baIDse FSM class is partially virtual and can't be
	 * instantiated directly, but over here we need to protect our concrete
	 * class.
	 */
	FSM(unsigned char id, unsigned char *params, unsigned char len) : parameters(params, len)
	{
		parameters[0] = id;
	}

	/**
	 * Only call this from same-type FSMs, because the buffer lengths must
	 * match and the ID bit is not copied.
	 */
	void CopyFrom(const TinyBuffer &params);
	void CopyFrom(const FSM &other) { CopyFrom(other.parameters); }

public:
	/**
	 * ID of the FSM; used to determine the identify of the derived class.
	 */
	unsigned char GetID() const { return parameters[0]; }

	/**
	 * A FSM is uniquely identified by its parameters. A FSM should not alter
	 * parameters after instantiation, because it would in effect transform
	 * itself into a dissimilar FSM.
	 */
	bool operator==(const FSM &other) const { return parameters == other.parameters; }

	unsigned char operator[](unsigned int i) const { return parameters[i]; }

	const TinyBuffer &GetParams() const { return parameters; }

	/**
	 * The maximum length of a message is 255 chars.
	 */
	unsigned char  GetSize() const { return parameters.Length(); }

	/**
	 * Construct an empty (invalid) FSM. Before being uploaded to the AVR,
	 * the necessary params must be set through the subclass's Set* methods
	 * and IsValid() must return true.
	 */
	static FSM* NewFromID(unsigned char id);

	static FSM* NewFromBuffer(const TinyBuffer &buf);

	bool IsValid() const;

private:
	TinyBuffer parameters;
};


class AnalogPublisher: public FSM, public ParamServer::AnalogPublisher
{
public:
	AnalogPublisher()                             : FSM(FSM_ANALOGPUBLISHER, m_params, sizeof(m_params)) { }
	AnalogPublisher(const AnalogPublisher &other) : FSM(FSM_ANALOGPUBLISHER, m_params, sizeof(m_params)) { CopyFrom(other); }
	AnalogPublisher(const TinyBuffer &params)     : FSM(FSM_ANALOGPUBLISHER, m_params, sizeof(m_params)) { CopyFrom(params); }
	AnalogPublisher &operator=(const AnalogPublisher &other) { CopyFrom(other); return *this; }
};

class BatteryMonitor: public FSM, public ParamServer::BatteryMonitor
{
public:
	BatteryMonitor()                            : FSM(FSM_BATTERYMONITOR, m_params, sizeof(m_params)) { }
	BatteryMonitor(const BatteryMonitor &other) : FSM(FSM_BATTERYMONITOR, m_params, sizeof(m_params)) { CopyFrom(other); }
	BatteryMonitor(const TinyBuffer &params)    : FSM(FSM_BATTERYMONITOR, m_params, sizeof(m_params)) { CopyFrom(params); }
	BatteryMonitor &operator=(const BatteryMonitor &other) { CopyFrom(other); return *this; }
};

class Blink : public FSM, public ParamServer::Blink
{
public:
	Blink()                         : FSM(FSM_BLINK, m_params, sizeof(m_params)) { }
	Blink(const Blink &other)       : FSM(FSM_BLINK, m_params, sizeof(m_params)) { CopyFrom(other); }
	Blink(const TinyBuffer &params) : FSM(FSM_BLINK, m_params, sizeof(m_params)) { CopyFrom(params); }
	Blink &operator=(const Blink &other) { CopyFrom(other); return *this; }
};

class ChristmasTree: public FSM, public ParamServer::ChristmasTree
{
public:
	ChristmasTree()                           : FSM(FSM_CHRISTMASTREE, m_params, sizeof(m_params)) { }
	ChristmasTree(const ChristmasTree &other) : FSM(FSM_CHRISTMASTREE, m_params, sizeof(m_params)) { CopyFrom(other); }
	ChristmasTree(const TinyBuffer &params)   : FSM(FSM_CHRISTMASTREE, m_params, sizeof(m_params)) { CopyFrom(params); }
	ChristmasTree &operator=(const ChristmasTree &other) { CopyFrom(other); return *this; }
};

class DigitalPublisher : public FSM, public ParamServer::DigitalPublisher
{
public:
	DigitalPublisher()                              : FSM(FSM_DIGITALPUBLISHER, m_params, sizeof(m_params)) { }
	DigitalPublisher(const DigitalPublisher &other) : FSM(FSM_DIGITALPUBLISHER, m_params, sizeof(m_params)) { CopyFrom(other); }
	DigitalPublisher(const TinyBuffer &params)      : FSM(FSM_DIGITALPUBLISHER, m_params, sizeof(m_params)) { CopyFrom(params); }
	DigitalPublisher &operator=(const DigitalPublisher &other) { CopyFrom(other); return *this; }
};

class Fade : public FSM, public ParamServer::Fade
{
public:
	Fade()                         : FSM(FSM_FADE, m_params, sizeof(m_params)) { }
	Fade(const Fade &other)        : FSM(FSM_FADE, m_params, sizeof(m_params)) { CopyFrom(other); }
	Fade(const TinyBuffer &params) : FSM(FSM_FADE, m_params, sizeof(m_params)) { CopyFrom(params); }
	Fade &operator=(const Fade &other) { CopyFrom(other); return *this; }
};

class Mimic : public FSM, public ParamServer::Mimic
{
public:
	Mimic()                         : FSM(FSM_MIMIC, m_params, sizeof(m_params)) { }
	Mimic(const Mimic &other)       : FSM(FSM_MIMIC, m_params, sizeof(m_params)) { CopyFrom(other); }
	Mimic(const TinyBuffer &params) : FSM(FSM_MIMIC, m_params, sizeof(m_params)) { CopyFrom(params); }
	Mimic &operator=(const Mimic &other) { CopyFrom(other); return *this; }
};

class Toggle : public FSM, public ParamServer::Toggle
{
public:
	Toggle()                         : FSM(FSM_TOGGLE, m_params, sizeof(m_params)) { }
	Toggle(const Toggle &other)      : FSM(FSM_TOGGLE, m_params, sizeof(m_params)) { CopyFrom(other); }
	Toggle(const TinyBuffer &params) : FSM(FSM_TOGGLE, m_params, sizeof(m_params)) { CopyFrom(params); }
	Toggle &operator=(const Toggle &other) { CopyFrom(other); return *this; }
};

} // namespace AVR

#endif // FSM_H
