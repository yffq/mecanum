#ifndef BYTEARRAY_H
#define BYTEARRAY_H


class ByteArray
{
public:
	ByteArray() : length(0) { }

	ByteArray(unsigned char *bytes, unsigned char length) : bytes(bytes), length(length) { }

	ByteArray(const ByteArray &other) : bytes(other.bytes), length(other.length) { }

	ByteArray &operator=(const ByteArray &src);

	~ByteArray() { }

	bool operator==(const ByteArray &other) const;

	unsigned char &operator[](unsigned char i) const { return bytes[i]; }

	/**
	 * Decrease the size of the buffer. The byte array is shifted to match the
	 * decreased size (bytes are popped off the front of the array).
	 */
	ByteArray &operator>>(unsigned char i);

	/**
	 * Increase the size of the buffer. This does NOT shift the byte array.
	 * Additional padding is pushed onto the back. Ensure that the buffer is
	 * large enough to handle the increased size (I know yo mamma is).
	 *
	 * The maximum size is 255 (0xFF); if i pushes the size over the limit,
	 * the length will overflow and actually shorted the byte array.
	 */
	ByteArray &operator<<(unsigned char i) { length += i; return *this; }

	/**
	 * Copy the byte array into buffer, with the first byte set to the array's
	 * length.  Buffer's length must be greater than this object's Length()
	 * plus one.
	 *
	 * Note: Doogie
	 */
	void Dump(unsigned char *buffer) const;
	//void PrependLengthToSelf() { Dump(bytes); }

	unsigned char Length() const { return length; }
	void SetLength(unsigned char newLength) { length = newLength; }

private:
	unsigned char *bytes;
	unsigned char length;
};

#endif // BYTEARRAY_H
