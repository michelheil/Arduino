/*
    Programm to communicate with I2C devices through Two-Wire Interface (ATMega328P)
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
 * Created: 09.10.2019 18:41:08
 *  Author: Michael
 */ 

/*
 * TWSR - TWI Status Register (initial value: 0xF8)
 * TWBR - TWI Bit Rate Register (initial value: 0x00)
 * TWCR - TWI Control Register
 * TWDR - TWI Data Register (initial value: 0xFF)
 * TWAR - TWI (Slave) Address Register; Includes TWGCE (TWI General Call Recognition Enable Bit) as bit 0.
 * TWAMR - TWI (Slave) Address Mask Register
*/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MYTWI_H_
#define MYTWI_H_

#include <avr/io.h> // required for uint types

#ifndef MYLOG_H_
  #define MYLOG_H_
  inline void LOG_debug(char * msg){};
  inline void LOG_info(char * msg){};
  inline void LOG_error(char * msg){};
#endif // MYLOG_H_


/*
 * According to the AMG88** data sheet the SCL clock frequency (f_SCL) has
 * min = 0 kHz and max = 400 kHZ
 *
 * According to the ATmega328p data sheet the SCL clock frequency is
 * f_SCL = CPU_ClockFrequency / (16 + 2 * TWBR * PrescalerValue)
 * where TWBR is the value of the TWI Bit Rate Register
 * and the PrescaleValue (in {1, 4, 16, 64}) can be set through the TWSR (TWI Status Register)
 * 
 * Assuming we want to work with a f_SCL of 100kHz
 * => TWBR * PrescalerValue = 72
 * => TWBR = 18 = 0b00010010
 * => PrescalerValue: TWPS1 = 0, TWPS0 = 1 (both bits are part of byte TWSR)
 * #define TWI_PRESCALER_VALUE     0x01 // PrescalerValue: 4
 * #define TWI_BIT_RATE            0x12 // Decimal: 18
 * 
 * 
 * In General f_SCL = x (known)
 * TWBR * PrescalerValue =: y (unknown)
 * x = CPU_ClockFrequency / (16 + 2y)
 * <=> 16x + 2xy = CPU_ClockFrequency
 * <=> 2xy = CPU_ClockFrequency - 16x
 * <=> y = (CPU_ClockFrequency - 16x) / 2x
 * 
 * x = 400kHz
 * => y = 12 (= TWBR * PrescalerValue)
*/

#define TWI_PRESCALER_VALUE     0x00 // TWPS1 = 0 and TWPS0 = 0 means PrescalerValue of 1
#define TWI_BIT_RATE            0x0C // Decimal: 12

// TWI general function


/*
 * Initializes the TWI (I2C) communication on Arduino by
 * . enabling TWI register bits
 * . setting clock frequency of SCL
 * (. activating pull-up resistors for SDA and SCL)
 *
 * returns: 0
 */
int TWI_init(void);

/**
 * @brief           Set one byte in register of the device through TWI (I2C)
 * 
 * @param sla       slave address
 * @param reg       register address that should be set
 * @param val       byte value that is written into the register
 */
void TWI_setRegisterByte(uint8_t sla, uint8_t reg, uint8_t val);

/**
 * @brief           Read one byte register of the device through TWI (I2C)
 * 
 * @param sla       slave address
 * @param reg       register address that should be read
 * @return uint8_t  byte value from the regster address
 */
uint8_t TWI_getRegisterByte(uint8_t sla, uint8_t reg);


/**
 * @brief           Read multiple byte register of the device through TWI (I2C)
 */
int TWI_getRegisterBytes(uint8_t sla, uint8_t reg, int len, uint8_t * dest);

#endif /* MYTWI_H_ */

#ifdef __cplusplus
}
#endif