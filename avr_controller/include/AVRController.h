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

#include "AVRInstance.h"
#include "FSMContainer.h"
#include "Message.h"

#include <vector>
#include <boost/asio.hpp>
#include <stddef.h> // for size_t

class AVRController
{
public:
	AVRController();
	~AVRController();

	/**
	 * Open the port and connect to the Arduino.
	 *
	 * Examples:
	 * Linux: Open("/dev/ttyACM0");
	 * Win32: Open("COM3");
	 */
	bool Open(const std::string &device);

	/**
	 * Disconnect from the Arduino and close the serial port.
	 */
	void Close();

	/**
	 * Reset the Arduino and unload all FSMs except for those loaded at the
	 * Arduino's boot time.
	 */
	bool Reset();

	/**
	 * Returns true if the serial port is open and ready for action.
	 */
	bool IsOpen();

	void Send(AVR::Message::Command *msg);

	void Receive(AVR::Message::Response *msg);




	/**
	 * Turn a pin on or off.
	 */
	void DigitalWrite(unsigned char pin, bool value);

	/**
	 * Read the state of a pin
	 */
	bool DigitalRead(unsigned char pin);

	/**
	 * Subscribe to state changes on a pin.
	 */
	bool DigitalSubscribe(unsigned char pin, bool callback);

	/**
	 * Blink a pin at the desired frequency.
	 */
	void Blink(unsigned char pin, float frequency /* Hz */);

	/**
	 * Control the value of a PWM pin. The range of values varies by pin,
	 * and will typically be either 0-255 (1 byte) or 0-65535 (2 bytes).
	 */
	void PWMWrite(unsigned char pin, unsigned int value);

	/**
	 * Read the value of an analog input pin.
	 */
	float AnalogRead(unsigned char pin);

	/**
	 * Invoke callback at the given frequency with the current value of the given
	 * pin.
	 */
	void AnalogSubscribe(unsigned char pin, float frequency /* Hz */, bool callback);

	/**
	 * Remove a digital or analog subscriber from a pin.
	 */
	void RemoveSubscriber(unsigned char pin);



public: // protected

	/**
	 * Load the FSM onto the Arduino and add it to v_fsm. Performs conflict
	 * resolution, so if, for example, and the FSM's pin is already in use,
	 * this function will unload the offending FSM before loading the new one.
	 */
	void LoadFSM(const FSMContainer &fsm);

	/**
	 * Reset the Arduino and load all FSMs from v_fsm onto the Arduino.
	 *
	 * TODO: Exception instead of bool return
	 */
	bool ResetAndLoadAll();

	/**
	 * Set the DTR bit on the serial port to the desired level (on or off).
	 *
	 * Untested.
	 */
	bool SetDTR(bool level);


private:
	AVRInstance m_instance;

	// Device name: only used for Reset()
	std::string              m_deviceName;
	// The I/O service talks to the serial device
	boost::asio::io_service  m_io;
	boost::asio::serial_port m_port;
	// A deadline_timer object allows us to set a serial timeout
	//boost::asio::deadline_timer m_timeout;

	//std::vector<int> responseQueue;
};




#endif /* AVRCONTROLLER_H_ */
