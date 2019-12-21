/*
 * myGlobalDefines.c
 *
 * Created: 06.12.2019 18:36:00
 *  Author: Michael
 */ 

#include "myGlobalDefines.h"

#include <stdio.h>
#include <stdlib.h>


// helper function to compare two strings.
unsigned char cmpString(volatile char * string1, char * string2)
{
    while (1)
    {
        if ( (*string1 == 0) && (*string2 == 0) ) return 1;
        if (*string1 != *string2) return 0;
        string1++; string2++;
    }
}

// www.mikrocontroller.net/articles/FAQ#Aktivieren_der_Floating_Point_Version_von_sprintf_beim_WinAVR_mit_AVR-Studio
char* float2str(float floatValue)
{
	static char retnum[20];       // Enough for 20 digits
	sprintf(retnum,"%d.%02d", (int)floatValue , (int) (100*((floatValue)-(int)floatValue)));
	return retnum;
}

char* uint82str(uint8_t uint8Value)
{
	static char buffer[1];
	itoa(uint8Value, buffer, 10);
	return buffer;
}

char* uint162str(uint16_t uint16Value)
{
	static char buffer[2];
	itoa(uint16Value, buffer, 10);
	return buffer;
}

char* int162str(int16_t int16Value)
{
	static char buffer[6];
	itoa(int16Value, buffer, 10);
	return buffer;
}

char* uint322str(uint32_t uint32Value)
{
	static char buffer[4];
	itoa(uint32Value, buffer, 10);
	return buffer;
}


// helper functions
// convert a 12-bit signed magnitude value to a floating point number
// According to data sheet AMG8833: 12-bit Thermistor resolution is indicated as code (sign) + absolute value
float signedMag12ToFloat(uint16_t val)
{
    //take first 11 bits as absolute val by applying the 11-bit mask 0x7FF
    uint16_t absVal = (val & 0x7FF);

    // if 12th bit (0x800) is 0 (= minus) then return negative absolute value, otherwise just return positive (absolute) value
    return (val & 0x800) ? 0 - (float) absVal : (float) absVal;
}


// convert a 12-bit integer two's complement value to a floating point number
// According to data sheet AMG8833: 12-bit Pixel resolution is indicated as two's complement form (Zweierkomplement)
float int12ToFloat(uint16_t val)
{
    // shift to left so that sign bit of 12 bit integer number is placed on sign bit of 16 bit signed integer number
    int16_t sVal = (val << 4);

    // shift back the signed number. Call of return converts to float
    return sVal >> 4;
}