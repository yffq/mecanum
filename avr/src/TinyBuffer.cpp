/*
 *        Copyright (C) 2112 Garrett Brown <gbruin@ucla.edu>
 *
 *  This Program is free software; you can redistribute it and/or modify it
 *  under the terms of the Modified BSD License.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the organization nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 *  This Program is distributed AS IS in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

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

void TinyBuffer::DumpBuffer(uint8_t *buffer) const
{
	*reinterpret_cast<uint16_t*>(buffer) = length + 2;
	for (uint16_t i = 0; i < length; i++)
		buffer[i + 2] = bytes[i];
}
