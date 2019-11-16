/*
 * myDS3231.h
 *
 * Created: 16.11.2019 10:47:24
 *  Author: Michael
 */ 


#ifndef MYDS3231_H_
#define MYDS3231_H_

#include "globalDefines.h"
#include <util/twi.h> // required for TW_READ and TW_WRITE

// DS3231 I2C Slave Address is 0x68
#define DS3231_SLAVE_ADDRESS            0x68
#define DS3231_SLA_W                    ((DS3231_SLAVE_ADDRESS << 1) | TW_WRITE) // shifted by one as the address is stored in the higher 7-bits
#define DS3231_SLA_R                    ((DS3231_SLAVE_ADDRESS << 1) | TW_READ)  // TW_WRITE (=0) and TW_READ (=1) are defined in util/twi.h

#define DS3231_SECONDS_REG              0x00
#define DS3231_MINUTES_REG              0x01
#define DS3231_HOURS_REG                0x02
#define DS3231_DAY_REG                  0x03
#define DS3231_DATE_REG                 0x04
#define DS3231_MONTH_CENTURY_REG        0x05
#define DS3231_YEAR_REG                 0x06
#define DS3231_ALARM_1_SECONDS_REG      0x07
#define DS3231_ALARM_1_MINUTES_REG      0x08
#define DS3231_ALARM_1_HOURS_REG        0x09
#define DS3231_ALARM_1_DAY_DATE_REG     0x0A
#define DS3231_ALARM_2_MINUTES_REG      0x0B
#define DS3231_ALARM_2_HOURS_REG        0x0C
#define DS3231_ALARM_2_DAY_DATE_REG     0x0D

///////////////////////////////////////////////////////
// getter functions

/*
 * Request current seconds from Adafruit DS3231
 * 
 * return: raw Bits of the requested DS3231 register 
 * (apply combineRegisterrBits helper function for actual value)
 */
uint8_t DS3231_getSeconds(void);

/*
 * Request current minutes from Adafruit DS3231
 * 
 * return: raw Bits of the requested DS3231 register 
 * (apply combineRegisterrBits helper function for actual value)
 */
uint8_t DS3231_getMinutes(void);

/*
 * Request current hours from Adafruit DS3231
 * 
 * return: raw Bits of the requested DS3231 register 
 * (apply combineRegisterrBits helper function for actual value)
 */
uint8_t DS3231_getHours(void);

/*
 * Request current Seconds, Minutes, and Hours from Adafruit DS3231
 * 
 * resPointer: storage for raw Bits of the requested DS3231 registers
 * (apply combineRegisterrBits helper function for actual value)
 */
void DS3231_getTime(uint8_t * resPointer);

/*
 * Request current Date, Month, and Year from Adafruit DS3231
 * 
 * resPointer: storage for raw Bits of the requested DS3231 registers
 * (apply combineRegisterrBits helper function for actual value)
 */
void DS3231_getDMY(uint8_t * resPointer);

/*
 * Request current Seconds, Minutes, Hours, Day, Date, Month, and Year from Adafruit DS3231
 * 
 * resPointer: storage for raw Bits of the requested DS3231 registers
 * (apply combineRegisterrBits helper function for actual value)
 */
void DS3231_getTimestamp(uint8_t * resPointer);

/*
 * Gets timestamp from DS3231 in format yyyy-MM-dd'T'HH:mm:ss
 * 
 * resPointer: storage for timestamp string
 * 
 * usage: 
 * char timestamp[22];
 * DS3231_getTimeStampString(&timestamp[0]);
 * USART_writeStringLn(&timestamp[0]);
 */
void DS3231_getTimeStampString(char * resPointer);

/*
 * Gets day from DS3231 as String
 * 
 * resPointer: storage for day string
 * 
 * char day[10];
 * DS3231_getDayString(&day[0]);
 * USART_writeStringLn(&day[0]);
 */  
void DS3231_getDayString(char * resPointer);

/*
 * Combines the bits of the DS3231 register bytes into decimal numbers
 * 
 * rawData: raw register byte value
 * 
 * return: decimal number
 */
uint8_t DS3231_combineRegisterBits(uint8_t rawData);


#endif /* MYDS3231_H_ */