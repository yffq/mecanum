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

#include "AVRController.h"
#include "BeagleBoardAddressBook.h"
#include "ParamServer.h"
#include "I2CBus.h"
#include "IMU.h"
#include "MotorController.h"
#include "Thumbwheel.h"

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define ARDUINO_PORT "/dev/ttyACM0"
#define BUTTON_TIMEOUT 10000000L  // 10.0s

bool bTestButtons = true;
bool bTestAVR = true;
bool bTestIMU = true;

void TestButton(const char *color, unsigned int expansionPin)
{
	GPIO gpio(expansionPin);
	ASSERT_TRUE(gpio.Open());
	EXPECT_NO_THROW(gpio.SetDirection(GPIO::IN));
	EXPECT_NO_THROW(gpio.SetEdge(GPIO::BOTH));
	unsigned int value;
	EXPECT_NO_THROW(value = gpio.GetValue());
	EXPECT_EQ(value, 1);

	cout << "Press the " << color << " button (" << (BUTTON_TIMEOUT / 1000000) << "s)" << endl;
	unsigned long duration = 0;
	unsigned int post_value = 2;
	EXPECT_TRUE(gpio.Poll(BUTTON_TIMEOUT, duration, true, post_value));
	EXPECT_TRUE(duration > 0);
	EXPECT_EQ(post_value, 0);
}

TEST(GPIOTest, green)
{
	if (bTestButtons)
		EXPECT_NO_THROW(TestButton("green", BUTTON_GREEN));
}

TEST(GPIOTest, yellow)
{
	if (bTestButtons)
		EXPECT_NO_THROW(TestButton("yellow", BUTTON_YELLOW));
}

TEST(GPIOTest, red)
{
	if (bTestButtons)
		EXPECT_NO_THROW(TestButton("red", BUTTON_RED));
}

void TestThumbwheel(Thumbwheel &tw, unsigned int target)
{
	if (tw.GetValue() != target)
	{
		cout << "Move thumbwheel to " << target << " (" << (BUTTON_TIMEOUT / 1000000) << "s)" << endl;
		unsigned int value;
		EXPECT_TRUE(tw.Poll(BUTTON_TIMEOUT, value));
		EXPECT_EQ(value, target);
	}
}

TEST(GPIOTest, thumnbwheel)
{
	if (bTestButtons)
	{
		Thumbwheel tw;
		ASSERT_TRUE(tw.Open());
		EXPECT_NO_THROW(TestThumbwheel(tw, 0));
		EXPECT_NO_THROW(TestThumbwheel(tw, 1));
		EXPECT_NO_THROW(TestThumbwheel(tw, 2));
		EXPECT_NO_THROW(TestThumbwheel(tw, 4));
	}
}

AVRController arduino;

TEST(AVRTest, fsm)
{
	if (bTestAVR)
	{
		ASSERT_TRUE(arduino.Open(ARDUINO_PORT));
		ASSERT_TRUE(arduino.IsOpen());

		vector<string> fsmv;
		EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
		// Assume at least 1 FSM is loaded on startup
		EXPECT_GT(fsmv.size(), 0);

		for (vector<string>::const_iterator it = fsmv.begin(); it != fsmv.end(); it++)
			arduino.DestroyFiniteStateMachine(*it);

		EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
		EXPECT_EQ(fsmv.size(), 0);

		ParamServer::ChristmasTree xmastree;
		arduino.CreateFiniteStateMachine(xmastree.GetString());

		EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
		EXPECT_EQ(fsmv.size(), 1);
	}
}

void TestBridge(unsigned int beaglePin, unsigned int arduinoPin)
{
	ASSERT_TRUE(arduino.IsOpen());

	GPIO gpio(beaglePin);
	ASSERT_TRUE(gpio.Open());
	EXPECT_NO_THROW(gpio.SetDirection(GPIO::IN));
	EXPECT_NO_THROW(gpio.SetEdge(GPIO::BOTH));
	EXPECT_EQ(gpio.GetValue(), 1);

	// Record the initial number of FSMs
	vector<string> fsmv;
	EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
	size_t initialLength = fsmv.size();

	// Create a toggle FSM. On creation it will pull the pin low
	ParamServer::Toggle toggle;
	toggle.SetPin((unsigned char)arduinoPin);
	arduino.CreateFiniteStateMachine(toggle.GetString());
	EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
	ASSERT_EQ(fsmv.size(), initialLength + 1); // Don't continue if the FSM hasn't been installed

	usleep(1000);
	EXPECT_EQ(gpio.GetValue(), 0);

	// Turn the pin on
	ParamServer::ToggleSubscriberMsg toggleMsg;
	toggleMsg.SetPin(arduinoPin);
	toggleMsg.SetCommand(1);
	arduino.Send(toggleMsg.GetString());
	usleep(1000);
	EXPECT_EQ(gpio.GetValue(), 1);

	// Remove the FSM, pin is pulled low as a post-condition
	arduino.DestroyFiniteStateMachine(toggle.GetString());
	EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
	ASSERT_EQ(fsmv.size(), initialLength);
	usleep(1000);
	EXPECT_EQ(gpio.GetValue(), 0);

	// Test subscribing to the BeagleBoard's GPIO pins
	ParamServer::DigitalPublisher digitalPub;
	digitalPub.SetPin((unsigned char)arduinoPin);
	digitalPub.SetDelay(100000);
	arduino.CreateFiniteStateMachine(digitalPub.GetString());
	EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
	ASSERT_EQ(fsmv.size(), initialLength + 1);

	// Test the pin
	EXPECT_NO_THROW(gpio.SetDirection(GPIO::OUT, 1));
	ParamServer::DigitalPublisherSubscriberMsg dpMsg;
	dpMsg.SetPin(arduinoPin);
	string strResponse;
	EXPECT_TRUE(arduino.Query(dpMsg.GetString(), strResponse, 1000)); // 1s
	ASSERT_TRUE(strResponse.length() == ParamServer::DigitalPublisherPublisherMsg::GetLength());
	ParamServer::DigitalPublisherPublisherMsg dbResponse(strResponse);
	EXPECT_TRUE(dbResponse.GetValue() == 1);

	// Flip the pin and test again
	strResponse.clear();
	EXPECT_NO_THROW(gpio.SetValue(0));
	ASSERT_TRUE(arduino.Query(dpMsg.GetString(), strResponse, 1000)); // 1s
	ASSERT_TRUE(strResponse.length() == ParamServer::DigitalPublisherPublisherMsg::GetLength());
	ParamServer::DigitalPublisherPublisherMsg dbResponse2(strResponse);
	EXPECT_TRUE(dbResponse2.GetValue() == 0);
}

