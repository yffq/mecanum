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

#include "SentryMonitor.h"
#include "GPIO.h"
#include "BeagleBoardAddressBook.h"
#include "ArduinoAddressBook.h"
#include "ParamServer.h"

#include <unistd.h> // for usleep()
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	SentryMonitor justdoit;
	justdoit.Main();
	return 0;
}

void SentryMonitor::Main()
{
	cout << "Opening Arduino port" << endl;
	GPIO gpio(ARDUINO_BRIDGE1);
	gpio.Open();
	gpio.SetDirection(GPIO::OUT, 0);

	// Connect to the Arduino
	m_arduino.Open(ARDUINO_PORT);

	usleep(1000000); // 1s

	gpio.SetValue(1);

	cout << "Uploading Sentry" << endl;
	ParamServer::Sentry sentry;
	m_arduino.CreateFSM(sentry.GetString());

	string response;
	if (m_arduino.Receive(FSM_SENTRY, response, 5000)) // 3s
	{
		ParamServer::SentryPublisherMsg msg(response);
		cout << "Left:  Ticks: " << (int)msg.GetTicks() << ", Microseconds: " << msg.GetMicroseconds() << endl;
		cout << "Left:  uS/tick:          " << (1.0f * msg.GetMicroseconds() / msg.GetTicks() * 360 / 100) << endl;
		// 1000 us / 24 degrees * 360 degrees / 100 ticks = 30 uS/tick
		cout << "Left:  estimated uS/tick:   -150 uS/tick" << endl;
		cout << "Left:  uS/degree:        " << (1.0f * msg.GetMicroseconds() / msg.GetTicks()) << endl;
		// 1000 us / 24 degrees = 8.33 uS/degree
		cout << "Left:  estimated uS/degree: -41.66 uS/degree" << endl;
		cout << endl;

		if (m_arduino.Receive(FSM_SENTRY, response, 5000)) // 3s
		{
			ParamServer::SentryPublisherMsg msg2(response);
			cout << "Right: Ticks: " << (int)msg2.GetTicks() << ", Microseconds: " << msg2.GetMicroseconds() << endl;
			cout << "Right: uS/tick:   " << (1.0f * msg2.GetMicroseconds() / msg2.GetTicks() * 360 / 100) << endl;
			cout << "Right: uS/degree: " << (1.0f * msg2.GetMicroseconds() / msg2.GetTicks()) << endl;

			gpio.SetValue(0);
			return;
		}
	}
	cout << "Error: Failed to receive response" << endl;
	gpio.SetValue(0);
}
