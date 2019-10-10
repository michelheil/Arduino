/*
 * myAMG8833.h
 *
 * Created: 10.10.2019 19:43:31
 *  Author: Michael
 */ 


#ifndef MYAMG8833_H_
#define MYAMG8833_H_

#include "globalDefines.h"

#include <util/twi.h>

// AMG8833 Power Control Register (PCTL)
#define AMG8833_PCTL                    0x00
#define AMG8833_PCTL_NORMAL_MODE        0x00
#define AMG8833_PCTL_SLEEP_MODE         0x10
#define AMG8833_PCTL_STAND_BY_MODE_60   0x20 // 60 sec intermittence
#define AMG8833_PCTL_STAND_BY_MODE_10   0x21 // 10 sec intermittence

// AMG8833 Reset Register (RST)
#define AMG8833_RST                     0x01
#define AMG8833_RST_FLAG_RESET          0x30
#define AMG8833_RST_INITIAL_RESET       0x3F

// AMG8833 Frame Rate Register (FPSC)
#define AMG8833_FPSC                    0x02
#define AMG8833_FPSC_1FPS               0x01
#define AMG8833_FPSC_10FPS              0x00

// AMG8833 Interrupt Control Register (INTC)
#define AMG8833_INTC                    0x03
#define AMG8833_INTC_INTMOD_ABS         0x02
#define AMG8833_INTC_INTMOD_DIFF        0x00
#define AMG8833_INTC_INTEN_ACTIVE       0x01
#define AMG8833_INTC_INTEN_REACTIVE     0x00


// AMG8833 I2C Slave Address is 0x69
#define AMG8833_SLAVE_ADDRESS           0x69
#define AMG8833_SLA_W                   ((AMG8833_SLAVE_ADDRESS << 1) | TW_WRITE) // shifted by one as the address is stored in the higher 7-bits
#define AMG8833_SLA_R                   ((AMG8833_SLAVE_ADDRESS << 1) | TW_READ)  // TW_WRITE (=0) and TW_READ (=1) are defined in util/twi.h

// AMG8833 Thermistor Register
#define AMG8833_THERMISTOR_BYTES        2       // Thermistor value consist of 2 bytes. bits T7..0 in lower byte and T10..8 in higher byte
// bit3 in higher byte carries sign (1=minus; 0=plus)
#define AMG8833_TTHL                    0x0E    // Thermistor Temperature Register (lower level)
#define AMG8833_TTHH                    0x0F    // Thermistor Temperature Register (higher level)
#define AMG8833_THERMISTOR_CONVERSION   0.0625  // According to data sheet: "1 LSB has 12 bit resolution which is equivalent to 0.0625"

// AMG8833 8x8 Temperature Grid Register
#define AMG8833_GRID_PIXELS             64      // Thermistor value consist of 8x8 pixels. Value of Pixel is stored in two Bytes.
// Bits T7..0 in lower byte and T10..8 in higher byte.
// Bit3 in higher byte carries sign (1=minus; 0=plus)
#define AMG8833_T01L                    0x80    // Pixel 1 Temperature Register (lower level)
#define AMG8833_T01H                    0x81    // Pixel 1 Temperature Register (higher level)
#define AMG8833_T64L                    0xFE    // Pixel 64
#define AMG8833_T64H                    0xFF    // Pixel 64
#define AMG8833_PIXEL_CONVERSION        0.25    // According to data sheet: "1 LSB has 12 bit resolution (11 bit + sign) which is equivalent to 0.25"

// setting TWAR is only required when the ATmega328p is in slave mode
// TWAR = (AMG8833_SLAVE_ADDRESS << 1); // move one bit to left as bit0 of TWAR is used for General Call


// AMG8833 specific functions
int     AMG8833_init();


#endif /* MYAMG8833_H_ */