TEST(AVRTest, bridge1)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(ARDUINO_BRIDGE1, BEAGLEBOARD_BRIDGE1));
}

TEST(AVRTest, bridge2)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(ARDUINO_BRIDGE2, BEAGLEBOARD_BRIDGE2));
}

TEST(AVRTest, bridge3)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(ARDUINO_BRIDGE3, BEAGLEBOARD_BRIDGE3));
}

TEST(AVRTest, bridge4)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(ARDUINO_BRIDGE4, BEAGLEBOARD_BRIDGE4));
}

TEST(AVRTest, bridge5)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(ARDUINO_BRIDGE5, BEAGLEBOARD_BRIDGE5));
}

TEST(AVRTest, bridge6)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(ARDUINO_BRIDGE6, BEAGLEBOARD_BRIDGE6));
}

TEST(I2CTest, detect)
{
	if (bTestIMU)
	{
		I2CBus i2c2(2);
		ASSERT_TRUE(i2c2.Open());
		vector<unsigned int> devices;
		ASSERT_TRUE(i2c2.DetectDevices(devices));
		EXPECT_EQ(devices.size(), 2);
		EXPECT_TRUE(std::find(devices.begin(), devices.end(), 0x53) != devices.end());
		EXPECT_TRUE(std::find(devices.begin(), devices.end(), 0x68) != devices.end());
	}
}

TEST(IMUTest, imu)
{
	if (bTestIMU)
	{
		IMU imu;
		ASSERT_TRUE(imu.Open());

		usleep(1000 * 50); // 50ms (5 samples)
		IMU::Frame frame = {0, 0, 0, 0, 0, 0, 0, {0, 0}};
		imu.GetFrame(frame);
		cout << "*********************" << endl;
		cout << "*** X: " << frame.x << "g" << endl;
		cout << "*** Y: " << frame.y << "g" << endl;
		cout << "*** Z: " << frame.z << "g" << endl;
		cout << "*** XRot: " << frame.xrot << "°/s" << endl;
		cout << "*** YRot: " << frame.yrot << "°/s" << endl;
		cout << "*** ZRot: " << frame.zrot << "°/s" << endl;
		cout << "*** Temp: " << (frame.temp * 9 / 5 + 32) << "°F" << endl;
		cout << "*********************" << endl;
		EXPECT_NE(frame.x, 0);
		EXPECT_NE(frame.y, 0);
		EXPECT_NE(frame.z, 0);
		EXPECT_NE(frame.xrot, 0);
		EXPECT_NE(frame.yrot, 0);
		EXPECT_NE(frame.zrot, 0);
		EXPECT_NE(frame.temp, 0);
	}
}
/*
TEST(MotorController, setSpeed)
{
	if (!arduino.IsOpen())
		ASSERT_TRUE(arduino.Open(ARDUINO_PORT));
	ASSERT_TRUE(arduino.IsOpen());

	MotorController motors;
	ASSERT_TRUE(motors.Connect(&arduino));
	motors.SetSpeed(0, 0, 0, 0);
	usleep(10 * 1000);
	motors.SetSpeed(-20, -20, -20, -20);
	usleep(10 * 1000);
}
*/
int main(int argc, char **argv)
{
	cout << "Test hardware buttons? (y/n)" << endl;
	bTestButtons = (cin.get() == 'y');
	cin.ignore(1000, '\n');

	cout << "Test AVR? (y/n)" << endl;
	bTestAVR = (cin.get() == 'y');
	cin.ignore(1000, '\n');

	cout << "Test IMU? (y/n)" << endl;
	bTestIMU = (cin.get() == 'y');
	cin.ignore(1000, '\n');

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
