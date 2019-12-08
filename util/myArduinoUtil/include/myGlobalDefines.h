/*
    Defines global Variables and inline Functions used in my Arduino projects
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
 * Created: 06.12.2019 18:19:57
 *  Author: Michael
 */ 


#ifndef GLOBALDEFINES_H_
#define GLOBALDEFINES_H_

#include <avr/io.h> // required for uint_t types

#define F_CPU 16000000L

// helper functions (macros) to support bit operations
#define sbi(PORT, bit) (PORT |= (1 << bit))  // set bit in PORT
#define cbi(PORT, bit) (PORT &= ~(1 << bit)) // clear bit in PORT
#define tgl(PORT, bit) (PORT ^= (1 << bit))  // switch bit in PORT

#define CONCATENATE_BYTES(msb, lsb) (((uint16_t)msb << 8) | (uint16_t)lsb) // combine msb and lsb

/*
 * define function to compare two Strings
 * 
 * string1: pointer to unsigned char of first string
 * string2: pointer to unsigned char of second string
 *
 * returns: 1 for string matching, 0 else
 *
 * Example: cmpString(&usartStr[0], &compareStr[0])
 */
uint8_t cmpString(volatile char * string1, char * string2);

char* float2str(float floatValue);
char* uint82str(uint8_t uint8Value);
char* uint162str(uint16_t uint16Value);
char* uint322str(uint32_t uint32Value);

// helper functions
float signedMag12ToFloat(uint16_t val);
float int12ToFloat(uint16_t val);

// variable structs
struct pairOfTwoUint16 {
	uint16_t sensorLowerBound;
	uint16_t sensorUpperBound;
};

#endif /* GLOBALDEFINES_H_ */