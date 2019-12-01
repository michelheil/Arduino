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
 * Created: 22.09.2019 15:19:57
 *  Author: Michael
 */ 


#ifndef GLOBALDEFINES_H_
#define GLOBALDEFINES_H_

#define F_CPU 16000000L

#define GLOBAL_LOG_LEVEL 3 // {ALL = 1, DEBUG = 2, INFO = 3, WARN = 4, ERROR = 5, FATAL = 6, OFF = 7};

// helper functions (macros) to support bit operations
#define sbi(PORT, bit) (PORT |= (1 << bit))  // set bit in PORT
#define cbi(PORT, bit) (PORT &= ~(1 << bit)) // clear bit in PORT
#define tgl(PORT, bit) (PORT ^= (1 << bit))  // switch bit in PORT

#define CONCATENATE_BYTES(msb, lsb) (((uint16_t)msb << 8) | (uint16_t)lsb) // combine msb and lsb

#endif /* GLOBALDEFINES_H_ */