/*
 *  	  Copyright (C) 2011 Garrett Brown
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "BBExpansionPin.h"

class Thumbwheel
{
public:
	Thumbwheel() : m_pin1(19), m_pin2(6), m_pin4(7) { }
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

	BBExpansionPin m_pin1;
	BBExpansionPin m_pin2;
	BBExpansionPin m_pin4;
};
