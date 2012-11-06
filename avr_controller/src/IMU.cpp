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

#include <vector>
#include <algorithm> // for std::find()

#define I2C_ADDRESS_ADXL345       0x53
#define I2C_ADDRESS_ITG3200       0x68

// ADXL345 Register Map
#define	ADXL345_DEVID             0x00   // Device ID Register
#define ADXL345_THRESH_TAP        0x1D   // Tap Threshold
#define ADXL345_OFSX              0x1E   // X-axis offset
#define ADXL345_OFSY              0x1F   // Y-axis offset
#define ADXL345_OFSZ              0x20   // Z-axis offset
#define ADXL345_DUR               0x21   // Tap Duration
#define ADXL345_Latent            0x22   // Tap latency
#define ADXL345_Window            0x23   // Tap window
#define ADXL345_THRESH_ACT        0x24   // Activity Threshold
#define ADXL345_THRESH_INACT      0x25   // Inactivity Threshold
#define ADXL345_TIME_INACT        0x26   // Inactivity Time
#define ADXL345_ACT_INACT_CTL     0x27   // Axis enable control for activity and inactivity detection
#define ADXL345_THRESH_FF         0x28   // free-fall threshold
#define ADXL345_TIME_FF           0x29   // Free-Fall Time
#define ADXL345_TAP_AXES          0x2A   // Axis control for tap/double tap
#define ADXL345_ACT_TAP_STATUS    0x2B   // Source of tap/double tap
#define ADXL345_BW_RATE           0x2C   // Data rate and power mode control
#define ADXL345_POWER_CTL         0x2D   // Power Control Register
#define ADXL345_INT_ENABLE        0x2E   // Interrupt Enable Control
#define ADXL345_INT_MAP           0x2F   // Interrupt Mapping Control
#define ADXL345_INT_SOURCE        0x30   // Source of interrupts
#define ADXL345_DATA_FORMAT       0x31   // Data format control
#define ADXL345_DATAX0            0x32   // X-Axis Data 0
#define ADXL345_DATAX1            0x33   // X-Axis Data 1
#define ADXL345_DATAY0            0x34   // Y-Axis Data 0
#define ADXL345_DATAY1            0x35   // Y-Axis Data 1
#define ADXL345_DATAZ0            0x36   // Z-Axis Data 0
#define ADXL345_DATAZ1            0x37   // Z-Axis Data 1
#define ADXL345_FIFO_CTL          0x38   // FIFO control
#define ADXL345_FIFO_STATUS       0x39   // FIFO status

// Power Control Register Bits
#define ADXL345_WU_0              (1<<0)   // Wake Up Mode - Bit 0
#define ADXL345_WU_1              (1<<1)   // Wake Up mode - Bit 1
#define ADXL345_SLEEP             (1<<2)   // Sleep Mode
#define ADXL345_MEASURE           (1<<3)   // Measurement Mode
#define ADXL345_AUTO_SLP          (1<<4)   // Auto Sleep Mode bit
#define ADXL345_LINK              (1<<5)   // Link bit

// Interrupt Enable/Interrupt Map/Interrupt Source Register Bits
#define ADXL345_OVERRUN           (1<<0)
#define ADXL345_WATERMARK         (1<<1)
#define ADXL345_FREE_FALL         (1<<2)
#define ADXL345_INACTIVITY        (1<<3)
#define ADXL345_ACTIVITY          (1<<4)
#define ADXL345_DOUBLE_TAP        (1<<5)
#define ADXL345_SINGLE_TAP        (1<<6)
#define ADXL345_DATA_READY        (1<<7)

// Data Format Bits
#define ADXL345_RANGE_2G          (0<<0)
#define ADXL345_RANGE_4G          (1<<0)
#define ADXL345_RANGE_8G          (1<<1)
#define ADXL345_RANGE_16G         (1<<0)|(1<<1)
#define ADXL345_JUSTIFY           (1<<2)
#define ADXL345_FULL_RES          (1<<3)

#define ADXL345_INT_INVERT        (1<<5)
#define ADXL345_SPI               (1<<6)
#define ADXL345_SELF_TEST         (1<<7)

// ITG3200 Register Map
#define ITG3200_WHO_AM_I          0x00 // Who Am I
#define ITG3200_SMPLRT_DIV        0x15 // Sample Rate Divider
#define ITG3200_DLPF_FS           0x16 // DLPF, Full Scale
#define ITG3200_INT_CFG           0x17 // Interrupt Configuration
#define ITG3200_INT_STATUS        0x1A // Interrupt Status
#define ITG3200_TEMP_OUT_H        0x1B // Temperature Data High
#define ITG3200_TEMP_OUT_L        0x1C // Temperature Data Low
#define ITG3200_GYRO_XOUT_H       0x1D // Gyro-X Data High
#define ITG3200_GYRO_XOUT_L       0x1E // Gyro-X Data Low
#define ITG3200_GYRO_YOUT_H       0x1F // Gyro-Y Data High
#define ITG3200_GYRO_YOUT_L       0x20 // Gyro-Y Data Low
#define ITG3200_GYRO_ZOUT_H       0x21 // Gyro-Z Data High
#define ITG3200_GYRO_ZOUT_L       0x22 // Gyro-Z Data Low
#define ITG3200_PWR_MGM           0x3E // Power Management

