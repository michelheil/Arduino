/*
    Programm to communicate with DS3231 device
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
 *  Created: 25.01.2020 06:47:24
 *  Author: Michael Heil
 */ 


#ifndef MYDS3231_H_
#define MYDS3231_H_

#include <util/twi.h> // required for TW_READ and TW_WRITE

#ifndef MYLOG_H_
  #define MYLOG_H_
  inline void LOG_debug(char * msg){};
  inline void LOG_info(char * msg){};
  inline void LOG_error(char * msg){};
#endif // MYLOG_H_


// DS3231 I2C Slave Address is 0x68
#define DS3231_SLAVE_ADDRESS            (0x68)
#define DS3231_SLA_W                    ((DS3231_SLAVE_ADDRESS << 1) | TW_WRITE) // shifted by one as the address is stored in the higher 7-bits
#define DS3231_SLA_R                    ((DS3231_SLAVE_ADDRESS << 1) | TW_READ)  // TW_WRITE (=0) and TW_READ (=1) are defined in util/twi.h

#define DS3231_SECONDS_REG              (0x00)
#define DS3231_MINUTES_REG              (0x01)
#define DS3231_HOURS_REG                (0x02)
#define DS3231_DAY_REG                  (0x03)
#define DS3231_DATE_REG                 (0x04)
#define DS3231_MONTH_CENTURY_REG        (0x05)
#define DS3231_YEAR_REG                 (0x06)
#define DS3231_ALARM_1_SECONDS_REG      (0x07)
#define DS3231_ALARM_1_MINUTES_REG      (0x08)
#define DS3231_ALARM_1_HOURS_REG        (0x09)
#define DS3231_ALARM_1_DAY_DATE_REG     (0x0A)
#define DS3231_ALARM_2_MINUTES_REG      (0x0B)
#define DS3231_ALARM_2_HOURS_REG        (0x0C)
#define DS3231_ALARM_2_DAY_DATE_REG     (0x0D)
#define DS3231_CONTROL_REG              (0x0E)
#define DS3231_STATUS_REG               (0x0F)

#define DS3231_STATUS_REG_A1F           0
#define DS3231_STATUS_REG_A2F           1

/**
 * @brief Initializes DS3231 through the initialisation of TWI
 * 
 */
void DS3231_init(void);

///////////////////////////////////////////////////////
// getter functions

/*
 * Request current seconds
 * 
 * return: raw Bits of the requested DS3231 register 
 * (apply combineRegisterrBits helper function for actual value)
 */
uint8_t DS3231_getSeconds(void);

/*
 * Request current minutes
 * 
 * return: raw Bits of the requested DS3231 register 
 * (apply combineRegisterrBits helper function for actual value)
 */
uint8_t DS3231_getMinutes(void);

/*
 * Request current hours
 * 
 * return: raw Bits of the requested DS3231 register 
 * (apply combineRegisterrBits helper function for actual value)
 */
uint8_t DS3231_getHours(void);

/*
 * Request current Seconds, Minutes, and Hours
 * 
 * resPointer: 3-byte buffer for raw Bits of the requested DS3231 registers
 * (apply combineRegisterrBits helper function for actual value)
 */
void DS3231_getTime(uint8_t * resPointer);

/*
 * Get time in format HH:mm:ss
 * 
 * resPointer: 9-byte buffer for time string
 * 
 * usage: 
 * char time[9];
 * DS3231_getTimeString(&time[0]);
 * USART_writeStringLn(&time[0]);
 */
void DS3231_getTimeString(char * resPointer);

/*
 * Request current Date, Month, and Year
 * 
 * resPointer: 3-byte buffer for raw Bits of the requested DS3231 registers
 * (apply combineRegisterrBits helper function for actual value)
 */
void DS3231_getDMY(uint8_t * resPointer);

/*
 * Get date, month, year in format yyyy-DD-mm
 * 
 * resPointer: 11-byte buffer for timestamp string
 * 
 * usage: 
 * char dmy[11];
 * DS3231_getDMYString(&dmy[0]);
 * USART_writeStringLn(&dmy[0]);
 */
void DS3231_getDMYString(char * resPointer);

/*
 * Request current Seconds, Minutes, Hours, Day, Date, Month, and Year
 * 
 * resPointer: 7-byte for raw Bits of the requested DS3231 registers
 * (apply combineRegisterrBits helper function for actual value)
 */
void DS3231_getTimestamp(uint8_t * resPointer);

/*
 * Get timestamp in format yyyy-MM-dd'T'HH:mm:ss
 * 
 * resPointer: 22-byte buffer for timestamp string
 * 
 * usage: 
 * char timestamp[22];
 * DS3231_getTimestampString(&timestamp[0]);
 * USART_writeStringLn(&timestamp[0]);
 */
void DS3231_getTimestampString(char * resPointer);

/*
 * Get day as String
 * 
 * resPointer: storage for day string
 * 
 * char day[10];
 * DS3231_getDayString(&day[0]);
 * USART_writeStringLn(&day[0]);
 */  
void DS3231_getDayString(char * resPointer);

///////////////////////////////////////////////////////
// Alarm

/**
 * @brief 
 * 
 * @return int 
 */
int DS3231_resetAlarmsFlags(void);

/**
 * @brief 
 * 
 * @return int 
 */
int DS3231_setAlarmOncePerMinute(void);


/*
 * Combine the bits of the DS3231 register bytes into decimal numbers
 * 
 * rawData: raw register byte value
 * 
 * return: decimal number
 */
uint8_t DS3231_combineRegisterBits(uint8_t rawData);


#endif /* MYDS3231_H_ */
