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
#include "ParamServer.h"
#include "BBExpansionPin.h"
#include "I2CBus.h"
#include "IMU.h"
#include "Thumbwheel.h"

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define BUTTON_TIMEOUT 10000000L  // 10.0s
#define THUMBWHEEL_SETTLE 1000000 // 1.0s

bool bTestButtons = true;
bool bTestAVR = true;

void TestButton(const char *color, unsigned int expansionPin)
{
	BBExpansionPin gpio(expansionPin);
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
		EXPECT_NO_THROW(TestButton("green", 3)); // 139
}

TEST(GPIOTest, yellow)
{
	if (bTestButtons)
		EXPECT_NO_THROW(TestButton("yellow", 5)); // 138
}

TEST(GPIOTest, red)
{
	if (bTestButtons)
		EXPECT_NO_THROW(TestButton("red", 8)); // 143
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
		ASSERT_TRUE(arduino.Open("/dev/ttyACM0"));
		ASSERT_TRUE(arduino.IsOpen());

		vector<string> fsmv;
		EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
		EXPECT_GT(fsmv.size(), 0);

		for (vector<string>::const_iterator it = fsmv.begin(); it != fsmv.end(); it++)
			arduino.DestroyFiniteStateMachine(*it);

		EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
		EXPECT_EQ(fsmv.size(), 0);

		string xmastree;
		xmastree.push_back((uint8_t)FSM_CHRISTMASTREE);
		arduino.CreateFiniteStateMachine(xmastree);

		EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
		EXPECT_EQ(fsmv.size(), 1);
	}
}

void TestBridge(unsigned int beaglePin, unsigned int arduinoPin)
{
	ASSERT_TRUE(arduino.IsOpen());

	BBExpansionPin gpio(beaglePin);
	ASSERT_TRUE(gpio.Open());
	EXPECT_NO_THROW(gpio.SetDirection(GPIO::IN));
	EXPECT_NO_THROW(gpio.SetEdge(GPIO::BOTH));
	EXPECT_EQ(gpio.GetValue(), 1);

	// Record the initial number of FSMs
	vector<string> fsmv;
	EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
	size_t initialLength = fsmv.size();

	// Create a toggle FSM. On creation it will pull the pin low
	string toggle;
	toggle.push_back((char)FSM_TOGGLE);
	toggle.push_back((char)arduinoPin);
	arduino.CreateFiniteStateMachine(toggle);
	EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
	ASSERT_EQ(fsmv.size(), initialLength + 1); // Don't continue if the FSM hasn't been installed

	usleep(1000);
	EXPECT_EQ(gpio.GetValue(), 0);

	// Turn the pin on
	ParamServer::ToggleSubscriberMsg toggleMsg(arduinoPin, 1);
	string strMsg(reinterpret_cast<const char*>(toggleMsg.GetBuffer()), toggleMsg.GetLength());
	arduino.Send(strMsg);
	usleep(1000);
	EXPECT_EQ(gpio.GetValue(), 1);

	// Remove the FSM, pin is pulled low as a post-condition
	arduino.DestroyFiniteStateMachine(toggle);
	EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
	ASSERT_EQ(fsmv.size(), initialLength);
	usleep(1000);
	EXPECT_EQ(gpio.GetValue(), 0);

	// Test subscribing to the BeagleBoard's GPIO pins
	string digitalPub;
	uint32_t delay = 100000; // ms (run on command only)
	digitalPub.push_back((char)FSM_DIGITALPUBLISHER);
	digitalPub.push_back((char)arduinoPin);
	digitalPub.append(reinterpret_cast<char*>(&delay), sizeof(delay));
	arduino.CreateFiniteStateMachine(digitalPub);
	EXPECT_TRUE(arduino.ListFiniteStateMachines(fsmv));
	ASSERT_EQ(fsmv.size(), initialLength + 1);

	// Test the pin
	EXPECT_NO_THROW(gpio.SetDirection(GPIO::OUT, 1));
	ParamServer::DigitalPublisherSubscriberMsg msg(arduinoPin);
	string strMsg2(reinterpret_cast<const char*>(msg.GetBuffer()), msg.GetLength());
	string strResponse;
	EXPECT_TRUE(arduino.Query(strMsg2, strResponse, 1000)); // 1s
	ASSERT_TRUE(strResponse.length() == ParamServer::DigitalPublisherPublisherMsg::GetLength());
	ParamServer::DigitalPublisherPublisherMsg res(reinterpret_cast<const uint8_t*>(strResponse.c_str()));
	EXPECT_TRUE(res.GetValue() == 1);

	// Flip the pin and test again
	EXPECT_NO_THROW(gpio.SetValue(0));
	ASSERT_TRUE(arduino.Query(strMsg2, strResponse, 1000)); // 1s
	ASSERT_TRUE(strResponse.length() == ParamServer::DigitalPublisherPublisherMsg::GetLength());
	ParamServer::DigitalPublisherPublisherMsg res2(reinterpret_cast<const uint8_t*>(strResponse.c_str()));
	EXPECT_TRUE(res2.GetValue() == 0);
}

TEST(AVRTest, bridge1)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(14, BEAGLEBOARD_BRIDGE1)); // 162
}

TEST(AVRTest, bridge2)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(10, BEAGLEBOARD_BRIDGE2)); // 145
}

TEST(AVRTest, bridge3)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(16, BEAGLEBOARD_BRIDGE3)); // 161
}

TEST(AVRTest, bridge4)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(18, BEAGLEBOARD_BRIDGE4)); // 159
}

TEST(AVRTest, bridge5)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(9, BEAGLEBOARD_BRIDGE5)); // 136
}

TEST(AVRTest, bridge6)
{
	if (bTestAVR)
		EXPECT_NO_THROW(TestBridge(12, BEAGLEBOARD_BRIDGE6)); // 158
}

TEST(I2CTest, detect)
{
	I2CBus i2c2(2);
	ASSERT_TRUE(i2c2.Open());
	vector<unsigned int> devices;
	ASSERT_TRUE(i2c2.DetectDevices(devices));
	EXPECT_EQ(devices.size(), 2);
	EXPECT_TRUE(std::find(devices.begin(), devices.end(), 0x53) != devices.end());
	EXPECT_TRUE(std::find(devices.begin(), devices.end(), 0x68) != devices.end());
}

TEST(IMUTest, imu)
{
	IMU imu;
	ASSERT_TRUE(imu.Open());

	int16_t xyz[3];

	usleep(10000 * 16);
	EXPECT_TRUE(imu.GetFrame(xyz));
	cout << "*** X: " << xyz[0] << endl;
	cout << "*** Y: " << xyz[1] << endl;
	cout << "*** Z: " << xyz[2] << endl;
	cout << "***********" << endl;

	usleep(10000 * 32);
	EXPECT_TRUE(imu.GetFrame(xyz));
	cout << "*** X: " << xyz[0] << endl;
	cout << "*** Y: " << xyz[1] << endl;
	cout << "*** Z: " << xyz[2] << endl;
	cout << "***********" << endl;


}

int main(int argc, char **argv)
{
	cout << "Test hardware buttons? (y/n)" << endl;
	bTestButtons = (cin.get() == 'y');
	cin.ignore(1000, '\n');

	cout << "Test AVR? (y/n)" << endl;
	bTestAVR = (cin.get() == 'y');
	cin.ignore(1000, '\n');

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
