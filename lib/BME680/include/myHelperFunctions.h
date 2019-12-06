/*
 * myHelperFunctions.h
 *
 * Created: 16.11.2019 18:36:00
 *  Author: Michael
 */ 
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MYHELPERFUNCTIONS_H_
#define MYHELPERFUNCTIONS_H_

#include <avr/io.h> // required for uint_t types

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

//void ausbin8(uint8_t wert);

char* float2str(float floatValue);
char* uint82str(uint8_t uint8Value);
char* uint162str(uint16_t uint16Value);
char* uint322str(uint32_t uint32Value);

// helper functions
float   signedMag12ToFloat(uint16_t val);
float   int12ToFloat(uint16_t val);

#endif /* MYHELPERFUNCTIONS_H_ */

#ifdef __cplusplus
}
#endif