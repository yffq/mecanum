#ifndef BYTEUTILS_H
#define BYTEUTILS_H


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
namespace ByteUtils
{
	/**
	 * Precondition: buffer's size is at least 4. To achieve compatibility with
	 * 8-bit processors, only the lowest 4 bytes of n are serialized.
	 * Postcondition: buffer contains n in big endian format.
	 */
	static void Serialize(unsigned long n, unsigned char *buffer);

	/**
	 * The pre- and post-conditions are similar to above, but probably reversed.
	 */
	static void Deserialize(const unsigned char *buffer, unsigned long &n) { n = Deserialize(buffer); }
	static unsigned long Deserialize(const unsigned char *buffer);
}

#endif // BYTEUTILS_H