// DLPF, Full Scale Register Bits
// FS_SEL must be set to 3 for proper operation
// Set DLPF_CFG to 3 for 1kHz Fint and 42 Hz Low Pass Filter
#define ITG3200_DLPF_CFG_0        (1<<0)
#define ITG3200_DLPF_CFG_1        (1<<1)
#define ITG3200_DLPF_CFG_2        (1<<2)
#define ITG3200_DLPF_FS_SEL_0     (1<<3)
#define ITG3200_DLPF_FS_SEL_1     (1<<4)

// Power Management Register Bits
// Recommended to set CLK_SEL to 1,2 or 3 at startup for more stable clock
#define ITG3200_PWR_MGM_CLK_SEL_0 (1<<0)
#define ITG3200_PWR_MGM_CLK_SEL_1 (1<<1)
#define ITG3200_PWR_MGM_CLK_SEL_2 (1<<2)
#define ITG3200_PWR_MGM_STBY_Z    (1<<3)
#define ITG3200_PWR_MGM_STBY_Y    (1<<4)
#define ITG3200_PWR_MGM_STBY_X    (1<<5)
#define ITG3200_PWR_MGM_SLEEP     (1<<6)
#define ITG3200_PWR_MGM_H_RESET   (1<<7)

// Interrupt Configuration Bits
#define ITG3200_INT_CFG_ACTL         (1<<7)
#define ITG3200_INT_CFG_OPEN         (1<<6)
#define ITG3200_INT_CFG_LATCH_INT_EN (1<<5)
#define ITG3200_INT_CFG_INT_ANYRD    (1<<4)
#define ITG3200_INT_CFG_ITG_RDY_EN   (1<<2)
#define ITG3200_INT_CFG_RAW_RDY_EN   (1<<0)


using namespace std;

bool IMU::Open()
{
	if (i2c.Open() && accInt.Open() && gyroInt.Open())
	{
		vector<unsigned int> devices;
		if (i2c.DetectDevices(devices) &&
				find(devices.begin(), devices.end(), I2C_ADDRESS_ADXL345) != devices.end() &&
				find(devices.begin(), devices.end(), I2C_ADDRESS_ITG3200) != devices.end())
		{
			if (InitAcc() && InitGyro())
				return true;
			else
				cerr << "IMU::Open - Failed to initialize accelerometer and gyroscope settings" << endl;
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
	i2c.Close();
	accInt.Close();
	gyroInt.Close();
}

bool IMU::Select(Device device)
{
	return ioctl(i2c.File(), I2C_SLAVE, device == ACC ? I2C_ADDRESS_ADXL345 : I2C_ADDRESS_ITG3200) >= 0;
}

bool IMU::InitAcc()
{
	bool ret = true;

	ret &= Select(ACC);

	// Set the range to +/- 4G (using the same resolution as 2G)
	ret &= (i2c_smbus_write_byte_data(i2c.File(), ADXL345_DATA_FORMAT, ADXL345_RANGE_4G | ADXL345_FULL_RES) >= 0);

	// Default ADXL345 rate is 100 Hz. Perfect!

	// Put the accelerometer in MEASURE mode
	ret &= (i2c_smbus_write_byte_data(i2c.File(), ADXL345_POWER_CTL, ADXL345_MEASURE) >= 0);

	return ret;
}

bool IMU::InitGyro()
{
	bool ret = true;

	ret &= Select(GYRO);

	// Set internal clock to 1kHz with 42Hz LPF and Full Scale to 3 for proper operation
	ret &= (i2c_smbus_write_byte_data(i2c.File(), ITG3200_DLPF_FS,
			ITG3200_DLPF_FS_SEL_0 | ITG3200_DLPF_FS_SEL_1 | ITG3200_DLPF_CFG_0) >= 0);

	// Set sample rate divider for 100 Hz operation (1KHz / (9 + 1))
	ret &= (i2c_smbus_write_byte_data(i2c.File(), ITG3200_SMPLRT_DIV, 9) >= 0);

	// Setup the interrupt to trigger when new data is ready
	ret &= (i2c_smbus_write_byte_data(i2c.File(), ITG3200_INT_CFG,
			ITG3200_INT_CFG_RAW_RDY_EN /* | ITG3200_INT_CFG_ITG_RDY_EN */) >= 0);

	// Select X gyro PLL for clock source
	ret &= (i2c_smbus_write_byte_data(i2c.File(), ITG3200_PWR_MGM, ITG3200_PWR_MGM_CLK_SEL_0) >= 0);

	return ret;
}

bool IMU::GetFrame(Frame &frame)
{
	bool ret = true;

	Select(ACC);
	// ADXL345 outputs little endian. Allocate three shorts so the data is memory-aligned
	int16_t acc_read[3];
	ret &= i2c_smbus_read_i2c_block_data(i2c.File(), ADXL345_DATAX0, sizeof(acc_read),
			reinterpret_cast<uint8_t*>(acc_read)) >= 0;
	frame.x = acc_read[0];
	frame.y = acc_read[1];
	frame.z = acc_read[2];

	Select(GYRO);
	// ITG3200 outputs big endian. Conversion is necessary, so alignment doesn't matter
	uint8_t gyro_read[8];
	ret &= i2c_smbus_read_i2c_block_data(i2c.File(), ITG3200_TEMP_OUT_H, sizeof(gyro_read), gyro_read) >= 0;
	frame.temp = (gyro_read[0] << 8) | gyro_read[1];
	frame.xRot = (gyro_read[2] << 8) | gyro_read[3];
	frame.yRot = (gyro_read[4] << 8) | gyro_read[5];
	frame.zRot = (gyro_read[6] << 8) | gyro_read[7];

	return ret;
}
