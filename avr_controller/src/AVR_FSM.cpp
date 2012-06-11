#include "AVR_FSM.h"



bool AVR_FSM::operator==(const AVR_FSM &other) const
{
	if (boost::asio::buffer_size(parameters) != boost::asio::buffer_size(other.parameters))
		return false;
	for (unsigned int i = 0; i < boost::asio::buffer_size(parameters); ++i)
		if (parameters[i] != other.parameters[i])
			return false;
	return true;
}


AVR_Blink::AVR_Blink(boost::asio::const_buffer parameters) : AVR_FSM(FSM_BLINK, m_params, sizeof(m_params))
{
	for (unsigned int i = 0; i < sizeof(m_params); ++i)
		m_params[i] = parameters[i];
}

