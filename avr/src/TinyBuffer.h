#ifndef TINYBUFFER_H
#define TINYBUFFER_H

#include <stdint.h>

/**
 * TinyBuffer is an implementation of a resizable array using data held on the
 * stack while avoiding the heap. Instead of allocating memory, the underlying
 * byte array is provided by the caller and this class manages access to the
 * data and the length of the array.
 */
class TinyBuffer
{
public:
	/**
	 * Construct an invalid byte array of zero bytes. If the TinyBuffer is
	 * resized using << or SetLength(), attempting to dereference the
	 * underlying array will cause an error.
	 */
	TinyBuffer() : bytes((uint8_t*)0), length(0) { }

	/**
	 * At its simplest, a TinyBuffer is a convenient data structure to manage an
	 * array while keeping the pointer and the length in a centralized location.
	 */
	TinyBuffer(uint8_t *bytes, uint16_t length) : bytes(bytes), length(length) { }

	/**
	 * Copying a TinyBuffer is a cheap operation because the underlying storage
	 * is provided by the owner.
	 */
	TinyBuffer(const TinyBuffer &other) : bytes(other.bytes), length(other.length) { }

	/**
	 * Use another's data, but set our own length.
	 */
	TinyBuffer(const TinyBuffer &other, uint16_t len);

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
	uint8_t &operator[](uint16_t i) const { return bytes[i]; }

	/**
	 * Decrease the size of the buffer. The byte array is shifted to match the
	 * decreased size (bytes are popped off the front of the array).
	 *
	 * Basically, this "consumes" the number of bytes in the front of the
	 * array.
	 *
	 * If i is larger than the buffer's size, the length is set to 0.
	 */
	TinyBuffer &operator>>(uint16_t i);

	/**
	 * Increase the size of the buffer. This does NOT shift the byte array.
	 * Additional padding is pushed onto the back. Ensure that the buffer is
	 * large enough to handle the increased size (I know yo mamma is).
	 */
	TinyBuffer &operator<<(uint16_t i) { length += i; return *this; }

	/**
	 * Copy the byte array into buffer, with the first two bytes set to the
	 * array's length (little endian).
	 *
	 * Precondition: buffer's length must be greater than this object's length plus two.
	 *
	 * Note: Doogie. That is all.
	 */
	void DumpBuffer(uint8_t *buffer) const;

	/**
	 * Get the length.
	 */
	uint16_t Length() const { return length; }

	/**
	 * Get the underlying buffer.
	 */
	uint8_t *Buffer() const { return bytes; }

	/**
	 * Set the length. Make sure that the size of the underlying array is at
	 * least this long.
	 */
	void SetLength(uint16_t newLength) { length = newLength; }

private:
	uint8_t *bytes;
	uint16_t length;
};

#endif // TINYBUFFER_H
