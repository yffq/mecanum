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

#include "I2CBus.h"
#include "i2c-dev.h"  // There are two of these, one at <linux/i2c-dev.h> and one from i2c-tools

#include <errno.h>    // for errno
#include <fcntl.h>    // for open()
#include <iostream>   // for cerr
#include <stdio.h>    // for snprintf()
#include <string.h>   // for strerror()

#define I2C_BUS_FILENAME "/dev/i2c-%d"

#define I2C_FIRST_ADDRESS 0x03 // Set to 0x00 for non-regular addresses
#define I2C_LAST_ADDRESS  0x77 // Set to 0x7F for non-regular addresses

using namespace std;

bool I2CBus::Open()
{
	if (!IsOpen())
	{
		char filename[sizeof(I2C_BUS_FILENAME) + 2]; // Allow 4 digits for the bus number
		snprintf(filename, sizeof(filename), I2C_BUS_FILENAME, m_i2cbus);
		m_fd = open(filename, O_RDWR);
		return m_fd >= 0;
	}
	return true;
}

void I2CBus::Close() throw()
{
	if (IsOpen())
	{
		close(m_fd);
		m_fd = INVALID_DESCRIPTOR;
	}
}

bool I2CBus::DetectDevices(vector<unsigned int> &devices)
{
	unsigned long funcs;
	if (ioctl(m_fd, I2C_FUNCS, &funcs) < 0)
	{
		cerr << "I2CBus::DetectDevices - Could not get the adapter functionality matrix";
		return false;
	}
	if (!(funcs & I2C_FUNC_SMBUS_READ_BYTE))
	{
		cerr << "I2CBus::DetectDevices - Can't use SMBus Read Byte command on this bus";
		return false;
	}

	devices.clear();

	for (unsigned int i = I2C_FIRST_ADDRESS; i < I2C_LAST_ADDRESS; i++)
	{
		// Set the slave address
		if (ioctl(m_fd, I2C_SLAVE, i) < 0)
		{
			if (errno == EBUSY)
			{
				// Device (or maybe bus?) is busy...
				continue;
			}
			else
			{
				char address[4];
				snprintf(address, sizeof(address), "0x%02x", i);
				cerr << "I2CBus::DetectDevices - Could not set address to " <<
						address << ": " << strerror(errno);
				return false;
			}
		}

		// Probe the address
		if (i2c_smbus_read_byte(m_fd) >= 0)
			devices.push_back(i);
	}
	return true;
}
