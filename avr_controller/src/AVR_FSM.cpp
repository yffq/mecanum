#include "AVR_FSM.h"


AVR_FSM::AVR_FSM(unsigned char *properties, unsigned int length) : m_length(length)
{
	m_properties = new unsigned char[m_length];
	for (unsigned int i = 0; i < m_length; ++i)
		m_properties[i] = properties[i];
}

AVR_FSM::AVR_FSM(const AVR_FSM &other) : m_length(other.m_length)
{
	m_properties = new unsigned char[m_length];
	for (unsigned int i = 0; i < m_length; ++i)
		m_properties[i] = other.m_properties[i];
}

AVR_FSM &AVR_FSM::operator=(const AVR_FSM &src)
{
	if (this != &src)
	{
		// 1. Free all memory in the target instance
		delete[] m_properties;

		// 2. Reallocate memory for the target instance
		m_properties = new unsigned char[src.m_length];

		// 3. Copy data from src into the target instance
		for (unsigned int i = 0; i < src.m_length; ++i)
			m_properties[i] = src.m_properties[i];
		m_length = src.m_length;
	}
	// 4. Return a reference to the target instance
	return *this;
}

bool AVR_FSM::operator==(const AVR_FSM &other) const
{
	if (m_length != other.m_length)
		return false;
	for (unsigned int i = 0; i < m_length; ++i)
		if (m_properties[i] != other.m_properties[i])
			return false;
	return true;
}

