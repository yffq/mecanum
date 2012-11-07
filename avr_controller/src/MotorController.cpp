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
#include "ParamServer.h"

#include <string>

using namespace std;

void MotorController::SetSpeed(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
	// Create the message
	ParamServer::MotorControllerSubscriberMsg command(motor1, motor2, motor3, motor4);
	string strMsg(reinterpret_cast<const char*>(command.GetBuffer()), command.GetLength());
	m_avr->Send(strMsg);

	/*
	string strResponse;
	m_avr->Query(strMsg, strResponse, 10); // 10ms timeout

	ParamServer::MotorControllerPublisherMsg values(reinterpret_cast<const uint8_t*>(strResponse.c_str()));
	uint16_t motor1cs = values.GetMotor1cs();
	uint16_t motor2cs = values.GetMotor2cs();
	uint16_t motor3cs = values.GetMotor3cs();
	uint16_t motor4cs = values.GetMotor4cs();
	*/
}
