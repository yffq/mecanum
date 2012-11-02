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
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <vector>

class AVRController : public boost::noncopyable
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

	static const int DEFAULT_TIMEOUT = 1000; // ms

	/**
	 * Send a message and wait for a response. Returns false if the query times
	 * out. If true, response is guaranteed to be a valid AVR string (2-byte
	 * length in little endian, followed by FSM ID, then message payload, if any).
	 */
	bool Query(const std::string &msg, std::string &response, unsigned long timeout = DEFAULT_TIMEOUT);

	/**
	 * Block until a message from the specified FSM arrives on the serial port.
	 * Returns false if the response times out. If true, response is guaranteed
	 * to be a valid AVR string.
	 */
	bool Receive(unsigned int fsmId, std::string &response, unsigned long timeout = DEFAULT_TIMEOUT);

	/**
	 * Interface with the MecanumMaster program running on the AVR.
	 */
	bool ListFiniteStateMachines(std::vector<std::string> &fsmv);
	void DestroyFiniteStateMachine(const std::string &fsm);
	void CreateFiniteStateMachine(const std::string &fsm);

	static uint16_t GetMsgLength(const void *bytes) { return *reinterpret_cast<const uint16_t*>(bytes); }

private:

	/**
	 * Performs the work for Query() and Receive().
	 */
	bool QueryInternal(unsigned int fsmId, bool sendMsg, const std::string &msg, std::string &response, unsigned long timeout);

	/**
	 * Writing to the serial port occurs in this thread. When no data is queued,
	 * it idles.
	 */
	void WriteThreadRun();

	/**
	 * This thread exists solely to give ReadCallback() a thread to run in. The
	 * io_service takes care off the details; we just provide the thread.
	 */
	void ReadThreadRun();

	/**
	 * Installs the async_read_some() callback onto the serial port. The caller
	 * must lock m_portMutex before entering this function. This should only be
	 * called from Open() and the async read callback (ReadCallback()) to avoid
	 * putting multiple callbacks on the same port.
	 */
	void InstallAsyncRead();

	/**
	 * Handles data flying off the serial port. On exit, this call InstallAsyncRead()
	 * to re-install itself onto the serial port.
	 */
	void ReadCallback(const boost::system::error_code& error, size_t bytes_transferred);

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
	volatile bool             m_bRunning;

	boost::thread             m_readThread;

	// Our response handler uses shared pointers to delegate memory ownership
	// from the stack to the heap. This lets us bail out early if the response
	// times out, without the repercussions of invoking an orphaned handler.
	typedef boost::tuple<
		int,                                /* fsmId */
		boost::shared_ptr<std::string>,     /* response, empty on error */
		boost::shared_ptr<boost::condition> /* wait condition */
	> ResponseHandler_t;
	std::vector<ResponseHandler_t> m_responseHandlers;
	boost::mutex                   m_responseMutex;

	/**
	 * Use a finite state machine for message composition. FSMs are best for
	 * this task because the number of expected characters is a function of the
	 * message length and its content (i.e. first word is the entire message
	 * length). Buffer memory management is also delegated to this class, as
	 * boost doesn't manage its buffer's memory, and static arrays are
	 * cumbersome and not well suited to this task.
	 *
	 * The general strategy is this: tell boost to read into the buffer
	 * [GetNextReadBuffer(), GetNextReadLength()]. The length of this buffer is
	 * determined by the internal state. After reading, boost will tell us how
	 * many bytes were read, which we forward to the class using Advance().
	 * This will update the internal state and allocate new buffers as
	 * necessary. Finally, IsFinished() will return true when a complete
	 * message has been retrieved, and GetMessage() will yield that message.
	 * Reset() resets the state, clears the internal data, and sets up a buffer
	 * for the next call to GetNextReadBuffer().
	 *
	 * Empirically, the first buffer is two bytes, which yields the length of
	 * the entire message. The size of the next buffer is that length (minus
	 * two), which contains the main payload. Thus, most reads use two buffers.
	 */
	class Message : public boost::noncopyable
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

		static const unsigned int MAX_LENGTH = 512;

	private:
		std::string    m_msg;
		uint16_t       m_msgLength;
		unsigned char *m_nextBuffer;
		size_t         m_nextBufferLength;
		enum ReadState
		{
			WaitingForLength,
			WaitingForLengthPt2,
			WaitingForMessage,
			Finished
		} m_readState;
	} m_message;
};

#endif /* AVRCONTROLLER_H_ */
