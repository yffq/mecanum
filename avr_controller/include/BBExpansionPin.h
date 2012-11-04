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

class BBExpansionPin : public GPIO
{
public:
	BBExpansionPin(unsigned int pin) : GPIO(Translate(pin)) { }

private:
	/**
	 * Demux an expansion pin into its GPIO mode.
	 *
	 * Source: BeagleBoard-xM System Reference Manual, Rev C.1.0, p108
	 *
	 * \param pin The expansion header pin between 3 and 24.
	 * \return The number of the GPIO pin multiplexed into the expansion header pin.
	 */
	unsigned int Translate(unsigned int pin);
};
