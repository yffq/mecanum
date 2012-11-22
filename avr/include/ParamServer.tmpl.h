#ifndef PARAMSERVER_H
#define PARAMSERVER_H

#if !defined(__ARM__) || !defined(__AVR__)
#pragma error("Must compile for ARM or AVR")
#endif

#include "ArduinoAddressBook.h"
#include <string.h> // for memcpy()

#if defined(__AVR__)
#include "TinyBuffer.h"
#elif defined(__ARM__)
#include <string>
#endif

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
to the parent object. A FSM's publisher and subscriber messages are rendered
below the parent FSM.

When a tag containing a "." is encountered, it is replaced with the attribute
belong to the object refered to in the first part of the tag. Concerning
capitalization, the attributes value's first letter is made to have the same
case as the attributes's name. The rest of the string remains unchanged.

The tag <%,%> has special meaning. The templater looks ahead, and the comma is
only rendered if the encompasing tag is repeated at least once more.

In this template, the "id" and "length" parameters are implicit and do not need
to be included in the header files.
--%>

<%FSM
class <%FSM.Name%>
{
public:
	<%FSM.Name%>() { m_params.id = <%FSM.ID%>; }
	<%FSM.Name%>(const uint8_t *bytes) { memcpy(&m_params, bytes, sizeof(Parameters)); }
#if defined(__ARM__)
	<%FSM.Name%>(const std::string &bytes) { memcpy(&m_params, bytes.c_str(), sizeof(Parameters));}
#endif

	uint8_t GetId() const { return <%FSM.ID%>; }
<%PARAMETER
	<%PARAMETER.type%> Get<%PARAMETER.Name%>() const { return m_params.<%PARAMETER.name%>; }
%>

	const uint8_t *GetBytes() const { return reinterpret_cast<const uint8_t*>(&m_params); }
	static uint16_t GetSize() { return sizeof(Parameters); }
#if defined(__AVR__)
	const TinyBuffer GetBuffer() { return TinyBuffer(reinterpret_cast<uint8_t*>(&m_params), sizeof(m_params)); }
#elif defined(__ARM__)
	const std::string GetString() { return std::string(reinterpret_cast<const char*>(&m_params), sizeof(Parameters)); }
#endif

<%PARAMETER
	void Set<%PARAMETER.Name%>(<%PARAMETER.type%> <%PARAMETER.name%>) { m_params.<%PARAMETER.name%> = <%PARAMETER.name%>; }
%>

#if defined(__AVR__)
	static bool Validate(const TinyBuffer &buffer)
	{
		if (buffer.Length() == sizeof(Parameters))
		{
			Parameters params;
			memcpy(&params, buffer.Buffer(), sizeof(Parameters));
			bool valid = (params.id == <%FSM.ID%>);
<%PARAMETER
			valid &= (ArduinoVerifier::<%PARAMETER.Test%>(params.<%PARAMETER.name%>));
%>
			return valid;
		}
		return false;
	}
#elif defined(__ARM__)
	static bool Validate(const std::string &strBytes)
	{
		if (strBytes.length() == sizeof(Parameters))
		{
			Parameters params;
			memcpy(&params, reinterpret_cast<const uint8_t*>(strBytes.c_str()), sizeof(Parameters));
			bool valid = (params.id == <%FSM.ID%>);
<%PARAMETER
			valid &= (ArduinoVerifier::<%PARAMETER.Test%>(params.<%PARAMETER.name%>));
%>
			return valid;
		}
		return false;
	}
#endif

private:
	struct Parameters
	{
		uint8_t id;
<%PARAMETER
		<%PARAMETER.type%> <%PARAMETER.name%>;
%>
	} __attribute__((packed));

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
