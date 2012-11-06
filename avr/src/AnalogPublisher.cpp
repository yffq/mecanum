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

#include "AnalogPublisher.h"
#include "ArduinoAddressBook.h"

#include <Arduino.h>


AnalogPublisher::AnalogPublisher(uint8_t pin, uint32_t delay) :
	FiniteStateMachine(FSM_ANALOGPUBLISHER, reinterpret_cast<uint8_t*>(&m_params), sizeof(m_params))
{
	SetPin(pin);
	SetDelay(delay);
}

AnalogPublisher *AnalogPublisher::NewFromArray(const TinyBuffer &params)
{
	if (Validate(params.Buffer(), params.Length()))
	{
		ParamServer::AnalogPublisher ap(params.Buffer());
		return new AnalogPublisher(ap.GetPin(), ap.GetDelay());
	}
	return NULL;
}

uint32_t AnalogPublisher::Step()
{
	ParamServer::AnalogPublisherPublisherMsg msg(GetPin(), analogRead(GetPin()));
	Serial.write(msg.GetBuffer(), msg.GetLength());
	return GetDelay();
}

bool AnalogPublisher::Message(const TinyBuffer &msg)
{
	if (msg.Length() == ParamServer::AnalogPublisherSubscriberMsg::GetLength())
	{
		ParamServer::AnalogPublisherSubscriberMsg message(msg.Buffer());
		return message.GetPin() == GetPin();
	}
	return false;
}
