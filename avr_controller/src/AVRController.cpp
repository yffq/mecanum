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

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> // for boost::posix_time::milliseconds
#include <iostream>

using namespace std;

AVRController::AVRController() : m_io(), m_port(m_io), m_bRunning(false)
{
}

AVRController::~AVRController() throw()
{
	Close();
}

bool AVRController::Open(const string &device)
{
	// TODO: Add a new parameter, a function/callback with no parameters, that
	// blocks until the Arduino has finished resetting itself. The function
	// could, for example, wait for the Arduino to pull a GPIO pin low.
	Close();

	cout << "AVRController::Open - Opening port " << device << endl;

	// Open the port, while guarding against other open/close calls
	boost::mutex::scoped_lock portLock(m_portMutex);
	try
	{
		m_port.open(device.c_str());
		if (!m_port.is_open())
			return false;

		typedef boost::asio::serial_port_base asio_serial;
		m_port.set_option(asio_serial::baud_rate(115200));
		m_port.set_option(asio_serial::character_size(8));
		m_port.set_option(asio_serial::stop_bits(asio_serial::stop_bits::one));
		m_port.set_option(asio_serial::parity(asio_serial::parity::none));
		m_port.set_option(asio_serial::flow_control(asio_serial::flow_control::none));

		// Wait for the Arduino to reset itself
		cout << "AVRController::Open - Port opened, sleeping..." << endl;
		//boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
		boost::asio::deadline_timer timer(m_io, boost::posix_time::milliseconds(2000));
		timer.wait();

		// m_ioThread is the thread in which our IO callbacks are executed
		cout << "AVRController::Open - Creating write thread" << endl;
		m_bRunning = true;
		boost::thread temp(boost::bind(&AVRController::WriteThreadRun, this));
		m_writeThread.swap(temp);

		InstallAsyncRead();

		// Create the read thread after InstallAsyncRead() so that when the io_service is
		// started via m_io.run(), it will have something to do and won't return immediately
		boost::thread temp2(boost::bind(&AVRController::ReadThreadRun, this));
		m_readThread.swap(temp2);
	}
	catch (const boost::system::error_code &ec)
	{
		cerr << "AVRController::Open - Error opening " << device << ": " << ec.message() << endl;
		return false;
	}
	catch (...)
	{
		cerr << "AVRController::Open - Unspecified error" << endl;
		return false;
	}

	m_deviceName = device;
	return true;
}

void AVRController::Close() throw()
{
	try
	{
		// Kill the write thread first
		m_bRunning = false;
		m_writeQueueCondition.notify_one();
		m_writeThread.join();

		m_deviceName = "";
		boost::mutex::scoped_lock portLock(m_portMutex);
		if (m_port.is_open())
		{
			m_port.cancel();
			m_port.close();
		}

		m_readThread.join();

		m_io.reset();
	}
	catch (const boost::system::error_code &ec)
	{
		cerr << "AVRController::Close - Boost system error: " << ec.message() << endl;
	}
	catch (...)
	{
		cerr << "AVRController::Close - Unspecified error" << endl;
	}
}

bool AVRController::IsOpen()
{
	boost::mutex::scoped_lock portLock(m_portMutex);
	return m_port.is_open();
}

void AVRController::Send(const std::string &msg)
{
	if (msg.length() < 3)
		return;

	boost::mutex::scoped_lock writeQueueLock(m_writeQueueMutex);
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

	// Hand the string off to the heap so that if we time out, we can exit without
	// crashing in ReadCallback() (same goes for the condition variable).
	boost::shared_ptr<string> strResponse(new string);
	boost::shared_ptr<boost::condition> responseCondition(new boost::condition);

	{
		boost::mutex::scoped_lock responseLock(m_responseMutex);
		m_responseHandlers.push_back(ResponseHandler_t(fsmId, strResponse, responseCondition));
	}

	// Send the message after installing the response handler to avoid dropping responses
	Send(msg);

	// Wait on a private mutex to avoid deadlock
	boost::mutex privateMutex;
	boost::mutex::scoped_lock privateLock(privateMutex);
	boost::system_time const endtime = boost::get_system_time() + boost::posix_time::milliseconds(timeout);

	if (responseCondition->timed_wait(privateLock, endtime) && strResponse->length())
	{
		response = *strResponse;
		return true;
	}
	return false;
}

bool AVRController::Receive(unsigned int fsmId, string &msg, unsigned long timeout)
{
	// Hand the string off to the heap so that if we time out, we can exit without
	// crashing in ReadCallback() (same goes for the condition variable).
	boost::shared_ptr<string> strResponse(new string);
	boost::shared_ptr<boost::condition> responseCondition(new boost::condition);

	{
		boost::mutex::scoped_lock responseLock(m_responseMutex);
		m_responseHandlers.push_back(ResponseHandler_t(fsmId, strResponse, responseCondition));
	}

	// Wait on a private mutex to avoid deadlock
	boost::mutex privateMutex;
	boost::mutex::scoped_lock privateLock(privateMutex);
	boost::system_time const endtime = boost::get_system_time() + boost::posix_time::milliseconds(timeout);

	if (responseCondition->timed_wait(privateLock, endtime) && strResponse->length())
	{
		msg = *strResponse;
		return true;
	}
	return false;
}

