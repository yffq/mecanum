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

AVRController::AVRController() : m_io(), m_port(m_io), writeBufferSize(0), readBufferPos(0)
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

	boost::lock_guard<boost::mutex> lock(port_mutex);
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

	//boost::this_thread::sleep(boost::posix_time::seconds(4000));
	boost::asio::deadline_timer timer(m_io, boost::posix_time::seconds(4));
	timer.wait();

	// See "A thread pool for executing arbitrary tasks"
	// http://think-async.com/Asio/Recipes

	// Give some work to the io_service before it is started. This stops its
	// run() function from exiting immediately.
	//boost::asio::io_service::work work(m_io);
	m_io.post(boost::bind(&AVRController::DoRead, this, 1));

	// io_thread is the thread in which our IO callbacks are executed
	boost::system::error_code ec; // Hack: the next line won't work without this:
	                              // "Invalid overload of 'boost::asio::io_service::run'"
	boost::thread temp_thread(boost::bind(&boost::asio::io_service::run, &m_io, ec));
	io_thread.swap(temp_thread);

	cout << "Querying FSMs..." << endl;

	// Initialize v_fsm with the list of FSMs currently running on the Arduino
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
	//boost::system::error_code ec;

	m_deviceName = "";
	m_instance.Clear();

	boost::lock_guard<boost::mutex> lock(port_mutex);
	if (m_port.is_open())
	{
		m_port.cancel();
		m_port.close();
	}

	io_thread.join();

	//m_io.reset(); // Does this remove m_port from m_io?
}

bool AVRController::Reset()
{
	return Open(m_deviceName);
}

bool AVRController::IsOpen()
{
	boost::lock_guard<boost::mutex> lock(port_mutex);
	return m_port.is_open();
}

void AVRController::Send(AVR::Message::Command *msg)
{
	{
		boost::lock_guard<boost::mutex> lock(write_mutex);
		const unsigned char *c = &msg->GetMessage()[0];
		writeQueue.insert(writeQueue.end(), c, c + msg->GetMessageSize());
	}
	m_io.post(boost::bind(&AVRController::DoWrite, this));
}

void AVRController::Receive(AVR::Message::Response *msg)
{
	boost::shared_ptr<boost::condition> wait_condition(new boost::condition);
	response_t response(msg, wait_condition);

	{
		boost::lock_guard<boost::mutex> lock(response_mutex);
		responseVec.push_back(response);
	}

	boost::mutex private_mutex;
	wait_condition->wait(private_mutex);
}

void AVRController::DoWrite()
{
	// If a write operation is already in progress, do nothing
	if (writeBuffer)
		return;

	// Lock the writeQueue
	boost::lock_guard<boost::mutex> lock(write_mutex);
	writeBufferSize = writeQueue.size();
	writeBuffer.reset(new char[writeBufferSize]);
	std::copy(writeQueue.begin(), writeQueue.end(), writeBuffer.get());
	writeQueue.clear();
	async_write(m_port, boost::asio::buffer(writeBuffer.get(), writeBufferSize),
			boost::bind(&AVRController::EndWrite, this, boost::asio::placeholders::error));
}

void AVRController::EndWrite(const boost::system::error_code &error)
{
	if (!error)
	{
		boost::lock_guard<boost::mutex> lock(write_mutex);
		// If there is more work to do, rev up the engines again
		if (!writeQueue.empty())
		{
			writeBufferSize = writeQueue.size();
			writeBuffer.reset(new char[writeBufferSize]);
			std::copy(writeQueue.begin(), writeQueue.end(), writeBuffer.get());
			writeQueue.clear();
			async_write(m_port, boost::asio::buffer(writeBuffer.get(), writeBufferSize),
					boost::bind(&AVRController::EndWrite, this, boost::asio::placeholders::error));
		}
		else
		{
			writeBuffer.reset();
			writeBufferSize = 0;
		}
	}
	else
	{
		//Close();
	}
}

void AVRController::DoRead(unsigned int count /* = 1 */)
{
	// Don't read past the end of the buffer
	if (count > READ_BUFFER_SIZE - readBufferPos)
		count = READ_BUFFER_SIZE - readBufferPos;

	m_port.async_read_some(boost::asio::buffer(readBuffer + readBufferPos, count),
			boost::bind(&AVRController::EndRead, this, boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}

void AVRController::EndRead(const boost::system::error_code& error, size_t bytesTransferred)
{
	if (!error && bytesTransferred)
	{
		// Adjust the target of our next read; consequently, readBufferPos is
		// also the size of used bytes in our readBuffer
		readBufferPos += bytesTransferred;

		// First byte is message length, make sure it's valid
		unsigned char msg_length = readBuffer[0];
		if (msg_length < 2)
		{
			// Invalid! Ignore data
			readBufferPos = 0;
			DoRead(1);
			return;
		}

		while (readBufferPos && readBufferPos >= readBuffer[0])
		{
			msg_length = readBuffer[0]; // Re-calculate our msg length
			// We've read in a full message, invoke callbacks
			OnReadMessage(TinyBuffer(readBuffer, msg_length));

			// If there's any data left over, shift it to the beginning
			for (int i = 0; i < (int)readBufferPos - msg_length; ++i)
				readBuffer[i] = readBuffer[i + msg_length];
			readBufferPos -= msg_length; // Next read happens here
		}

		if (readBufferPos) // assert !(readBufferPos >= readBuffer[0])
		{
			// Current message is incomplete, read in the rest. Desired size
			// is msg length minus current buffer size (a.k.a. readBufferPos).
			msg_length = readBuffer[0]; // Re-calculate our msg length
			if (msg_length < 2)
			{
				// Invalid! Ignore data
				readBufferPos = 0;
				DoRead(1);
				return;
			}
			else
			{
				DoRead(msg_length - readBufferPos);
				return;
			}
		}

		// assert !(readBufferPos)
		// Entire buffer was consumed, start over
		DoRead(1);
		return;
	}

	/*
	// Error can be true if serial port is closed, so don't re-close
	if (IsOpen())
		Close();
	*/
}

// There once was a TinyBuffer named tim
void AVRController::OnReadMessage(const TinyBuffer &tim)
{
	boost::lock_guard<boost::mutex> lock(response_mutex);

	for (std::vector<response_t>::iterator it = responseVec.begin(); it != responseVec.end(); ++it)
	{
		unsigned char fsm_id = tim[1];
		if (it->get<0>()->WaitForFSM() == fsm_id && it->get<0>()->OnReceive(tim + 2))
		{
			it->get<1>()->notify_one();
			responseVec.erase(it);
			break;
		}
	}

	// Notice: Received message with no registered responses
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
