#include "FSM.h"


using namespace AVR;

void FSM::CopyFrom(const TinyBuffer &params)
{
	for (unsigned int i = 0; i < parameters.Length(); ++i)
		parameters[i] = params[i];
}

FSM* FSM::NewFromID(unsigned char id)
{
	switch (id)
	{
	case FSM_ANALOGPUBLISHER:
		return new AnalogPublisher();
	case FSM_BATTERYMONITOR:
		return new BatteryMonitor();
	case FSM_BLINK:
		return new Blink();
	case FSM_CHRISTMASTREE:
		return new ChristmasTree();
	case FSM_DIGITALPUBLISHER:
		return new DigitalPublisher();
	case FSM_FADE:
		return new Fade();
	case FSM_MIMIC:
		return new Mimic();
	case FSM_TOGGLE:
		return new Toggle();
	}
	return (FSM*)NULL;
}

FSM* FSM::NewFromBuffer(const TinyBuffer &buf)
{
	switch (buf[0])
	{
	case FSM_ANALOGPUBLISHER:
		if (AnalogPublisher::Validate(buf))
			return new AnalogPublisher(buf);
		break;
	case FSM_BATTERYMONITOR:
		if (BatteryMonitor::Validate(buf))
			return new BatteryMonitor(buf);
		break;
	case FSM_BLINK:
		if (Blink::Validate(buf))
			return new Blink(buf);
		break;
	case FSM_CHRISTMASTREE:
		if (ChristmasTree::Validate(buf))
			return new ChristmasTree(buf);
		break;
	case FSM_DIGITALPUBLISHER:
		if (DigitalPublisher::Validate(buf))
			return new DigitalPublisher(buf);
		break;
	case FSM_FADE:
		if (Fade::Validate(buf))
			return new Fade(buf);
		break;
	case FSM_MIMIC:
		if (Mimic::Validate(buf))
			return new Mimic(buf);
		break;
	case FSM_TOGGLE:
		if (Toggle::Validate(buf))
			return new Toggle(buf);
		break;
	}
	return (FSM*)NULL;
}

bool FSM::IsValid() const
{
	switch (parameters[0])
	{
	case FSM_ANALOGPUBLISHER:
		return AnalogPublisher::Validate(parameters);
	case FSM_BATTERYMONITOR:
		return BatteryMonitor::Validate(parameters);
	case FSM_BLINK:
		return Blink::Validate(parameters);
	case FSM_CHRISTMASTREE:
		return ChristmasTree::Validate(parameters);
	case FSM_DIGITALPUBLISHER:
		return DigitalPublisher::Validate(parameters);
	case FSM_FADE:
		return Fade::Validate(parameters);
	case FSM_MIMIC:
		return Mimic::Validate(parameters);
	case FSM_TOGGLE:
		return Toggle::Validate(parameters);
	}
	return false;
}
