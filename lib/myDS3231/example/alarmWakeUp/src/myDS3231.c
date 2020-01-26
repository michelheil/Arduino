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

// reset Alarm flags
int DS3231_resetAlarmsFlags(void)
{
  // extract current value in status register for masking it while switching the alarm flags off
  uint8_t status_reg = TWI_getRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_STATUS_REG);
  status_reg &= ~((1 << DS3231_STATUS_REG_A2F) | (1 << DS3231_STATUS_REG_A1F));
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_STATUS_REG, status_reg);

  return 1;
}

//
int DS3231_setAlarmOncePerSecond(void)
{
  // Alarm once per second
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_DAY_DATE_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_HOURS_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_MINUTES_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_SECONDS_REG, 0x80);
  
  // set Interrupt Control (INTCN) and Alarm 2 Interrupt Enable (A2IE) and Alarm 1 Interrupt Enable (A1IE)
  // important to enable both(!) alarm interrupts
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG, 6);
  return 1;
}

//
int DS3231_setAlarmSeconds(uint8_t sec)
{
  //ToDo: Check input parameters  
  
  // Alarm once per second
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_DAY_DATE_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_HOURS_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_MINUTES_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_SECONDS_REG, sec);
  
  // set Interrupt Control (INTCN) and Alarm 2 Interrupt Enable (A2IE) and Alarm 1 Interrupt Enable (A1IE)
  // important to enable both(!) alarm interrupts
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG, 6);
  return 1;
}

//
int DS3231_setAlarmMinutesSeconds(uint8_t minutes, uint8_t sec)
{
  //ToDo: Check input parameters  
  
  // Alarm once per second
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_DAY_DATE_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_HOURS_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_MINUTES_REG, minutes);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_SECONDS_REG, sec);
  
  // set Interrupt Control (INTCN) and Alarm 2 Interrupt Enable (A2IE) and Alarm 1 Interrupt Enable (A1IE)
  // important to enable both(!) alarm interrupts
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG, 6);
  return 1;
}

//
int DS3231_setAlarmHoursMinutesSeconds(uint8_t hours, uint8_t minutes, uint8_t sec)
{
  //ToDo: Check input parameters

  // Alarm once per second
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_DAY_DATE_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_HOURS_REG, hours);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_MINUTES_REG, minutes);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_SECONDS_REG, sec);
  
  // set Interrupt Control (INTCN) and Alarm 2 Interrupt Enable (A2IE) and Alarm 1 Interrupt Enable (A1IE)
  // important to enable both(!) alarm interrupts
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG, 6);
  return 1;
}

//
int DS3231_setAlarmDateHoursMinutesSeconds(uint8_t date, uint8_t hours, uint8_t minutes, uint8_t sec)
{
  //ToDo: Check input parameters

  // Alarm once per second
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_DAY_DATE_REG, date);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_HOURS_REG, hours);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_MINUTES_REG, minutes);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_SECONDS_REG, sec);
  
  // set Interrupt Control (INTCN) and Alarm 2 Interrupt Enable (A2IE) and Alarm 1 Interrupt Enable (A1IE)
  // important to enable both(!) alarm interrupts
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG, 6);
  return 1;
}

//
int DS3231_setAlarmDayHoursMinutesSeconds(uint8_t day, uint8_t hours, uint8_t minutes, uint8_t sec)
{
  // DY/DT flag has to be set to 1 to select DY
  day |= (1 << 6);

  //ToDo: Check input parameters

  // Alarm once per second
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_DAY_DATE_REG, day);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_HOURS_REG, hours);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_MINUTES_REG, minutes);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_1_SECONDS_REG, sec);
  
  // set Interrupt Control (INTCN) and Alarm 2 Interrupt Enable (A2IE) and Alarm 1 Interrupt Enable (A1IE)
  // important to enable both(!) alarm interrupts
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG, 6);
  return 1;
}


// set Alarm 2 for once a minute (at 00 seconds)
int DS3231_setAlarmOncePerMinute(void)
{
  // Alarm once per minute (00 seconds of every minute)
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_DAY_DATE_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_HOURS_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_MINUTES_REG, 0x80);
  
  // set Interrupt Control (INTCN) and Alarm 2 Interrupt Enable (A2IE) and Alarm 1 Interrupt Enable (A1IE)
  // important to enable both(!) alarm interrupts
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG, 6);
  return 1;
}

//
int DS3231_setAlarmMinutes(uint8_t minutes)
{
  // ToDo: Check input parameters

  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_DAY_DATE_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_HOURS_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_MINUTES_REG, minutes);

  // set Interrupt Control (INTCN) and Alarm 2 Interrupt Enable (A2IE) and Alarm 1 Interrupt Enable (A1IE)
  // important to enable both(!) alarm interrupts
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG, 6);
  return 1;
}

//
int DS3231_setAlarmHoursMinutes(uint8_t hours, uint8_t minutes)
{
  // ToDo: Check input parameters

  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_DAY_DATE_REG, 0x80);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_HOURS_REG, hours);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_MINUTES_REG, minutes);

  // set Interrupt Control (INTCN) and Alarm 2 Interrupt Enable (A2IE) and Alarm 1 Interrupt Enable (A1IE)
  // important to enable both(!) alarm interrupts
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG, 6);
  return 1;
}

//
int DS3231_setAlarmDateHoursMinutes(uint8_t date, uint8_t hours, uint8_t minutes)
{
  // ToDo: Check input parameters

  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_DAY_DATE_REG, date);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_HOURS_REG, hours);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_MINUTES_REG, minutes);

  // set Interrupt Control (INTCN) and Alarm 2 Interrupt Enable (A2IE) and Alarm 1 Interrupt Enable (A1IE)
  // important to enable both(!) alarm interrupts
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG, 6);
  return 1;
}

//
int DS3231_setAlarmDayHoursMinutes(uint8_t day, uint8_t hours, uint8_t minutes)
{
  // DY/DT flag has to be set to 1 to select DY
  day |= (1 << 6);

  // ToDo: Check input parameters

  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_DAY_DATE_REG, day);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_HOURS_REG, hours);
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_ALARM_2_MINUTES_REG, minutes);

  // set Interrupt Control (INTCN) and Alarm 2 Interrupt Enable (A2IE) and Alarm 1 Interrupt Enable (A1IE)
  // important to enable both(!) alarm interrupts
  TWI_setRegisterByte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG, 6);
  return 1;
}


uint8_t DS3231_combineRegisterBits(uint8_t rawData) {
    
    return (10*(rawData >> 4) + (rawData & 0x0F));

}