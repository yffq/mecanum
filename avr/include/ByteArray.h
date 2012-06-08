#ifndef BYTEARRAY_H
#define BYTEARRAY_H


class ByteArray
{
public:
	ByteArray() : length(0) { }

	ByteArray(unsigned char *bytes, unsigned char length) : bytes(bytes), length(length) { }

	ByteArray(const ByteArray &other) : bytes(other.bytes), length(other.length) { }

	ByteArray &operator=(const ByteArray &src)
	{
		if (this != &src)
		{
			bytes = src.bytes;
			length = src.length;
		}
		return *this;
	}

	~ByteArray() { }

	bool operator==(const ByteArray &other) const
	{
		if (length != other.length)
			return false;
		for (unsigned char i = 0; i < length; ++i)
			if (bytes[i] != other.bytes[i])
				return false;
		return true;
	}

	unsigned char* &operator[](unsigned char i) const { return bytes[i]; }

	void operator>>(unsigned char i) { Shift(i); }

	void Shift(unsigned char i)
	{
		if (i > length) i = length;
		bytes += i;
		length -= i;
	}

	/**
	 * Buffer's length equals this object's Length() plus one.
	 *
	void PrependLength(unsigned char *buffer)
	{
		buffer[0] = length;
		for (unsigned char i = 0; i < length; ++i)
			buffer[i + 1] = bytes[i];
	}
	/**/

	unsigned char Length() const { return length; }

	/**
	 * Precondition: buffer's size is at least 4. To achieve compatibility with
	 * 8-bit processors, only the lowest 4 bytes of n are serialized.
	 * Postcondition: buffer contains n in big endian format.
	 */
	static void Serialize(unsigned long n, unsigned char *buffer)
	{
		n &= 0xFFFFFFFF;
		buffer[0] = n >> 24; // big endian
		buffer[1] = n >> 16 & 0xFF;
		buffer[2] = n >> 8 & 0xFF;
		buffer[3] = n & 0xFF;
	}

	/**
	 * The pre- and post-conditions are similar to above, but reversed.
	 */
	static void Deserialize(const unsigned char *buffer, unsigned long &n)
	{
		n = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
	}

private:
	unsigned char *bytes;
	unsigned char length;
};

#endif // BYTEARRAY_H
