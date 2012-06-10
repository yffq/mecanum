/*
 * ByteUtils.h
 *
 *  Created on: Jun 9, 2012
 *      Author: garrett
 */

#ifndef BYTEUTILS_H_
#define BYTEUTILS_H_

namespace ByteUtils
{
	/**
	 * Precondition: buffer's size is at least 4. To achieve compatibility with
	 * 8-bit processors, only the lowest 4 bytes of n are serialized.
	 * Postcondition: buffer contains n in big endian format.
	 */
	void Serialize(unsigned long n, unsigned char *buffer)
	{
		n &= 0xFFFFFFFF;
		buffer[0] = n >> 24; // big endian
		buffer[1] = n >> 16 & 0xFF;
		buffer[2] = n >> 8 & 0xFF;
		buffer[3] = n & 0xFF;
	}

	/**
	 * The pre- and post-conditions are similar to above, but probably reversed.
	 */
	void Deserialize(const unsigned char *buffer, unsigned long &n)
	{
		n = static_cast<unsigned long>(buffer[0]) << 24 |
		    static_cast<unsigned long>(buffer[1]) << 16 |
		    static_cast<unsigned long>(buffer[2]) << 8 | buffer[3];
	}
}

#endif // BYTEUTILS_H_
