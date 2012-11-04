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

#include "BBExpansionPin.h"

unsigned int BBExpansionPin::Translate(unsigned int pin)
{
	const unsigned int expansionHeader[] =
	{
		/*  1 */  // VIO_1V8
		/*  2 */  // DC_5V
		/*  3 */  139,
		/*  4 */  144,
		/*  5 */  138,
		/*  6 */  146,
		/*  7 */  137,
		/*  8 */  143,
		/*  9 */  136,
		/* 10 */  145,
		/* 11 */  135,
		/* 12 */  158,
		/* 13 */  134,
		/* 14 */  162,
		/* 15 */  133,
		/* 16 */  161,
		/* 17 */  132,
		/* 18 */  159,
		/* 19 */  131,
		/* 20 */  156,
		/* 21 */  130,
		/* 22 */  157,
		/* 23 */  183,
		/* 24 */  168
		/* 25 */  // REGEN
		/* 26 */  // Nreset
		/* 27 */  // GND
		/* 28 */  // GND
	};
	return (3 <= pin && pin <= 24) ? expansionHeader[pin - 3] : 0;
}
