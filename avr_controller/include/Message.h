#ifndef MESSAGE_H
#define MESSAGE_H

#include "FSMContainer.h"
#include "TinyBuffer.h"

#include "AddressBook.h"

namespace AVR
{

namespace Message
{

class Command
{
protected:
	Command(unsigned char fsm_id, unsigned char *params, unsigned char len) : message(params, len)
	{
		message[0] = len;
		message[1] = fsm_id;
	}

public:
	const TinyBuffer &GetMessage() const { return message; }

	unsigned char operator[](unsigned int i) const { return message[i]; }

	size_t GetMessageSize() const { return message.Length(); }

protected:
	TinyBuffer message;
};

class Response
{
public:
	virtual unsigned char WaitForFSM() = 0;

	/**
	 * Receive the incoming data. The buffer will start with the first byte
	 * if data following the FSM ID (in other words, len(msg) will be two less
	 * than the incoming message because the message length byte and the FSM ID
	 * byte are both dropped).
	 *
	 * @return true if the message was handled.
	 */
	virtual bool OnReceive(const TinyBuffer &msg) = 0;
	virtual ~Response() { }
};


class ListFSM : public Command, public Response
{
public:
	ListFSM() : Command(FSM_MASTER, m_sendData, sizeof(m_sendData))
	{
		m_sendData[2] = MSG_MASTER_LIST_FSM;
	}

	virtual ~ListFSM() { };

	virtual unsigned char WaitForFSM() { return FSM_MASTER; }

	virtual bool OnReceive(const TinyBuffer &message);

	std::vector<FSMContainer> GetFSMList() { return m_fsmList; }

	size_t Count() { return m_fsmList.size(); }

private:
	unsigned char m_sendData[3];
	std::vector<FSMContainer> m_fsmList;
};


} // namespace Message

} // namespace AVR

#endif // MESSAGE_H
