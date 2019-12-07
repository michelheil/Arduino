/*
 * myDS3231.c
 *
 * Created: 16.11.2019 10:50:07
 *  Author: Michael
 */ 
#include "myDS3231.h"

#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include "myTWI.h"

// initialise DS3231 by activating TWI on Arduino
void DS3231_init(void)
{
    TWI_init();
}

// get byte value from register Seconds
uint8_t DS3231_getSeconds(void) {
    uint8_t res;
    TWI_getRegisterBytes(DS3231_SLAVE_ADDRESS, DS3231_SECONDS_REG, 1, &res);
    return res;
}

// get byte value from register Minutes
uint8_t DS3231_getMinutes(void) {
    uint8_t res;
    TWI_getRegisterBytes(DS3231_SLAVE_ADDRESS, DS3231_MINUTES_REG, 1, &res);
    return res;
}

// get byte value from register Hours
uint8_t DS3231_getHours(void) {
    uint8_t res;
    TWI_getRegisterBytes(DS3231_SLAVE_ADDRESS, DS3231_HOURS_REG, 1, &res);
    return res;
}

// get 3 byte values starting from register Seconds
void DS3231_getTime(uint8_t * resPointer) {
    TWI_getRegisterBytes(DS3231_SLAVE_ADDRESS, DS3231_SECONDS_REG, 3, resPointer);
}

// transform raw byte values of into string HH:mm:ss
void DS3231_getTimeString(char * resPointer) {
    uint8_t rawTime[3];
    DS3231_getTime(&rawTime[0]);

    itoa((rawTime[2] >> 4), resPointer++, 10);
    itoa((rawTime[2] & 0x0F), resPointer++, 10);
    *resPointer++ = ':';
    itoa((rawTime[1] >> 4), resPointer++, 10);
    itoa((rawTime[1] & 0x0F), resPointer++, 10); 
    *resPointer++ = ':';
    itoa((rawTime[0] >> 4), resPointer++, 10);
    itoa((rawTime[0] & 0x0F), resPointer++, 10);
    *resPointer = '\0';
}

// get 3 byte values starting from register Date
void DS3231_getDMY(uint8_t * resPointer) {
    TWI_getRegisterBytes(DS3231_SLAVE_ADDRESS, DS3231_DATE_REG, 3, resPointer);
}

// transform raw byte values of into string yyyy-MM-dd
void DS3231_getDMYString(char * resPointer) {
    uint8_t rawDMY[3];
    DS3231_getDMY(&rawDMY[0]);

    *resPointer++ = '2';
    *resPointer++ = '0';
    itoa((rawDMY[2] >> 4), resPointer++, 10);
    itoa((rawDMY[2] & 0x0F), resPointer++, 10);
    *resPointer++ = '-';
    itoa((rawDMY[1] >> 4), resPointer++, 10);
    itoa((rawDMY[1] & 0x0F), resPointer++, 10); 
    *resPointer++ = '-';
    itoa((rawDMY[0] >> 4), resPointer++, 10);
    itoa((rawDMY[0] & 0x0F), resPointer++, 10); 
    *resPointer = '\0';
}

// get 7 byte values starting from register Seconds
void DS3231_getTimestamp(uint8_t * resPointer) {
    TWI_getRegisterBytes(DS3231_SLAVE_ADDRESS, DS3231_SECONDS_REG, 7, resPointer);
}

// transform raw byte values into string
void DS3231_getTimestampString(char * resPointer) {

    uint8_t rawTimestamp[7];
    DS3231_getTimestamp(&rawTimestamp[0]);

    *resPointer++ = '2';
    *resPointer++ = '0';
    itoa((rawTimestamp[6] >> 4), resPointer++, 10);
    itoa((rawTimestamp[6] & 0x0F), resPointer++, 10);
    *resPointer++ = '-';
    itoa((rawTimestamp[5] >> 4), resPointer++, 10);
    itoa((rawTimestamp[5] & 0x0F), resPointer++, 10); 
    *resPointer++ = '-';
    itoa((rawTimestamp[4] >> 4), resPointer++, 10);
    itoa((rawTimestamp[4] & 0x0F), resPointer++, 10); 
    *resPointer++ = '\''; 
    *resPointer++ = 'T'; 
    *resPointer++ = '\''; 
    itoa((rawTimestamp[2] >> 4), resPointer++, 10);
    itoa((rawTimestamp[2] & 0x0F), resPointer++, 10);
    *resPointer++ = ':';
    itoa((rawTimestamp[1] >> 4), resPointer++, 10);
    itoa((rawTimestamp[1] & 0x0F), resPointer++, 10);
    *resPointer++ = ':';
    itoa((rawTimestamp[0] >> 4), resPointer++, 10);
    itoa((rawTimestamp[0] & 0x0F), resPointer++, 10);
    *resPointer = '\0';
}

// get 3 byte values starting from register Day
void DS3231_getDayString(char * resPointer) {
    
    uint8_t rawDay;
    TWI_getRegisterBytes(DS3231_SLAVE_ADDRESS, DS3231_DAY_REG, 1, &rawDay);

    switch(rawDay)
    {
        case 1:
            strcpy(resPointer, "Sunday");
            break;
        case 2:
            strcpy(resPointer, "Monday");
            break;
        case 3:
            strcpy(resPointer, "Tuesday");
            break;
        case 4:
            strcpy(resPointer, "Wednesday");
            break;
        case 5:
            strcpy(resPointer, "Thursday");
            break;
        case 6:
            strcpy(resPointer, "Friday");
            break;
        case 7:
            strcpy(resPointer, "Saturday");
            break;
        default:
            LOG_error("DS3231_getDayString");
    }
}



uint8_t DS3231_combineRegisterBits(uint8_t rawData) {
    
    return (10*(rawData >> 4) + (rawData & 0x0F));

}