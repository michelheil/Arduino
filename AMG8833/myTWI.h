/*
 * myTWI.h
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


#ifndef MYTWI_H_
#define MYTWI_H_

#include <avr/io.h>

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
*/

#define TWI_PRESCALER_VALUE     0x04 // Dec: 4
#define TWI_BIT_RATE            0x12 // Dec: 18

// TWI general function
void        TWI_init(void);
uint16_t    TWI_readBytesFromAddressRaw(uint8_t devAddress, uint8_t regAddress, int numberBytes);
uint16_t *  TWI_readPairBytesFromAddressRaw(uint8_t devAddress, uint8_t regAddress, int numberPix);
int         TWI_readAMG8833Bytes(uint8_t sla, uint8_t reg, int len, uint8_t * dest);

// TWI function for transmissions
void        TWI_startTransmission(void);
void        TWI_repeatStartTransmission(void);
void        TWI_writeSlaW(uint8_t slaw);
void        TWI_writeRegisterAddress(uint8_t addr);
void        TWI_writeByte(uint8_t addr);
void        TWI_writeSlaR(uint8_t slar);
void        TWI_stopTransmission(void);

// helper functions
float       TWI_signedMag12ToFloat(uint16_t val);
float       TWI_int12ToFloat(uint16_t val);

#endif /* MYTWI_H_ */