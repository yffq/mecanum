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
#pragma once

#include "GPIO.h"
#include "BeagleBoardAddressBook.h"

class Thumbwheel
{
public:
	Thumbwheel() : m_pin1(THUMBWHEEL1), m_pin2(THUMBWHEEL2), m_pin4(THUMBWHEEL4) { }
	~Thumbwheel() { }

	/**
	 * Open the GPIO pins associated with the thumbwheel.
	 */
	bool Open();

	/**
	 * Return the decoded value currently displayed by the thumbwheel.
	 */
	unsigned int GetValue();

	/**
	 * Block until a new value is available. An additional settling time is
	 * applied to better avoid returning transitory values. Returns false if the
	 * timeout occurs before a new value is detected.
	 */
	bool Poll(unsigned long timeout, unsigned int &value);

private:
	/**
	 * This object is noncopyable.
	 */
	Thumbwheel(const Thumbwheel &other);
	Thumbwheel& operator=(const Thumbwheel &rhs);

	GPIO m_pin1;
	GPIO m_pin2;
	GPIO m_pin4;
};
