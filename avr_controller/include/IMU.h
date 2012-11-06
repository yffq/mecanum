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

#include "BBExpansionPin.h"
#include "I2CBus.h"

#include <boost/thread.hpp>
#include <time.h> // for timespec

class IMU
{
public:
	IMU() : m_i2c(2), m_accInt(22), m_gyroInt(21), m_bRunning(false), m_frame() { }
	~IMU() throw() { Close(); }

	bool Open();
	bool IsOpen() const { return m_i2c.IsOpen(); } // All three resources are opened together
	void Close() throw();

	struct Frame
	{
		float x;
		float y;
		float z;
		int16_t xrot;
		int16_t yrot;
		int16_t zrot;
		int16_t temp;
		timespec timestamp; // timestamp of the latter sample
	};

	bool GetFrame(Frame &frame);

private:
	/**
	 * Set the target of the next read or write operation.
	 */
	enum Device { ACC, GYRO };
	bool Select(Device device);

	bool InitAcc();
	bool InitGyro();

	void AccRun();
	void GyroRun();

	I2CBus         m_i2c;
	boost::mutex   m_i2cMutex;
	BBExpansionPin m_accInt;
	BBExpansionPin m_gyroInt;

	volatile bool  m_bRunning;
	boost::thread  m_accThread;
	boost::thread  m_gyroThread;

	Frame          m_frame;
	boost::mutex   m_frameMutex;
};
