#include "ByteArray.h"

ByteArray &ByteArray::operator=(const ByteArray &src)
{
	// Skip checking if this != &src, no dynamic memory is being swapped
	bytes = src.bytes;
	length = src.length;
	return *this;
}

bool ByteArray::operator==(const ByteArray &other) const
{
	if (length != other.length)
		return false;
	for (unsigned char i = 0; i < length; ++i)
		if (bytes[i] != other.bytes[i])
			return false;
	return true;
}

ByteArray &ByteArray::operator>>(unsigned char i)
{
	if (i > length)
		i = length;
	bytes += i;
	length -= i;
	return *this;
}

void ByteArray::Dump(unsigned char *buffer) const
{
	// Copy from end to beginning so that we can prepend the length to ourselves
	for (unsigned char i = length; i > 0; --i)
		buffer[i] = bytes[i - 1];
	buffer[0] = length + 1;
}

void ByteArray::Serialize(unsigned long n, unsigned char *buffer)
{
	buffer[0] = n >> 24; // big endian
	buffer[1] = n >> 16 & 0xFF;
	buffer[2] = n >> 8 & 0xFF;
	buffer[3] = n & 0xFF;
}

void ByteArray::Deserialize(const unsigned char *buffer, unsigned long &n)
{
	n = static_cast<unsigned long>(buffer[0]) << 24 |
	    static_cast<unsigned long>(buffer[1]) << 16 |
	    static_cast<unsigned long>(buffer[2]) << 8 | buffer[3];
}
