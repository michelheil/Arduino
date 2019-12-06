/*
    Programm to communicate with AMG8833 device
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
 * Created: 10.10.2019 19:43:31
 *  Author: Michael
 */ 


#ifndef MYAMG8833_H_
#define MYAMG8833_H_

#include <util/twi.h> // required for TW_READ and TW_WRITE

// AMG8833 I2C Slave Address is 0x69
#define AMG8833_SLAVE_ADDRESS           0x69
#define AMG8833_SLA_W                   ((AMG8833_SLAVE_ADDRESS << 1) | TW_WRITE) // shifted by one as the address is stored in the higher 7-bits
#define AMG8833_SLA_R                   ((AMG8833_SLAVE_ADDRESS << 1) | TW_READ)  // TW_WRITE (=0) and TW_READ (=1) are defined in util/twi.h

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

// AMG8833 Status Register (for only reading) to indicate Overflow Flag and Interrupt Flag
#define AMG8833_SR                      0x04
#define AMG8833_SR_THERM_OVERFLOW       0x04 // bit3: OVF_THS 1: Thermistor Temperature Output Overflow ?Value of Thermistor (0x0E?0x0F) : 0xFFF?
#define AMG8833_SR_TEMP_OVERFLOW        0x02 // bit2: OVF_IRS 1: Temperature Output Overflow ?Value of Temperature Register?0x80?0xFF?: 0xFFF?
#define AMG8833_SR_INT_OUTBREAK         0x01 // bit1: INTF 1: Interrupt Outbreak ?Value of Interrupt Table Register?0x10?0x17?: Except for 0x00

// AMG8833 Clear Status Register (for only writing) to clear the Overflow Flag and Interrupt Flag. After writing, automatically turns 0x00.
#define AMG8833_CSR                     0x05
#define AMG8833_CSR_THERM_OVERFLOW      0x04 // bit3: OVT_CLR 1: Thermistor Temperature Output Overflow Flag Clear
#define AMG8833_CSR_TEMP_OVERFLOW       0x02 // bit2: OVS_CLR 1: Temperature Output Overflow Flag Clear
#define AMG8833_CSR_INT_OUTBREAK        0x01 // bit1: INTCLR 1: Interrupt Flag Clear
#define AMG8833_CSR_ALL                 0x07 // clear all flags

// AMG8833 Average Register for setting moving average Output Mode.
#define AMG8833_AVE                     0x07
#define AMG8833_AVE_SWITCH_ON           0x20 // bit5: MAMOD 1: Twice moving average Output Mode
#define AMG8833_AVE_SWITCH_OFF          0x00

// AMG8833 Interrupt Level Register 
// Register for setting upper / lower limit Hysteresis on Interrupt Level. 1 LSB has 12 bit resolution (11 bit + sign) 
// which is equivalent to 0.25? and it is indicated as two's complement form. 
#define AMG8833_INTHL                   0x08 // bits 7..0; Interrupt Level upper limit setting when the value is upper than the set value, 
#define AMG8833_INTHH                   0x09 // bits 11..8; Interrupt Output and Interrupt Pixel Table are set.
#define AMG8833_INTLL                   0x0A // bits 7..0; Interrupt Level lower limit setting when the value is lower than the set value,  
#define AMG8833_INTLH                   0x0B // bits 11..8; Interrupt Output and Interrupt Pixel Table are set.
#define AMG8833_IHYSL                   0x0C // bits 7..0; Setting of Interrupt Hysteresis Level when Interrupt is generated, set Hysteresis Level applied to Interrupt Level upper / lower value. 
#define AMG8833_IHYSH                   0x0D // bits 11..8; When the value is set over Interrupt Level, Interrupt Output cannot be correct. 



// AMG8833 Thermistor Register
#define AMG8833_THERMISTOR_BYTES        2       // Thermistor value consist of 2 bytes. bits T7..0 in lower byte and T10..8 in higher byte
// bit3 in higher byte carries sign (1=minus; 0=plus)
#define AMG8833_TTHL                    0x0E    // Thermistor Temperature Register (lower level)
#define AMG8833_TTHH                    0x0F    // Thermistor Temperature Register (higher level)
#define AMG8833_THERMISTOR_CONVERSION   0.0625  // According to data sheet: "1 LSB has 12 bit resolution which is equivalent to 0.0625"

// AMG8833 8x8 Temperature Grid Register
#define AMG8833_GRID_PIXELS             64      // Thermistor value consist of 8x8 pixels. Value of Pixel is stored in two Bytes.
#define AMG8833_GRID_PIXELS_X           8       // Pixels on x-axis
#define AMG8833_GRID_PIXELS_Y           8       // Pixels on y-axis
// Bits T7..0 in lower byte and T10..8 in higher byte.
// Bit3 in higher byte carries sign (1=minus; 0=plus)
#define AMG8833_T01L                    0x80    // Pixel 1 Temperature Register (lower level)
#define AMG8833_T01H                    0x81    // Pixel 1 Temperature Register (higher level)
#define AMG8833_T64L                    0xFE    // Pixel 64
#define AMG8833_T64H                    0xFF    // Pixel 64
#define AMG8833_PIXEL_CONVERSION        0.25    // According to data sheet: "1 LSB has 12 bit resolution (11 bit + sign) which is equivalent to 0.25"

// setting TWAR is only required when the ATmega328p is in slave mode
// TWAR = (AMG8833_SLAVE_ADDRESS << 1); // move one bit to left as bit0 of TWAR is used for General Call

//////////////////////////////////////////////////////////////////////////
// Functions

/*
 * Initialize the AMG8833 including initialization of TWI (I2C)
 * 
 * pcr: Power Control Register
 * rr: Reset Register
 * frr: Frame Rate Register
 * icr: Interrupt Control Register
 *
 * returns: 0
 */
int  AMG8833_init(uint8_t pcr, uint8_t rr, uint8_t frr, uint8_t icr);

/*
 * Set a register of the AMG8833 device through TWI (I2C)
 * 
 * reg: Register Address of AMG8833 that should be set
 * val: Byte value that is written into the register
 *
 * returns: no return value
 */
void AMG8833_setRegisterByte(uint8_t reg, uint8_t val);

/*
 * Read register of the AMG8833 device through TWI (I2C)
 * 
 * reg: Register Address of AMG8833 that should be read
 *
 * returns: Byte value that is written in the register
 */
uint8_t AMG8833_getRegisterByte(uint8_t reg);

/*
 * Read the Temperature Register of all 8x8 Pixels from AMG8833 through TWI (I2C)
 * 
 * resultGridValues: Pointer to float Array to store Celsius values
 *
 * returns: no return value
 */
void AMG8833_readGrid(float * resultGridValues);

/*
 * Read the Thermistor Register from AMG8833 through TWI (I2C)
 * 
 * no input parameters
 *
 * returns: float value containing temperature in Celsius
 */
float AMG8833_readThermistor(void);

/*
 * Read the Status Register from AMG8833 through TWI (I2C)
 * 
 * no input parameters
 *
 * returns: String describing the Status
 */
const char * AMG8833_readStatusRegister(void);

#endif /* MYAMG8833_H_ */
