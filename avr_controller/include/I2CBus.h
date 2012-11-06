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
#pragma once

#include <vector>

#define INVALID_DESCRIPTOR -1

class I2CBus
{
public:
	/**
	 * I2CBus encapsulates the opening/closing of /dev/i2c-* devices. After
	 * calling Open(), the file descriptor can be retrieved with File() to be
	 * used with ioctl(). I2CBus provides quasi-RAII; when the object falls out
	 * of scope, the file descriptor will automatically be closed.
	 */
	I2CBus(unsigned int i2cbus) : m_i2cbus(i2cbus), m_fd(INVALID_DESCRIPTOR) { }
	~I2CBus() throw() { Close(); }

	bool Open();
	bool IsOpen() const { return m_fd >= 0; }
	void Close() throw();

	/**
	 * Return the file descriptor of the opened device. This will be valid
	 * after Open() returns true and until Close() is called or the object
	 * falls out of scope.
	 */
	int File() const { return m_fd; }

	/**
	 * Scan an I2C bus for devices. Use SMBus "read byte" commands for probing.
	 * This is known to lock SMBus on various write-only chips (most notably
	 * clock chips at address 0x69).
	 *
	 * \param  devices The addresses of the detected devices. If an error occurs
	 *                 and this returns false, devices may or may not be modified.
	 * \return true if no error occurred; otherwise, false is returned and the
	 *                 error message is printed to cerr.
	 */
	bool DetectDevices(std::vector<unsigned int> &devices);

private:
	/**
	 * This object is noncopyable.
	 */
	I2CBus(const I2CBus &other);
	I2CBus& operator=(const I2CBus &rhs);

	unsigned int m_i2cbus;
	int m_fd;
};
