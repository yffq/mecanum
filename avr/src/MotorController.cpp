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

#include "MotorController.h"
#include "ArduinoAddressBook.h"

#include <Arduino.h>
#include <limits.h> // for ULONG_MAX

#define TIMEOUT 1000 // ms
#define FOREVER (ULONG_MAX / 2)

MotorController::MotorController() :
	FiniteStateMachine(FSM_MOTORCONTROLLER, GetBuffer()),
	m_bMessaged(false)
{
	pinMode(MOTOR1_PWM, OUTPUT);
	pinMode(MOTOR1_A, OUTPUT);
	pinMode(MOTOR1_B, OUTPUT);

	pinMode(MOTOR2_PWM, OUTPUT);
	pinMode(MOTOR2_A, OUTPUT);
	pinMode(MOTOR2_B, OUTPUT);

	pinMode(MOTOR3_PWM, OUTPUT);
	pinMode(MOTOR3_A, OUTPUT);
	pinMode(MOTOR3_B, OUTPUT);

	pinMode(MOTOR4_PWM, OUTPUT);
	pinMode(MOTOR4_A, OUTPUT);
	pinMode(MOTOR4_B, OUTPUT);

	analogWrite(MOTOR1_PWM, 0);
	analogWrite(MOTOR2_PWM, 0);
	analogWrite(MOTOR3_PWM, 0);
	analogWrite(MOTOR4_PWM, 0);

	// Debug
	pinMode(LED_BATTERY_FULL, OUTPUT);
}

MotorController *MotorController::NewFromArray(const TinyBuffer &params)
{
	return Validate(params) ? new MotorController() : (MotorController*)0;
}

uint32_t MotorController::Step()
{
	// If Step() was called after being messaged, set MecanumMaster's delay to TIMEOUT
	if (m_bMessaged)
	{
		m_bMessaged = false;
		return TIMEOUT;
	}

	// Timeout occurred (no messages received for duration TIMEOUT)
	// Stop the motors while waiting for the next message
	analogWrite(MOTOR1_PWM, 0);
	analogWrite(MOTOR2_PWM, 0);
	analogWrite(MOTOR3_PWM, 0);
	analogWrite(MOTOR4_PWM, 0);

	// Wait dormant until the next message
	return FOREVER;
}

bool MotorController::Message(const TinyBuffer &msg)
{
	if (msg.Length() == ParamServer::MotorControllerSubscriberMsg::GetLength())
	{
		// Debug
		static uint8_t enable = 1;
		digitalWrite(LED_BATTERY_FULL, enable);
		enable = 1 - enable;


		ParamServer::MotorControllerSubscriberMsg message(msg);
		uint8_t a, b, pwm;

		a = message.GetMotor1() >= 0 ? 1 : 0;
		b = 1 - a;
		pwm = message.GetMotor1() >= 0 ? message.GetMotor1() : -message.GetMotor1();
		digitalWrite(MOTOR1_A, a);
		digitalWrite(MOTOR1_B, b);
		analogWrite(MOTOR1_PWM, pwm);

		a = message.GetMotor2() >= 0 ? 1 : 0;
		b = 1 - a;
		pwm = message.GetMotor2() >= 0 ? message.GetMotor2() : -message.GetMotor2();
		digitalWrite(MOTOR2_A, a);
		digitalWrite(MOTOR2_B, b);
		analogWrite(MOTOR2_PWM, pwm);

		a = message.GetMotor3() >= 0 ? 1 : 0;
		b = 1 - a;
		pwm = message.GetMotor3() >= 0 ? message.GetMotor3() : -message.GetMotor3();
		digitalWrite(MOTOR3_A, a);
		digitalWrite(MOTOR3_B, b);
		analogWrite(MOTOR3_PWM, pwm);

		a = message.GetMotor4() >= 0 ? 1 : 0;
		b = 1 - a;
		pwm = message.GetMotor4() >= 0 ? message.GetMotor4() : -message.GetMotor4();
		digitalWrite(MOTOR4_A, a);
		digitalWrite(MOTOR4_B, b);
		analogWrite(MOTOR4_PWM, pwm);

		ParamServer::MotorControllerPublisherMsg msg;
		msg.SetMotor1cs(analogRead(MOTOR1_CS));
		msg.SetMotor2cs(analogRead(MOTOR2_CS));
		msg.SetMotor3cs(analogRead(MOTOR3_CS));
		msg.SetMotor4cs(analogRead(MOTOR4_CS));
		Serial.write(msg.GetBytes(), msg.GetLength());

		// Return true to let MecanumMaster update the timeout
		m_bMessaged = true;
		return true;
	}
	return false;
}
