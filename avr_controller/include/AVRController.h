/*
 * Copyright (C) 2112 Garrett Brown <gbruin@ucla.edu>
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

#ifndef AVRCONTROLLER_H_
#define AVRCONTROLLER_H_

#include <boost/asio.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/tuple/tuple.hpp>
#include <limits>
#include <string>
#include <vector>

class AVRController
{
public:
	AVRController();
	~AVRController() throw();

	/**
	 * Open the port and connect to the Arduino.
	 *
	 * Examples:
	 * Linux: avr_controller.Open("/dev/ttyACM0");
	 * Win32: avr_controller.Open("COM3");
	 */
	bool Open(const std::string &device);

	/**
	 * Disconnect from the Arduino and close the serial port.
	 */
	void Close() throw();

	/**
	 * Reset the Arduino and unload all FSMs except for those loaded at the
	 * Arduino's boot time.
	 */
	bool Reset() { return Open(m_deviceName); }

	/**
	 * Returns true if the serial port is open and ready for action.
	 */
	bool IsOpen();

	/**
	 * Send a message to the AVR. Messages are represented by Pascal-style
	 * strings, where the first two bytes are the length of the message,
	 * including those two bytes. (They are little endian, as this is the same
	 * endianness of ARM, x86 and AVR.) The following character is the FSM ID
	 * that the message is intended for, followed by the FSM's required
	 * parameters.
	 */
	void Send(const std::string &msg);

	/**
	 * Send a message and wait for a response. Returns false if the query times
	 * out.
	 */
	bool Query(const std::string &msg, std::string &response, unsigned long timeout = DEFAULT_TIMEOUT);

	/**
	 * Block until a message from the specified FSM arrives on the serial port.
	 */
	bool Receive(unsigned int fsmId, std::string &msg, unsigned long timeout = DEFAULT_TIMEOUT);

	static const int DEFAULT_TIMEOUT = 1000; // ms

private:
	void WriteThreadRun();

	void InstallAsyncRead();

	void ReadCallback(const boost::system::error_code& error, size_t bytes_transferred);

private:
	/**
	 * Set the DTR bit on the serial port to the desired level (on or off).
	 *
	 * Untested.
	 */
	bool SetDTR(bool level);

	// Device name: only used for Reset()
	std::string               m_deviceName;

	// The I/O service talks to the serial device
	boost::asio::io_service   m_io;
	boost::asio::serial_port  m_port;
	boost::mutex              m_portMutex;

	boost::thread             m_writeThread;
	std::vector<std::string>  m_writeQueue;
	boost::mutex              m_writeQueueMutex;
	boost::condition          m_writeQueueCondition;
	bool                      m_bRunning;

	class Message
	{
	public:
		Message() : m_nextBuffer(NULL) { Reset(); }
		~Message() { delete[] m_nextBuffer; }
		unsigned char *GetNextReadBuffer() const { return m_nextBuffer; }
		size_t GetNextReadLength() const { return m_nextBufferLength; }
		bool IsFinished() const { return m_readState == Finished; }
		const std::string &GetMessage() const { return m_msg; }
		void Reset();
		void Advance(size_t bytes);

	private:
		std::string    m_msg;
		size_t         m_msgLength;
		unsigned char *m_nextBuffer;
		size_t         m_nextBufferLength;
		enum ReadState
		{
			WaitingForLength,
			WaitingForLengthPt2,
			WaitingForMessage,
			Finished
		} m_readState;
	};

	Message m_message;
};

#endif /* AVRCONTROLLER_H_ */
