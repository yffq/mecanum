/*
 *  	  Copyright 2011 Garrett Brown
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

#include "BeagleBoard.h"

using namespace NBeagleBoard;

unsigned int CExpansionPin::Demux(unsigned int pin)
{
	unsigned int expansionHeader[] =
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
