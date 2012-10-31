/*
 *  	  Copyright 2011 Garrett Brown
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

#include "GPIO.h"

class BBExpansionPin : public GPIO
{
public:
	BBExpansionPin(unsigned int pin) : GPIO(Demux(pin)) { }

private:
	/**
	 * Demux an expansion pin into its GPIO mode.
	 *
	 * Source: BeagleBoard-xM System Reference Manual, Rev C.1.0, p108
	 *
	 * \param pin The expansion header pin between 3 and 24.
	 * \return The number of the GPIO pin multiplexed into the expansion header pin.
	 */
	unsigned int Demux(unsigned int pin);
};
