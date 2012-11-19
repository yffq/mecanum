#ifndef PARAMSERVER_H
#define PARAMSERVER_H

#include "ArduinoAddressBook.h"
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
 * appetite. Now, a straight-forward Python parser and template is used to
 * generate this file.
 */
namespace ParamServer
{

<%--
The templating rules are as follows:

When a tag name is solo (no "." in the name), the surrounded text is repeated
once for each instance of that object. For example, the code in the FSM tag is
instantiated for every FSM discovered in the AVR header files. This rule
applies to subtags as well; these tags are expected to be objects belonging
to the parent object.

When a tag containing a "." is encountered, it is replaced with the attribute
belong to the object refered to in the first part of the tag. Concerning
capitalization, if the subtag is all caps the replacement string is uppercased.
Otherwise, the case of the first letter of the replacement string is made to
match the case of the first letter of the subtag, with the rest of the string
remaining unchanged.

The tag <%,%> has special meaning. The templater looks ahead, and the comma is
only rendered if the encompasing tag is repeated at least once more.

The "id" and "length" parameters are implicit and do not need to be included in
the header files.
--%>

<%FSM
class <%FSM.Name%>
{
public:
	<%FSM.Name%>() { m_params.id = <%FSM.ID%>; }
	<%FSM.Name%>(const uint8_t *bytes) { memcpy(&m_params, bytes, sizeof(Parameters)); }

<%PARAMETER
	<%PARAMETER.type%> Get<%PARAMETER.Name%>() const { return m_params.<%PARAMETER.name%>; }
%>
<%PARAMETER
	void Set<%PARAMETER.Name%>(<%PARAMETER.type%> <%PARAMETER.name%>) { m_params.<%PARAMETER.name%> = <%PARAMETER.name%>; }
%>

	static bool Validate(const uint8_t *bytes, uint16_t length)
	{
		if (length == sizeof(Parameters))
		{
			Parameters params;
			memcpy(&params, bytes, sizeof(Parameters));
			bool valid = (params.id == <%FSM.ID%>);
<%PARAMETER
			valid &= (ArduinoVerifier::<%PARAMETER.test%>(params.<%PARAMETER.name%>));
%>
			return valid;
		}
		return false;
	}

	struct Parameters
	{
<%PARAMETER
		<%PARAMETER.type%> <%PARAMETER.name%>;
%>
	} __attribute__((packed));

protected:
	Parameters m_params;
};

<%MESSAGE
class <%FSM.Name%><%MESSAGE.Which%>Msg
{
public:
	<%FSM.Name%><%MESSAGE.Which%>Msg(<%PARAMETER <%PARAMETER.type%> <%PARAMETER.name%><%,%> %>)
	{
		m_msg.length = sizeof(Message);
		m_msg.id = <%FSM.ID%>;
<%PARAMETER
		m_msg.<%PARAMETER.name%> = <%PARAMETER.name%>;
%>
	}
	<%FSM.Name%><%MESSAGE.Which%>Msg(const uint8_t *bytes) { memcpy(&m_msg, bytes, sizeof(Message)); }

	static uint16_t GetLength() { return sizeof(Message); }
	uint8_t GetId() const { return m_msg.id; }
<%PARAMETER
	<%PARAMETER.type%> Get<%PARAMETER.Name%>() const { return m_msg.<%PARAMETER.name%>; }
%>
	const uint8_t *GetBuffer() const { return reinterpret_cast<const uint8_t*>(&m_msg); }

private:
	struct Message
	{
		uint16_t length;
		uint8_t id;
<%PARAMETER
		<%PARAMETER.type%> <%PARAMETER.name%>;
%>
	} __attribute__((packed));

	Message m_msg;
};
%>


%>

} // namespace ParamServer

#endif // PARAMSERVER_H
