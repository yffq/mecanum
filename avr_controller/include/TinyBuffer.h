#ifndef TINYBUFFER_H
#define TINYBUFFER_H

/**
 * TinyBuffer is an implementation of a resizable array using data held on the
 * stack while avoiding the heap. Instead of allocating memory, the underlying
 * byte array is provided by the caller and this class manages access to the
 * data and the length of the array.
 *
 * To save space, an 8-bit unsigned bytes is used to keep track of the length.
 * This means that an array can be at most 255 bytes. In the future, TinyBuffer
 * may be given some template love so that the array length can be an int
 * or a long to promote reusability.
 */
class TinyBuffer
{
public:
	/**
	 * Construct an invalid byte array of zero bytes. If the TinyBuffer is
	 * resized using << or SetLength(), attempting to dereference the
	 * underlying array will cause an error.
	 */
	TinyBuffer() : bytes(0), length(0) { }

	/**
	 * At its simplest, a TinyBuffer is a convenient data structure to manage an
	 * array while keeping the pointer and the length in a centralized location.
	 */
	TinyBuffer(unsigned char *bytes, unsigned char length) : bytes(bytes), length(length) { }

	/**
	 * Copying a TinyBuffer is a cheap operation because the underlying storage
	 * is provided by the owner.
	 */
	TinyBuffer(const TinyBuffer &other) : bytes(other.bytes), length(other.length) { }

	/**
	 * Use another's data, but set our own length.
	 */
	TinyBuffer(const TinyBuffer &other, unsigned char len);

	/**
	 * Assigning a TinyBuffer to itself is a valid operation. Normally, we would
	 * check for a self-copy (this != &src), but because no dynamic memory is
	 * being swapped, we can avoid this check.
	 */
	TinyBuffer &operator=(const TinyBuffer &src) { bytes = src.bytes; length = src.length; return *this; }

	~TinyBuffer() { }

	/**
	 * Two TinyBuffers are equal if their arrays are equal (both content and
	 * length).
	 */
	bool operator==(const TinyBuffer &other) const;

	/**
	 * Referencing a TinyBuffer index accesses the underlying array.
	 */
	unsigned char &operator[](unsigned char i) const { return bytes[i]; }

	/**
	 * Decrease the size of the buffer. The byte array is shifted to match the
	 * decreased size (bytes are popped off the front of the array).
	 *
	 * Basically, this "consumes" the number of bytes in the front of the
	 * array.
	 *
	 * If i is larger than the buffer's size, the length is set to 0.
	 */
	TinyBuffer &operator>>(unsigned char i);

	/**
	 * This operator is the const form of operator>>(); it will shift the bytes
	 * pointer right, but returns a copy instead of modifying itself in place.
	 */
	TinyBuffer operator+(unsigned char i) const;

	/**
	 * Increase the size of the buffer. This does NOT shift the byte array.
	 * Additional padding is pushed onto the back. Ensure that the buffer is
	 * large enough to handle the increased size (I know yo mamma is).
	 *
	 * The maximum size is 255 (0xFF); if i pushes the size over the limit,
	 * the length will overflow and actually shorted the byte array.
	 */
	TinyBuffer &operator<<(unsigned char i) { length += i; return *this; }

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

private:
	unsigned char *bytes;
	unsigned char length;
};

#endif // TINYBUFFER_H
