#include "ByteUtils.h"

using namespace ByteUtils;

void Serialize(unsigned long n, unsigned char *buffer)
{
	n &= 0xFFFFFFFF;
	buffer[0] = n >> 24; // big endian
	buffer[1] = n >> 16 & 0xFF;
	buffer[2] = n >> 8 & 0xFF;
	buffer[3] = n & 0xFF;
}

unsigned long Deserialize(const unsigned char *buffer)
{
	return static_cast<unsigned long>(buffer[0]) << 24 |
	       static_cast<unsigned long>(buffer[1]) << 16 |
	       static_cast<unsigned long>(buffer[2]) << 8 | buffer[3];
}
