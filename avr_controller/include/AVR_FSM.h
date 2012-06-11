#ifndef AVR_FSM_H
#define AVR_FSM_H

#include "ParamServer.h"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

class AVR_FSM
{
protected:
	AVR_FSM(unsigned char id, unsigned char *params, unsigned char len) : parameters(params, len)
	{
		parameters[0] = id;
	}

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
	bool operator==(const AVR_FSM &other) const;

	/*
	static boost::shared_ptr<AVR_FSM> NewFromBuffer(boost::asio::buffer buf)
	{
		AVR_FSM *fsm = NULL;
		switch (buf[0])
		{

		}
		return boost::shared_ptr<AVR_FSM>(new);
	}
	*/

private:
	boost::asio::mutable_buffer parameters;
};



class AVR_Blink : public AVR_FSM, public ParamServer::Blink
{
	AVR_Blink() : AVR_FSM(FSM_BLINK, m_params, sizeof(m_params)) { }
	AVR_Blink(boost::asio::const_buffer parameters);
};

#endif // AVR_FSM_H
