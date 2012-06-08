/*
 * Copyright (C) 2011-2112 Garrett Brown <gbruin@ucla.edu>
 *
 * This Program is free software; you can redistribute it and/or modify it
 * under the terms of the Modified BSD License.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the organization nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * This Program is distributed AS IS in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

// RoboteQ AX3500 motor controller multi-threaded interface

#include "AVRController.h"

#include "/home/garrett/ros/mecanum/avr/include/AddressBook.h"

//#include <boost/bind.hpp>
#include <boost/asio.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/shared_array.hpp>
//#include <boost/thread.hpp>
//#include <boost/thread/condition.hpp>
//#include <boost/tuple/tuple.hpp>

#include <iostream>
#include <vector>

AVRController::AVRController() : m_io(), m_port(m_io)
{
}

AVRController::~AVRController()
{
	Close();
}

bool AVRController::Open(const std::string &device)
{
	Close();

	m_port.open(device.c_str());
	if (!m_port.is_open())
		return false;

	typedef boost::asio::serial_port_base asio_serial;
	m_port.set_option(asio_serial::baud_rate(115200));
	m_port.set_option(asio_serial::character_size(8));
	m_port.set_option(asio_serial::stop_bits(asio_serial::stop_bits::one));
	m_port.set_option(asio_serial::parity(asio_serial::parity::none));
	m_port.set_option(asio_serial::flow_control(asio_serial::flow_control::none));

	// Initialize v_fsm with the list of FSMs currently running on the Arduino
	QueryAllFromAVR(v_fsm);

	m_deviceName = device;
	return true;
}

void AVRController::Close()
{
	m_deviceName = "";
	v_fsm.clear();
	if (m_port.is_open())
		m_port.close();
}

bool AVRController::Reset()
{
	return Open(m_deviceName);
}

bool AVRController::IsOpen()
{
	return m_port.is_open();
}

void AVRController::DigitalWrite(unsigned char pin, bool value)
{
	// Use the ...
}

















void AVRController::GetByPin(unsigned char pin, std::vector<AVR_FSM> &fsmv) const
{
	for (std::vector<AVR_FSM>::const_iterator it = v_fsm.begin(); it != v_fsm.end(); ++it)
	{
		switch (it->GetID())
		{
		case FSM_ANALOGPUBLISHER:
		case FSM_BLINK:
		case FSM_DIGITALPUBLISHER:
		case FSM_FADE:
		case FSM_MIMIC:
		case FSM_TOGGLE:
			// These FSMs use the next property to specify the pin ID
			if (it->GetProperty(1) == pin && !Contains(fsmv, *it))
				fsmv.push_back(*it);
			break;
		case FSM_CHRISTMASTREE:
			// Christmas tree uses five PWM pins:
			if ((pin == LED_UV || pin == LED_RED || pin == LED_YELLOW ||
					pin == LED_GREEN || pin == LED_EMERGENCY) && !Contains(fsmv, *it))
				fsmv.push_back(*it);
			break;
		case FSM_BATTERYMONITOR:
			// Battery monitor uses four LEDs:
			if ((pin == LED_BATTERY_EMPTY || pin == LED_BATTERY_LOW ||
					pin == LED_BATTERY_MEDIUM || pin == LED_BATTERY_HIGH) && !Contains(fsmv, *it))
				fsmv.push_back(*it);
			break;
		default:
			break;
		}
	}
}

void AVRController::GetByID(unsigned char fsm_id, std::vector<AVR_FSM> &fsmv) const
{
	for (std::vector<AVR_FSM>::const_iterator it = v_fsm.begin(); it != v_fsm.end(); ++it)
		if (it->GetID() == fsm_id && !Contains(fsmv, *it))
			fsmv.push_back(*it);
}

bool AVRController::Contains(const std::vector<AVR_FSM> &fsmv, const AVR_FSM &fsm) const
{
	return std::find(fsmv.begin(), fsmv.end(), fsm) != fsmv.end();
	/*
	for (std::vector<AVR_FSM>::const_iterator it = fsmv.begin(); it != fsmv.end(); ++it)
		if (*it == fsm)
			return true;
	return false;
	*/
}

void AVRController::LoadFSM(const AVR_FSM &fsm)
{
	// First, check to see if the FSM is already loaded
	if (Contains(v_fsm, fsm))
		return;

	// Get a list of conflicting FSMs
	std::vector<AVR_FSM> conflicts;
	switch (fsm.GetID())
	{
	case FSM_ANALOGPUBLISHER:
	case FSM_BLINK:
	case FSM_DIGITALPUBLISHER:
	case FSM_FADE:
	case FSM_MIMIC:
	case FSM_TOGGLE:
		GetByPin(fsm[1], conflicts);
		break;
	case FSM_CHRISTMASTREE:
		GetByPin(LED_UV, conflicts);
		GetByPin(LED_RED, conflicts);
		GetByPin(LED_YELLOW, conflicts);
		GetByPin(LED_GREEN, conflicts);
		GetByPin(LED_EMERGENCY, conflicts);
		break;
	case FSM_BATTERYMONITOR:
		GetByPin(LED_BATTERY_EMPTY, conflicts);
		GetByPin(LED_BATTERY_LOW, conflicts);
		GetByPin(LED_BATTERY_MEDIUM, conflicts);
		GetByPin(LED_BATTERY_HIGH, conflicts);
		break;
	default:
		break;
	}

	// Unload the conflicts and load the new FSM
	for (std::vector<AVR_FSM>::iterator it = conflicts.begin(); it != conflicts.end(); ++it)
		UnloadFSM(*it);

	// TODO: Send the FSM to FSM_MASTER

	v_fsm.push_back(fsm);
}


bool AVRController::ResetAndLoadAll()
{
	std::vector<AVR_FSM> copy = v_fsm;
	if (!Reset())
		return false;
	for (std::vector<AVR_FSM>::iterator it = copy.begin(); it != copy.end(); ++it)
		LoadFSM(*it);
	return true;
}

void AVRController::QueryAllFromAVR(std::vector<AVR_FSM> &fsmv)
{
	// Ask FSM_MASTER for a list of its FSMs and build the array into fsmv
}

void AVRController::UnloadFSM(const AVR_FSM &fsm)
{
	// First, make sure the FSM is loaded
	if (!Contains(v_fsm, fsm))
		return;

	// TODO: Send the unload command to FSM_MASTER

	std::remove(v_fsm.begin(), v_fsm.end(), fsm);
}

/**
 * Send a message to a FSM on the Arduino. If fsm doesn't exist in v_fsm,
 * this function returns false.
 */
bool AVRController::MessageFSM(const AVR_FSM &fsm, unsigned char *msg, size_t length)
{
	if (!Contains(v_fsm, fsm))
		return false;

	// TODO: Compose and send the message to fsm[0];

	return true;
}














bool AVRController::SetDTR(bool level)
{
	int fd = m_port.native();
	int status;
	if (ioctl(fd, TIOCMGET, &status) >= 0)
	{
		if (level)
			status |= TIOCM_DTR;
		else
			status &= ~TIOCM_DTR;

		if (ioctl(fd, TIOCMSET, &status) >= 0)
			return true;
	}
	return false;
}
