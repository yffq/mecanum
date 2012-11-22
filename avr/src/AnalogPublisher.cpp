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
	FiniteStateMachine(FSM_ANALOGPUBLISHER, m_params.GetBuffer())
{
	m_params.SetPin(pin);
	m_params.SetDelay(delay);
}

AnalogPublisher *AnalogPublisher::NewFromArray(const TinyBuffer &params)
{
	if (ParamServer::AnalogPublisher::Validate(params))
	{
		ParamServer::AnalogPublisher ap(params);
		return new AnalogPublisher(ap.GetPin(), ap.GetDelay());
	}
	return NULL;
}

uint32_t AnalogPublisher::Step()
{
	ParamServer::AnalogPublisherPublisherMsg msg;
	msg.SetPin(m_params.GetPin());
	msg.SetValue(analogRead(m_params.GetPin()));
	Serial.write(msg.GetBytes(), msg.GetLength());
	return m_params.GetDelay();
}

bool AnalogPublisher::Message(const TinyBuffer &msg)
{
	if (msg.Length() == m_params.GetSize())
	{
		ParamServer::AnalogPublisherSubscriberMsg message(msg);
		return message.GetPin() == m_params.GetPin();
	}
	return false;
}
