#include "TinyBuffer.h"


TinyBuffer::TinyBuffer(const TinyBuffer &other, uint16_t len) : bytes(other.bytes)
{
	// Don't use len if the other buffer's size is smaller than that
	length = (len < other.length ? len : other.length);
}

bool TinyBuffer::operator==(const TinyBuffer &other) const
{
	if (length != other.length)
		return false;
	for (uint16_t i = 0; i < length; ++i)
		if (bytes[i] != other.bytes[i])
			return false;
	return true;
}

TinyBuffer &TinyBuffer::operator>>(uint16_t i)
{
	if (i > length)
		i = length;
	bytes += i;
	length -= i;
	return *this;
}

void TinyBuffer::Dump(uint8_t *buffer) const
{
	*reinterpret_cast<uint16_t*>(buffer) = length;
	for (uint16_t i = 0; i < length; i++)
		buffer[i + 2] = bytes[i];
}
