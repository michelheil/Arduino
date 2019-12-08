/**
 * @page TWI_page myTWI.h
 * @brief Programm to communicate with I2C devices through Two-Wire Interface (ATMega328P)
 * @details Here I can add some details to this library
 * 
 * @date 09.10.2019 18:41:08
 * @author Michael
 * @copyright GNU General Public License
 * 
 * @section TWI TWI Register Overview
 * 
 * The following list of registers are available for ATMega328P to work with the two-wire interface (TWI).
 * 
 * @li TWSR - TWI Status Register (initial value: 0xF8)
 * @li TWBR - TWI Bit Rate Register (initial value: 0x00)
 * @li TWCR - TWI Control Register
 * @li TWDR - TWI Data Register (initial value: 0xFF)
 * @li TWAR - TWI (Slave) Address Register; Includes TWGCE (TWI General Call Recognition Enable Bit) as bit 0
 * @li TWAMR - TWI (Slave) Address Mask Register
 *
 * @section CLK Calculation of SCL frequency
 * 
 * According to the AMG88** data sheet the SCL clock frequency (f_SCL) has @n
 * min = 0 kHz and max = 400 kHZ @n
 * @n
 * According to the ATmega328p data sheet the SCL clock frequency is @n
 * f_SCL = CPU_ClockFrequency / (16 + 2 * TWBR * PrescalerValue) @n
 * where TWBR is the value of the TWI Bit Rate Register @n
 * and the PrescaleValue (in {1, 4, 16, 64}) can be set through the TWSR (TWI Status Register) @n
 * @n
 * Assuming we want to work with a f_SCL of 100kHz @n
 * => TWBR * PrescalerValue = 72 @n
 * => TWBR = 18 = 0b00010010 @n
 * => PrescalerValue: TWPS1 = 0, TWPS0 = 1 (both bits are part of byte TWSR) @n
 * #define TWI_PRESCALER_VALUE     0x01 // PrescalerValue: 4 @n
 * #define TWI_BIT_RATE            0x12 // Decimal: 18 @n
 * @n
 * @n
 * In General f_SCL = x (known) @n
 * TWBR * PrescalerValue =: y (unknown) @n
 * x = CPU_ClockFrequency / (16 + 2y) @n
 * <=> 16x + 2xy = CPU_ClockFrequency @n
 * <=> 2xy = CPU_ClockFrequency - 16x @n
 * <=> y = (CPU_ClockFrequency - 16x) / 2x @n
 * @n
 * x = 400kHz @n
 * => y = 12 (= TWBR * PrescalerValue) @n
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MYTWI_H_
#define MYTWI_H_

#include <avr/io.h> // required for uint types
#include "myLOG.h"

#define TWI_PRESCALER_VALUE     0x00 /**< TWPS1 = 0 and TWPS0 = 0 means PrescalerValue of 1 */
#define TWI_BIT_RATE            0x0C /**< Decimal: 12 */

/**
 * @brief Initializes the TWI (I2C) communication on Arduino by
 * @li enabling TWI register bits
 * @li setting clock frequency of SCL
 * @li (activating pull-up resistors for SDA and SCL)
 *
 * @return          0 if successful
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
 * @return          uint8_t byte value from the regster address
 */
uint8_t TWI_getRegisterByte(uint8_t sla, uint8_t reg);


/**
 * @brief           Read multiple bytes of the device through TWI (I2C)
 * 
 * @param sla       slave address
 * @param reg       register address that should be read
 * @param len       number of bytes to be read
 * @param dest      pointer to an array containing the content of read bytes
 * 
 * @return          number of read bytes as int (should be equal to len)
 */
int TWI_getRegisterBytes(uint8_t sla, uint8_t reg, int len, uint8_t * dest);

#endif /* MYTWI_H_ */

#ifdef __cplusplus
}
#endif
