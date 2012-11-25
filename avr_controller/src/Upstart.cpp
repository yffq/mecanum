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

#include "Upstart.h"
#include "GPIO.h"
#include "BeagleBoardAddressBook.h"
#include "ParamServer.h"

#include <unistd.h> // for usleep
#include <string>

#include <iostream>

#define SHUTDOWN_COMMAND "./system_shutdown" // in current bin directory
#define ARDUINO_PORT "/dev/ttyACM0"
#define BUTTON_TIMEOUT 5000000UL  // 5.0s
//#define POWER_TIMEOUT 5000000UL  // 5.0s

#define RED_FADE 1000

using namespace std;

int main(int argc, char **argv)
{
	Upstart justdoit;
	justdoit.Main();
}
void Upstart::Main()
{
	// Wait 10 seconds
	usleep(10000000L);

	GPIO arduino1(ARDUINO_BRIDGE1);
	arduino1.Open();
	arduino1.SetDirection(GPIO::OUT, 0);

	// Wait 20 seconds
	usleep(20000000L);

	// Connect to the Arduino
	arduino.Open(ARDUINO_PORT);

	// Wait 2 seconds
	usleep(2000000L);

	GPIO arduino2(ARDUINO_BRIDGE2);
	arduino2.Open();
	arduino2.SetDirection(GPIO::OUT, 0);

	// Rev up the threads
	m_bRunning = true;

	boost::thread tempGreen(boost::bind(&Upstart::GreenThreadRun, this));
	m_greenThread.swap(tempGreen);

	//boost::thread tempYellow(boost::bind(&Upstart::YellowThreadRun, this));
	//m_yellowThread.swap(tempYellow);

	boost::thread tempRed(boost::bind(&Upstart::RedThreadRun, this));
	m_redThread.swap(tempRed);

	//boost::thread tempThumbwheel(boost::bind(&Upstart::ThumbwheelThreadRun, this));
	//m_thumbwheelThread.swap(tempThumbwheel);

	//boost::thread tempIMU(boost::bind(&Upstart::IMUThreadRun, this));
	//m_IMUThread.swap(tempIMU);

	// Run until all threads have completed
	m_greenThread.join();
	m_redThread.join();

	GPIO arduino3(ARDUINO_BRIDGE3);
	arduino3.Open();
	arduino3.SetDirection(GPIO::OUT, 0);
}

void Upstart::GreenThreadRun()
{
	GPIO gpio(BUTTON_GREEN);
	gpio.Open();

	gpio.SetDirection(GPIO::IN);
	gpio.SetEdge(GPIO::BOTH);

	/*
	ParamServer::BatteryMonitor bm;
	string fsm = bm.GetString();
	arduino.DestroyFSM(fsm); // Make sure FSM isn't running before we start
	*/

	GPIO arduino4(ARDUINO_BRIDGE4);
	arduino4.Open();
	arduino4.SetDirection(GPIO::OUT, 0);

	enum STATE
	{
		ENABLED = 0,
		DISABLED = 1
	} state = DISABLED;

	unsigned long duration = 0;
	unsigned int post_value = 0;

	while (m_bRunning)
	{
		try
		{
			if (gpio.Poll(BUTTON_TIMEOUT, duration, true, post_value))
			{
				// No timeout
				if (post_value == 0)
				{
					// Pressed
					if (state == DISABLED)
					{
						//arduino.CreateFSM(fsm);
						arduino4.SetValue(1);
						state = ENABLED;
					}
					else
					{
						//arduino.DestroyFSM(fsm);
						arduino4.SetValue(0);
						state = DISABLED;
					}
				}
				else
				{
					// Depressed
				}
			}
			else
			{
				// Timeout
			}
		}
		catch (const GPIO::Exception &e)
		{
			m_bRunning = false;
		}
	}
	//arduino.DestroyFSM(fsm);
	arduino4.SetValue(1);
}

void Upstart::RedThreadRun()
{
	GPIO gpio(BUTTON_RED);
	gpio.Open();

	gpio.SetDirection(GPIO::IN);
	gpio.SetEdge(GPIO::BOTH);

	ParamServer::ChristmasTree xmastree;
	string strXmastree = xmastree.GetString();
	ParamServer::Fade fade;
	fade.SetPin(LED_EMERGENCY);
	fade.SetPeriod(RED_FADE);
	fade.SetDelay(50);
	fade.SetCurve(1);
	string strFade = fade.GetString();
	arduino.DestroyFSM(strFade); // Make sure FSM isn't running before we start

	unsigned long duration = 0;
	unsigned int post_value = 0;

	while (m_bRunning)
	{
		try
		{
			if (gpio.Poll(BUTTON_TIMEOUT, duration, true, post_value))
			{
				// No timeout
				if (post_value == 0)
					arduino.CreateFSM(strFade);
				else
					arduino.DestroyFSM(strFade);
			}
			else
			{
				// Timed out
				if (gpio.GetValue() == 0)
				{
					// Timed out on press waiting for depress
					m_bRunning = false;
					arduino.DestroyFSM(strFade);
					int result = system(SHUTDOWN_COMMAND);
					(void)result;
				}
				else
				{
					// Timed out on depress waiting for press
				}
			}
		}
		catch (const GPIO::Exception &e)
		{
			m_bRunning = false;
		}
	}
	arduino.DestroyFSM(strFade);
}
