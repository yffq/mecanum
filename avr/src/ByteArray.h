#ifndef BYTEARRAY_H
#define BYTEARRAY_H


/**
 * ByteArray is an implementation of a resizable array using data held on the
 * stack while avoiding the heap. Instead of allocating memory, the underlying
 * byte array is provided by the caller and this class manages access to the
 * data and the length of the array.
 *
 * To save space, an 8-bit unsigned bytes is used to keep track of the length.
 * This means that an array can be at most 255 bytes. In the future, ByteArray
 * may be given some template love so that the array length can be an int
 * or a long to promote reusability.
 */
class ByteArray
{
public:
	/**
	 * Construct an invalid byte array of zero bytes. If the ByteArray is
	 * resized using << or SetLength(), attempting to dereference the
	 * underlying array will cause an error.
	 */
	ByteArray() : bytes(0), length(0) { }

	/**
	 * At its simplest, a ByteArray is a convenient data structure to manage an
	 * array while keeping the pointer and the length in a centralized location.
	 */
	ByteArray(unsigned char *bytes, unsigned char length) : bytes(bytes), length(length) { }

	/**
	 * Copying a ByteArray is a cheap operation because the underlying storage
	 * is provided by the owner.
	 */
	ByteArray(const ByteArray &other) : bytes(other.bytes), length(other.length) { }

	/**
	 * Assigning a ByteArray to itself is a valid operation. Normally, we would
	 * check for a self-copy (this != &src), but because no dynamic memory is
	 * being swapped, we can avoid this check and save cpu instruction without
	 * sacrificing correctness.
	 */
	ByteArray &operator=(const ByteArray &src) { bytes = src.bytes; length = src.length; return *this; }

	~ByteArray() { }

	/**
	 * Two ByteArrays are equal if their arrays are equal (both content and
	 * length).
	 */
	bool operator==(const ByteArray &other) const;

	/**
	 * Referencing a ByteArray index accesses the underlying array.
	 */
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
	 * Note: Doogie. That is all.
	 */
	void Dump(unsigned char *buffer) const;

	/**
	 * Get the length.
	 */
	unsigned char Length() const { return length; }

	/**
	 * Set the length. Make sure that the size of the underlying array is at
	 * least this long.
	 */
	void SetLength(unsigned char newLength) { length = newLength; }


	/***** Static utility methods *****/

	/**
	 * Precondition: buffer's size is at least 4. To achieve compatibility with
	 * 8-bit processors, only the lowest 4 bytes of n are serialized.
	 * Postcondition: buffer contains n in big endian format.
	 */
	static void Serialize(unsigned long n, unsigned char *buffer);

	/**
	 * The pre- and post-conditions are similar to above, but probably reversed.
	 */
	static void Deserialize(const unsigned char *buffer, unsigned long &n);

private:
	unsigned char *bytes;
	unsigned char length;
};

#endif // BYTEARRAY_H
