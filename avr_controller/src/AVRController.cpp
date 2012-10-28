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
//#include <boost/thread/condition.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> // for boost::posix_time::milliseconds

#include <boost/bind.hpp>

//#include <boost/thread/locks.hpp> // for boost::lock_guard

#include <iostream>

using namespace std;

AVRController::AVRController() : m_io(), m_port(m_io), m_bRunning(false)
{
}

AVRController::~AVRController()
{
	Close();
}

bool AVRController::Open(const string &device)
{
	// TODO: Add a new parameter, a function with no parameters, that blocks
	// until the Arduino has finished resetting itself. The function could,
	// for example, wait for the Arduino to pull a GPIO pin low.
	Close();

	cout << __FUNCTION__ << " Opening port..." << endl;

	// Open the port, while guarding against other open/close calls
	boost::lock_guard<boost::mutex> lock(m_portMutex);
	m_port.open(device.c_str());
	if (!m_port.is_open())
		return false;

	typedef boost::asio::serial_port_base asio_serial;
	m_port.set_option(asio_serial::baud_rate(115200));
	m_port.set_option(asio_serial::character_size(8));
	m_port.set_option(asio_serial::stop_bits(asio_serial::stop_bits::one));
	m_port.set_option(asio_serial::parity(asio_serial::parity::none));
	m_port.set_option(asio_serial::flow_control(asio_serial::flow_control::none));

	// Wait for the Arduino to reset itself (TODO: What if DTR is off?)
	cout << __FUNCTION__ << " Port opened, sleeping..." << endl;
	//boost::this_thread::sleep(boost::posix_time::seconds(4000));
	boost::asio::deadline_timer timer(m_io, boost::posix_time::seconds(4));
	timer.wait();

	// See "A thread pool for executing arbitrary tasks"
	// http://think-async.com/Asio/Recipes

	/*
	// Give some work to the io_service before it is started. This stops its
	// run() function from exiting immediately.
	//boost::asio::io_service::work work(m_io);
	m_io.post(boost::bind(&AVRController::DoRead, this, 1));
	*/

	// m_ioThread is the thread in which our IO callbacks are executed
	m_bRunning = true;
	boost::thread temp_thread(boost::bind(&AVRController::WriteThreadRun, this));
	m_writeThread.swap(temp_thread);

	InstallAsyncRead();

	m_deviceName = device;
	return true;
}

void AVRController::Close()
{
	m_bRunning = false;
	m_writeQueueCondition.notify_one();
	m_writeThread.join();

	m_deviceName = "";
	boost::lock_guard<boost::mutex> lock(m_portMutex);
	if (m_port.is_open())
	{
		m_port.cancel();
		m_port.close();
	}

	//m_io.reset(); // Does this remove m_port from m_io?
}

bool AVRController::IsOpen()
{
	boost::lock_guard<boost::mutex> lock(m_portMutex);
	return m_port.is_open();
}

void AVRController::Send(const std::string &msg)
{
	boost::mutex::scoped_lock lock(m_writeQueueMutex);
	m_writeQueue.push_back(msg);
	m_writeQueueCondition.notify_one();
}

bool AVRController::Query(const string &msg, string &response, unsigned long timeout)
{
	// Require 2-byte message length and target FSM
	if (msg.length() < 3)
		return false;

	// FSM ID is the third byte (following the 2-byte message length)
	unsigned char fsmId = msg[2];
	Send(msg);
	return Receive(fsmId, response, timeout);
}

bool AVRController::Receive(unsigned int fsmId, string &msg, unsigned long timeout)
{

	return false;
}

