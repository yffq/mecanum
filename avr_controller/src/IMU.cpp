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

#include "IMU.h"
#include "i2c-dev.h"
#include "ADXL345.h"
#include "ITG3200.h"

#include <algorithm> // for std::find()
#include <iostream>  // for cerr
#include <vector>

#define ACCELEROMETER_UPDATE_FREQ 100 // Hz
#define GYROSCOPE_UPDATE_FREQ     100 // Hz

#define I2C_ADDRESS_ADXL345       0x53
#define I2C_ADDRESS_ITG3200       0x68

using namespace std;

bool IMU::Open()
{
	if (IsOpen())
		return true;

	if (m_i2c.Open() && m_accInt.Open() && m_gyroInt.Open())
	{
		vector<unsigned int> devices;
		if (m_i2c.DetectDevices(devices) &&
				find(devices.begin(), devices.end(), I2C_ADDRESS_ADXL345) != devices.end() &&
				find(devices.begin(), devices.end(), I2C_ADDRESS_ITG3200) != devices.end())
		{
			if (InitAcc() && InitGyro())
			{
				m_bRunning = true;
				boost::thread accTemp(boost::bind(&IMU::AccRun, this));
				m_accThread.swap(accTemp);
				boost::thread gyroTemp(boost::bind(&IMU::GyroRun, this));
				m_gyroThread.swap(gyroTemp);
				return true;
			}
			else
			{
				cerr << "IMU::Open - Failed to initialize accelerometer and gyroscope settings" << endl;
			}
		}
		else
		{
			cerr << "IMU::Open - Failed to detect devices 0x53 (ADXL345) and 0x68 (ITG3200)" << endl;
		}
	}
	Close();
	return false;
}

void IMU::Close() throw()
{
	m_bRunning = false;
	m_accThread.join();
	m_gyroThread.join();
	m_i2c.Close();
	m_accInt.Close();
	m_gyroInt.Close();
}

bool IMU::Select(Device device)
{
	return ioctl(m_i2c.File(), I2C_SLAVE, device == ACC ? I2C_ADDRESS_ADXL345 : I2C_ADDRESS_ITG3200) >= 0;
}

bool IMU::InitAcc()
{
	// Interrupt is triggered when logic is high
	m_accInt.SetDirection(GPIO::IN);
	m_accInt.SetEdge(GPIO::RISING);

	bool ret = true;

	ret &= Select(ACC);

	// Default ADXL345 rate is 100 Hz, which is exactly what we want
	ret &= (i2c_smbus_write_byte_data(m_i2c.File(), ADXL345_BW_RATE, ADXL345_DATA_RATE_100_HZ) >= 0);

	// Install the data-ready event on interrupt 1
	ret &= (i2c_smbus_write_byte_data(m_i2c.File(), ADXL345_INT_ENABLE, ADXL345_DATA_READY) >= 0);
	ret &= (i2c_smbus_write_byte_data(m_i2c.File(), ADXL345_INT_MAP, ADXL345_DATA_READY & ADXL345_INTERRUPT1) >= 0);

	// Set the range to +/- 4G (using the same resolution as 2G)
	ret &= (i2c_smbus_write_byte_data(m_i2c.File(), ADXL345_DATA_FORMAT, ADXL345_RANGE_4G | ADXL345_FULL_RES) >= 0);

	// Disable the "patent pending FIFO technology"
	ret &= (i2c_smbus_write_byte_data(m_i2c.File(), ADXL345_FIFO_CTL, ADXL345_BYPASS) >= 0);

	// Put the accelerometer in MEASURE mode
	ret &= (i2c_smbus_write_byte_data(m_i2c.File(), ADXL345_POWER_CTL, ADXL345_MEASURE) >= 0);

	return ret;
}

