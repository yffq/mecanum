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

#include <algorithm>
#include <string>
#include <vector>

#include <iostream>

using namespace std;

bool MotorController::Connect(AVRController *avr)
{
	if (!avr || !avr->IsOpen())
	{
		cout << "AVR is not open" << endl;
		return false;
	}

	// Create the FSM if it doesn't exist
	string strFsm;
	strFsm.push_back((char)FSM_MOTORCONTROLLER);
	vector<string> fsmv;
	if (!avr->ListFiniteStateMachines(fsmv))
	{
		cout << "Could not list FSMs" << endl;
		return false;
	}
	if (find(fsmv.begin(), fsmv.end(), strFsm) == fsmv.end())
	{
		cout << "Creating FSM " << fsmv.size() << endl;
		// Doesn't exist, create it now
		avr->CreateFiniteStateMachine(strFsm);

		// Repeat the above check. TODO: Add a parameter, verify, to Create..()
		if (!avr->ListFiniteStateMachines(fsmv))
			return false;
		if (find(fsmv.begin(), fsmv.end(), strFsm) == fsmv.end())
		{
			cout << "Failed to load FSM. Loaded FSMs are:" << fsmv.size() << endl;
			for (vector<string>::const_iterator it = fsmv.begin(); it != fsmv.end(); it++)
				cout << (unsigned int)(*it->c_str()) << endl;

			return false;
		}
	}

	m_avr = avr;
	return true;
}

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
