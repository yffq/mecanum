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


#include "AVRController.h"

//#include <boost/bind.hpp>
//#include <boost/asio.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
//#include <boost/thread/condition.hpp>
//#include <boost/tuple/tuple.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp> // for boost::posix_time::milliseconds


#include <iostream>

AVRController::AVRController() : m_io(), m_port(m_io)
{
}

AVRController::~AVRController()
{
	Close();
}

bool AVRController::Open(const std::string &device)
{
	// TODO: Add a new parameter, a function with no parameters, that blocks
	// until the Arduino has finished resetting itself. The function could,
	// for example, wait for the Arduino to pull a GPIO pin low.
	using namespace std;
	Close();

	cout << "Opening port..." << endl;

	m_port.open(device.c_str());
	if (!m_port.is_open())
		return false;

	typedef boost::asio::serial_port_base asio_serial;
	m_port.set_option(asio_serial::baud_rate(115200));
	m_port.set_option(asio_serial::character_size(8));
	m_port.set_option(asio_serial::stop_bits(asio_serial::stop_bits::one));
	m_port.set_option(asio_serial::parity(asio_serial::parity::none));
	m_port.set_option(asio_serial::flow_control(asio_serial::flow_control::none));

	cout << "Port opened, sleeping..." << endl;

	boost::this_thread::sleep(boost::posix_time::milliseconds(4000));

	cout << "Querying FSMs..." << endl;

	// Initialize v_fsm with the list of FSMs currently running on the Arduino
	//AVR::Message::ListFSM *msg = new AVR::Message::ListFSM();
	AVR::Message::ListFSM msg;
	Send(&msg);
	do
	{
		Receive(&msg);
		m_instance.AddMany(msg.GetFSMList());
	} while (msg.Count() > 0);

	cout << "Retrieved " << m_instance.Size() << " FSM" << (m_instance.Size() > 1 ? "s" : "") << endl;

	m_deviceName = device;
	return true;
}

void AVRController::Close()
{
	m_deviceName = "";
	m_instance.Clear();
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

void AVRController::Send(AVR::Message::Command *msg)
{
	//boost::asio::const_buffer buf = msg->GetMessage();
	//write(m_port, buf);
}

void AVRController::Receive(AVR::Message::Response *msg)
{
	/*
	unsigned char msg_size[0];
	unsigned char data[255];
	boost::asio::mutable_buffer buf(msg_size, 1);
	size_t bytesRead = read(m_port, buf);
	if (bytesRead != 1 || msg_size[0] < 2)
		return;
	*/
	/*
	// Read 1 less than the msg size, because msg size includes msg size byte
	boost::asio::mutable_buffer buf2(data, msg_size[0] - 1);
	size_t payloadSize = read(m_port, buf2);
	if (msg_size[0] - 1 != payloadSize)
		return;
	// For now, assume FSM ID is FSM_MASTER and skip the first FSM ID byte
	msg->OnReceive(boost::asio::mutable_buffer(data + 1, payloadSize - 1));
	*/
}


/*
void AVRController::LoadFSM(const AVR_FSM &fsm)
{
	// First, check to see if the FSM is already loaded
	if (m_instance.Contains(fsm))
		return;

	// Get a list of conflicting FSMs
	std::vector<AVR_FSM> conflicts;
	if (m_instance.GetConflicts(fsm, conflicts))
	{
		// Unload the conflicts and load the new FSM
		for (std::vector<AVR_FSM>::iterator it = conflicts.begin(); it != conflicts.end(); ++it)
			UnloadFSM(*it);
	}

	// TODO: Send the FSM to FSM_MASTER

	m_instance.Add(fsm);
}


void dump(const unsigned char *bytes, int length)
{
	using namespace std;
	for (int i = 0; i < length; ++i)
	{
		cout << (int)bytes[i];
		if (i != length - 1)
			cout << ", ";
	}
}

*/


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
