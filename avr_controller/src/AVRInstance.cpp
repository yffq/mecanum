#include "AVRInstance.h"

#include <algorithm>

/* static */
bool AVRInstance::Contains(const std::vector<FSMContainer> &fsmv, const FSMContainer &fsm)
{
	return std::find(fsmv.begin(), fsmv.end(), fsm) != fsmv.end();
}

void AVRInstance::AddMany(const std::vector<FSMContainer> &fsmv)
{
	v_fsm.insert(v_fsm.end(), fsmv.begin(), fsmv.end());
}

void AVRInstance::Remove(const FSMContainer &fsm)
{
	v_fsm.erase(std::find(v_fsm.begin(), v_fsm.end(), fsm));
}

bool AVRInstance::GetByPin(unsigned char pin, std::vector<FSMContainer> &result) const
{
	size_t initial = result.size();
	for (std::vector<FSMContainer>::const_iterator it = v_fsm.begin(); it != v_fsm.end(); ++it)
	{
		switch (it->GetID())
		{
		case FSM_ANALOGPUBLISHER:
		case FSM_BLINK:
		case FSM_DIGITALPUBLISHER:
		case FSM_FADE:
		case FSM_MIMIC:
		case FSM_TOGGLE:
			// These FSMs use the first property to specify the pin ID
			if ((*it)[1] == pin && !Contains(*it))
				result.push_back(*it);
			break;
		case FSM_CHRISTMASTREE:
			// Christmas tree uses five PWM pins:
			if ((pin == LED_UV || pin == LED_RED || pin == LED_YELLOW ||
					pin == LED_GREEN || pin == LED_EMERGENCY) && !Contains(*it))
				result.push_back(*it);
			break;
		case FSM_BATTERYMONITOR:
			// Battery monitor uses four LEDs:
			if ((pin == LED_BATTERY_EMPTY || pin == LED_BATTERY_LOW ||
					pin == LED_BATTERY_MEDIUM || pin == LED_BATTERY_HIGH) && !Contains(*it))
				result.push_back(*it);
			break;
		default:
			break;
		}
	}
	return initial != result.size();

}

bool AVRInstance::GetByID(unsigned char fsm_id, std::vector<FSMContainer> &result) const
{
	size_t initial = result.size();
	for (std::vector<FSMContainer>::const_iterator it = v_fsm.begin(); it != v_fsm.end(); ++it)
		if (it->GetID() == fsm_id && !Contains(*it))
			result.push_back(*it);
	return initial != result.size();
}

bool AVRInstance::GetConflicts(const FSMContainer &fsm, std::vector<FSMContainer> &result) const
{
	size_t initial = result.size();
	switch (fsm.GetID())
	{
	case FSM_ANALOGPUBLISHER:
	case FSM_BLINK:
	case FSM_DIGITALPUBLISHER:
	case FSM_FADE:
	case FSM_MIMIC:
	case FSM_TOGGLE:
		GetByPin(fsm[1], result);
		break;
	case FSM_CHRISTMASTREE:
		GetByPin(LED_UV, result);
		GetByPin(LED_RED, result);
		GetByPin(LED_YELLOW, result);
		GetByPin(LED_GREEN, result);
		GetByPin(LED_EMERGENCY, result);
		break;
	case FSM_BATTERYMONITOR:
		GetByPin(LED_BATTERY_EMPTY, result);
		GetByPin(LED_BATTERY_LOW, result);
		GetByPin(LED_BATTERY_MEDIUM, result);
		GetByPin(LED_BATTERY_HIGH, result);
		break;
	default:
		break;
	}
	return initial != result.size();
}
