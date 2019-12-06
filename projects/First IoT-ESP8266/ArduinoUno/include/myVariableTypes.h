/*
    Defines global struct types used in my personal Arudino Uno projects
    Copyright (C) 2019  Michael Heil

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Created: 22.09.2019 15:36:00
 *  Author: Michael
 */ 


#ifndef MYVARIABLETYPES_H_
#define MYVARIABLETYPES_H_

#include <avr/io.h> // required for uint types

struct pairOfTwoUint16 {
	uint16_t sensorLowerBound;
	uint16_t sensorUpperBound;
};


#endif /* MYVARIABLETYPES_H_ */