void AVRController::WriteThreadRun()
{
	while (true)
	{
		string msg;

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
				return;
			}
			msg = m_writeQueue.front();
			m_writeQueue.erase(m_writeQueue.begin());
			// Don't need the lock for the rest of the while loop
		}

		{
			// Now lock the port mutex
			boost::mutex::scoped_lock portLock(m_portMutex);
			// Cancel the async read to free up the serial port. When the port
			// mutex is released, the async read will be re-installed automatically
			// by ReadCallback()
			m_port.cancel();
			// Write the data synchronously
			boost::asio::write(m_port, boost::asio::buffer(msg.c_str(), msg.length()));
		}
	}
}

void AVRController::ReadThreadRun()
{
	while (m_bRunning)
	{
		m_io.run();
		m_io.reset();
		// In case run() called with no work to do (shouldn't happen)
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}
}

// Lock the port mutex before calling InstallAsyncRead()
void AVRController::InstallAsyncRead()
{
	m_port.async_read_some(boost::asio::buffer(m_message.GetNextReadBuffer(), m_message.GetNextReadLength()),
		boost::bind(&AVRController::ReadCallback, this, boost::asio::placeholders::error,
		                                                boost::asio::placeholders::bytes_transferred));
}

void AVRController::ReadCallback(const boost::system::error_code &error, size_t bytes_transferred)
{
	// We don't care if async_read_some() was interrupted, try to use the data anyway
	m_message.Advance(bytes_transferred);
	if (m_message.IsFinished())
	{
		// Process completed message
		string msg = m_message.GetMessage();
		unsigned char fsmId = msg[2];

		boost::mutex::scoped_lock responseLock(m_responseMutex);

		// Iterate over our response handlers, notify all handlers waiting on
		// the FSM that this message belongs to
		vector<ResponseHandler_t>::iterator it = m_responseHandlers.begin();
		while (it != m_responseHandlers.end())
		{
			if (fsmId == it->get<0>())
			{
				it->get<1>()->assign(msg);
				it->get<2>()->notify_one();
				// Once it gets the message, remove the handler
				it = m_responseHandlers.erase(it);
			}
			else
			{
				it++;
			}
		}
		m_message.Reset();
	}

	// Don't re-install the async read if we are exiting
	if (m_bRunning)
	{
		// Grabbing the port mutex here means waiting until writing is finished
		boost::mutex::scoped_lock portLock(m_portMutex);
		InstallAsyncRead();
	}
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
	// Clear internal data
	delete[] m_nextBuffer;
	m_msg.clear();
	m_msgLength = 0;

	// New state: expect message length as 2 bytes
	m_readState = WaitingForLength;
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
			// Only first byte was read, store it...
			m_msg.push_back(m_nextBuffer[0]);
			delete[] m_nextBuffer;

			// ..and allocate a buffer for its friend
			m_readState = WaitingForLengthPt2;
			m_nextBufferLength = 1;
			m_nextBuffer = new unsigned char[m_nextBufferLength];
		}
		else
		{
			// Got our message length
			m_msg.push_back(m_nextBuffer[0]);
			m_msg.push_back(m_nextBuffer[1]);
			// Use native endian to recover the length
			m_msgLength = *reinterpret_cast<uint16_t*>(m_nextBuffer);
			delete[] m_nextBuffer;

			// Create a buffer for the rest of the message
			m_readState = WaitingForMessage;
			m_msg.reserve(m_msgLength);
			m_nextBufferLength = m_msgLength - 2;
			m_nextBuffer = new unsigned char[m_nextBufferLength];
		}
		break;

	case WaitingForLengthPt2:
		// Got the second half of our message length
		m_msg.push_back(m_nextBuffer[0]);
		// Use native endian to recover the length
		m_msgLength = *reinterpret_cast<const uint16_t*>(m_msg.c_str());
		delete[] m_nextBuffer;

		// Create a buffer for the rest of the message
		m_readState = WaitingForMessage;
		m_msg.reserve(m_msgLength);
		m_nextBufferLength = m_msgLength - m_msg.length();
		m_nextBuffer = new unsigned char[m_nextBufferLength];
		break;

	case WaitingForMessage:
		// Append the incoming data to our message
		for (size_t i = 0; i < bytes; i++)
			m_msg.push_back(m_nextBuffer[i]);
		delete[] m_nextBuffer;

		// Check to see if we need more data
		if (m_msgLength == m_msg.length())
		{
			m_readState = Finished;
			m_nextBufferLength = 0;
			m_nextBuffer = NULL;
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
