#include "TinyBuffer.h"


TinyBuffer::TinyBuffer(const TinyBuffer &other, unsigned char len) : bytes(other.bytes)
{
	length = (len < other.length ? len : other.length);
}

bool TinyBuffer::operator==(const TinyBuffer &other) const
{
	if (length != other.length)
		return false;
	for (unsigned char i = 0; i < length; ++i)
		if (bytes[i] != other.bytes[i])
			return false;
	return true;
}

TinyBuffer &TinyBuffer::operator>>(unsigned char i)
{
	if (i > length)
		i = length;
	bytes += i;
	length -= i;
	return *this;
}

void TinyBuffer::Dump(unsigned char *buffer) const
{
	// Copy from end to beginning so that we can prepend the length to ourselves
	for (unsigned char i = length; i > 0; --i)
		buffer[i] = bytes[i - 1];
	buffer[0] = length + 1;
}
