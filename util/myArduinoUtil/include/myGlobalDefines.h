/** @mainpage myArduinoUtil
 * @brief This library contains all utilities used in my Arduino Uno projects
 * 
 *@section Lisence Lisence 
 * 
 * Copyright (C) 2019  Michael Heil @n
 * @n
 * This program is free software: you can redistribute it and/or modify @n
 * it under the terms of the GNU General Public License as published by @n
 * the Free Software Foundation, either version 3 of the License, or @n
 * (at your option) any later version. @n
 * @n
 * This program is distributed in the hope that it will be useful, @n
 * but WITHOUT ANY WARRANTY; without even the implied warranty of @n
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the @n
 * GNU General Public License for more details. @n
 * @n
 * You should have received a copy of the GNU General Public License @n
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. @n
 *
 *
 * @date 09.10.2019 18:41:08
 * @author Michael
 * @copyright GNU General Public License
 */ 

/**
 * @page myGlobalDefines_page myGlobalDefines.h
 * @brief This header file contains globally used variables and functions
 * 
 * @date 09.10.2019 18:41:08
 * @author Michael
 * @copyright GNU General Public License
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GLOBALDEFINES_H_
#define GLOBALDEFINES_H_

#include <avr/io.h> // required for uint_t types

#define F_CPU 16000000L /**< Define clock frequency of Arduino Uno as global setting */

// helper functions (macros) to support bit operations
#define sbi(PORT, bit) (PORT |= (1 << bit))  /**< set bit in PORT */
#define cbi(PORT, bit) (PORT &= ~(1 << bit)) /**< clear bit in PORT */
#define tgl(PORT, bit) (PORT ^= (1 << bit))  /**< switch bit in PORT */

#define SET_BIT_LEVEL(PORT, bit, level) ( ((level) == 0) ? (cbi(PORT, bit)) : (sbi(PORT, bit)) )

#define CONCATENATE_BYTES(msb, lsb) (((uint16_t)msb << 8) | (uint16_t)lsb) /**< combine msb and lsb */
#define CONCATENATE_INT16_BYTES(msb, lsb) (((int16_t)msb << 8) | (int16_t)lsb) /**< combine msb and lsb */

/**
 * @brief Define function to compare two Strings
 * 
 * Example: cmpString(&usartStr[0], &compareStr[0])
 * 
 * @param string1 pointer to unsigned char of first string
 * @param string2 pointer to unsigned char of second string
 *
 * @return 1 for string matching, 0 else
 */
uint8_t cmpString(volatile char * string1, char * string2);

/**
 * @brief Convert a float into a string
 * 
 * @param floatValue Value that is casted to a string
 * @return Start pointer of an char array (String)
 */
char* float2str(float floatValue);

/**
 * @brief Convert a uint8_t into a string
 * 
 * @param uint8Value Value that is casted to a string
 * @return Start pointer of an char array (String)
 */
char* uint82str(uint8_t uint8Value);

/**
 * @brief Convert a uint16_t into a string
 * 
 * @param uint16Value Value that is casted to a string
 * @return Start pointer of an char array (String)
 */
char* uint162str(uint16_t uint16Value);

/**
 * @brief Convert a int16_t into a string
 * 
 * @param int16Value Value that is casted to a string
 * @return Start pointer of an char array (String)
 */
char* int162str(int16_t int16Value);

/**
 * @brief Convert a uint32_t into a string
 * 
 * @param uint32Value Value that is casted to a string
 * @return Start pointer of an char array (String)
 */
char* uint322str(uint32_t uint32Value);

// helper functions
float signedMag12ToFloat(uint16_t val);
float int12ToFloat(uint16_t val);

/**
 * @brief Struct of two uint16_t variables 
 */
struct pairOfTwoUint16 {
	uint16_t sensorLowerBound;
	uint16_t sensorUpperBound;
};

/**
 * @brief Struct to store 3 int16_t values (such as 3-axis)
 */
struct xyzInt16Values {
  int16_t x;
  int16_t y;
  int16_t z;
};

/**
 * @brief can be used in TCS3200 for storing measured colors
 */
struct rgb {
  // stores values for each color (red, green, blue)
  int red;
  int green;
  int blue;

  // calibration factors for each color (red, green, blue)
  float redCalibFact;
  float greenCalibFact;
  float blueCalibFact;
};


#endif /* GLOBALDEFINES_H_ */

#ifdef __cplusplus
}
#endif