void AVRController::WriteThreadRun()
{
	while (true)
	{
		boost::mutex::scoped_lock writeQueueLock(m_writeQueueMutex);

		// On entry, release mutex and suspend this thread. On return, re-acquire mutex
		while (m_writeQueue.empty() && m_bRunning)
			m_writeQueueCondition.wait(writeQueueLock);

		// If we were awaken to exit, then clean up shop and die a quiet death
		if (!m_bRunning)
		{
			m_writeQueue.clear();
			// Note, this is the only exit point of the function
			// Automatically releases the scoped lock upon destruction
			return;
		}

		string msg = m_writeQueue.front();
		m_writeQueue.erase(m_writeQueue.begin());

		// Don't need the lock for the rest of the while loop, let it expire
		writeQueueLock.unlock();

		// Now lock the port mutex
		boost::mutex::scoped_lock portLock(m_portMutex);

		// Cancel the async read to free up the serial port
		m_port.cancel();

		// Write the data synchronously
		boost::asio::write(m_port, boost::asio::buffer(msg.c_str(), msg.length()));

		// Restore the async read task
		InstallAsyncRead();
	}
}

void AVRController::InstallAsyncRead()
{
	m_port.async_read_some(boost::asio::buffer(m_message.GetNextReadBuffer(), m_message.GetNextReadLength()),
		boost::bind(&AVRController::ReadCallback, this, boost::asio::placeholders::error,
		                                 boost::asio::placeholders::bytes_transferred));
}

void AVRController::ReadCallback(const boost::system::error_code &error, size_t bytes_transferred)
{
	// We don't care if async_read_some() was interrupted, use the data anyway
	m_message.Advance(bytes_transferred);
	if (m_message.IsFinished())
	{
		// Process message
		string msg = m_message.GetMessage();



		m_message.Reset();
	}
	boost::mutex::scoped_lock portLock(m_portMutex);
	InstallAsyncRead();
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

void AVRController::Message::Reset()
{
	delete[] m_nextBuffer;
	m_readState = WaitingForLength;
	m_msg.clear();
	m_msgLength = 0;
	m_nextBufferLength = 2;
	m_nextBuffer = new unsigned char[m_nextBufferLength];
}

void AVRController::Message::Advance(size_t bytes)
{
	if (bytes == 0)
		return;

	if (bytes > m_nextBufferLength)
		bytes = m_nextBufferLength;

	switch (m_readState)
	{
	case WaitingForLength:
		if (bytes == 1)
		{
			m_msg.push_back(m_nextBuffer[0]);
			delete[] m_nextBuffer;

			m_readState = WaitingForLengthPt2;
			m_nextBufferLength--;
			m_nextBuffer = new unsigned char[m_nextBufferLength];
		}
		else
		{
			m_msg.push_back(m_nextBuffer[0]);
			m_msg.push_back(m_nextBuffer[1]);
			// Got our message length, use native endian to recover the length
			m_msgLength = *reinterpret_cast<uint16_t*>(m_nextBuffer);
			delete[] m_nextBuffer;

			m_readState = WaitingForMessage;
			m_msg.reserve(m_msgLength);
			m_nextBufferLength = m_msgLength - 2;
			m_nextBuffer = new unsigned char[m_nextBufferLength];
		}
		break;
	case WaitingForLengthPt2:
		m_msg.push_back(m_nextBuffer[0]);
		// Got our message length, use native endian to recover the length
		m_msgLength = *reinterpret_cast<const uint16_t*>(m_msg.c_str());
		delete[] m_nextBuffer;

		m_readState = WaitingForMessage;
		m_msg.reserve(m_msgLength);
		m_nextBufferLength = m_msgLength - m_msg.length();
		m_nextBuffer = new unsigned char[m_nextBufferLength];
		break;
	case WaitingForMessage:
		for (size_t i = 0; i < bytes; i++)
			m_msg.push_back(m_nextBuffer[i]);
		delete[] m_nextBuffer;

		if (m_msgLength == m_msg.length())
		{
			m_readState = Finished;
			m_nextBuffer = NULL;
			m_nextBufferLength = 0;
		}
		else
		{
			m_nextBufferLength = m_msgLength - m_msg.length();
			m_nextBuffer = new unsigned char[m_nextBufferLength];
		}
		break;
	case Finished:
	default:
		break;
	}
}




