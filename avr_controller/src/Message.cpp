#include "Message.h"

using namespace AVR::Message;

bool ListFSM::OnReceive(const TinyBuffer &message)
{
	TinyBuffer msg(message);
	m_fsmList.clear();
	size_t len = msg.Length();
	//size_t marker = 0;
	if (len && msg[0] == MSG_MASTER_LIST_FSM)
	{
		msg >> 1; // Skip the msg ID
		while (msg.Length())
		{
			unsigned char fsm_size = msg[0];
			if (fsm_size < 2 || fsm_size > msg.Length())
				break; // Bad data, bail out

			// First byte is parameter count, skip it for FSM creation
			msg >> 1;
			fsm_size--;

			try
			{
				FSMContainer fsm(TinyBuffer(msg, fsm_size));
				if (fsm.IsValid())
					m_fsmList.push_back(fsm);
			}
			catch (std::runtime_error &e)
			{
				// Invalid FSM, pass
			}

			msg >> fsm_size;
		}
		return true;
	}
	return false;
}