bool IMU::InitGyro()
{
	// Interrupt is triggered when logic is high
	m_gyroInt.SetDirection(GPIO::IN);
	m_gyroInt.SetEdge(GPIO::RISING);

	bool ret = true;

	ret &= Select(GYRO);

	// Set sample rate divider for 100 Hz operation (1KHz / (9 + 1))
	ret &= (i2c_smbus_write_byte_data(m_i2c.File(), ITG3200_SMPLRT_DIV, 9) >= 0);

	// Set internal clock to 1kHz with 42Hz LPF and Full Scale to 3 for proper operation
	ret &= (i2c_smbus_write_byte_data(m_i2c.File(), ITG3200_DLPF_FS,
		ITG3200_DLPF_FS_SEL_0 | ITG3200_DLPF_FS_SEL_1 | ITG3200_DLPF_CFG_0) >= 0);

	// Setup the interrupt to trigger when new data is ready: Stay high until any register is read
	ret &= (i2c_smbus_write_byte_data(m_i2c.File(), ITG3200_INT_CFG,
		ITG3200_INT_CFG_LATCH_INT_EN | ITG3200_INT_CFG_INT_ANYRD | ITG3200_INT_CFG_RAW_RDY_EN) >= 0);

	/*
	// Select X gyro PLL for clock source
	ret &= (i2c_smbus_write_byte_data(m_i2c.File(), ITG3200_PWR_MGM, ITG3200_PWR_MGM_CLK_SEL_0) >= 0);
	*/

	return ret;
}

void IMU::AccRun()
{
	// ADXL345 outputs little endian. Allocate three shorts so the data is memory-aligned
	int16_t acc_read[3];
	unsigned long duration = 0; // Unused

	while (m_bRunning)
	{
		// Wait twice the period for the interrupt to rise (or a timeout if already risen)
		if (!m_accInt.Poll(2 * 1000000 / ACCELEROMETER_UPDATE_FREQ, duration, false) && m_accInt.GetValue() != 1)
			continue;

		{
			boost::mutex::scoped_lock i2cLock(m_i2cMutex);
			Select(ACC);
			if (i2c_smbus_read_i2c_block_data(m_i2c.File(), ADXL345_DATAX0, sizeof(acc_read),
					reinterpret_cast<uint8_t*>(acc_read)) < 0)
				continue; // what else can we do?
		}

		{
			boost::mutex::scoped_lock frameLock(m_frameMutex);
			m_frame.x = acc_read[0] * 0.0039f;
			m_frame.y = acc_read[1] * 0.0039f;
			m_frame.z = acc_read[2] * 0.0039f;
		}
	}
}

void IMU::GyroRun()
{
	// ITG3200 outputs big endian. Conversion is necessary, so alignment doesn't matter
	uint8_t gyro_read[8];
	unsigned long duration = 0; // Unused

	while (m_bRunning)
	{
		// Wait twice the period for the interrupt to rise (or a timeout if already risen)
		if (!m_gyroInt.Poll(2 * 1000000 / GYROSCOPE_UPDATE_FREQ, duration, false) && m_gyroInt.GetValue() != 1)
			continue;

		{
			boost::mutex::scoped_lock i2cLock(m_i2cMutex);
			Select(GYRO);
			if (i2c_smbus_read_i2c_block_data(m_i2c.File(), ITG3200_TEMP_OUT_H, sizeof(gyro_read), gyro_read) < 0)
				continue; // what else can we do?
		}

		{
			boost::mutex::scoped_lock frameLock(m_frameMutex);
			m_frame.temp = (13200 + static_cast<int16_t>((gyro_read[0] << 8) | gyro_read[1])) / 280.0f + 35;
			m_frame.xrot = static_cast<int16_t>((gyro_read[2] << 8) | gyro_read[3]) / 14.375f;
			m_frame.yrot = static_cast<int16_t>((gyro_read[4] << 8) | gyro_read[5]) / 14.375f;
			m_frame.zrot = static_cast<int16_t>((gyro_read[6] << 8) | gyro_read[7]) / 14.375f;
		}
	}
}

void IMU::GetFrame(Frame &frame)
{
	boost::mutex::scoped_lock frameLock(m_frameMutex);
	frame = m_frame;
}
