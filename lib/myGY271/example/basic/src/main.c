/*
 * Example for using library GY271
 * Created: 20.12.2019 18:23:18
 * Author : Michael
 */ 

#include "myGlobalDefines.h"

#include <util/delay.h>

#include "myGY271.h"
#include "myTWI.h"
#include "myUSART.h"

#define GY271_SLAVE_ADDRESS 0x0D // according to QMC5883L

// Registers 0x00 - 0x05H store the measurement data from each axis magnetic sensor
// in continuous-measurement. In the continuous measurement mode, the output data 
// is refreshed periodically based on the data update rate ODR setup in control 
// registers 1. The data stays the same, regardless of reading status through I2C,
// until new data replaces them. Each axis has 16 bit data width in 2’s complement,
// i.e., MSB of 0x01/0x03/0x05 indicates the sign of each axis. The output data of each
// channel saturates at -32768 and 32767.
#define GY271_X_LSB                       0x00
#define GY271_X_MSB                       0x01
#define GY271_Y_LSB                       0x02
#define GY271_Y_MSB                       0x03
#define GY271_Z_LSB                       0x04
#define GY271_Z_MSB                       0x05
#define GY271_STATUS_REGISTER             0x06
#define GY271_TEMPERATURE_LSB             0x07
#define GY271_TEMPERATURE_MSB             0x08

// OSR[1:0], RNG[1:0], ODR[1:0], MODE[1:0]
// OSR - Over Sampling Rate
// RNG - Range or Sensitivity of the Sensors
// ODR - Output Data Update Rate
// MODE - Operational Mode
#define GY271_CONTROL_REGISTER_1          0x09
#define GY271_CR1_CONTINUOUS_MODE 0x01 // Continuous Mode, 10Hz, 2G

// SOFT_RST, ROL_PNT, x, x, x, x, x, INT_ENB
// SOFT_RST - Soft reset
// POL_PNT - Point roll over function enabling
// [...] - five unused bits
// INT_ENB - Interrupt Pin enabling
#define GY271_CONTROL_REGISTER_2          0x0A
#define GY271_CR2_SOFT_RESET 0x80


#define GY271_SET_RESET_PERIOD_REGISTER   0x0B
#define GY271_FBR_RECOMMENDATION 0x01 // it is recommended that the register 0BH is written by 0x01


int main(void)
{
  // Initialize Serial (USART)
  USART_init();

  TWI_init();
  
// GY271_init  
  // Soft Reset
  TWI_setRegisterByte(GY271_SLAVE_ADDRESS, GY271_CONTROL_REGISTER_2, GY271_CR2_SOFT_RESET);
  _delay_ms(100);

  // configuration (oversampling, range, rate, mode)
  TWI_setRegisterByte(GY271_SLAVE_ADDRESS, GY271_CONTROL_REGISTER_1, GY271_CR1_CONTINUOUS_MODE);
  _delay_ms(100);

  // reset
  TWI_setRegisterByte(GY271_SLAVE_ADDRESS, GY271_SET_RESET_PERIOD_REGISTER, GY271_FBR_RECOMMENDATION);
  _delay_ms(100);
// GY271_init - end

  while(1)
  {
    uint8_t statusRegister, drdy, dor;

// check if new measurement is ready
    do {
      statusRegister = TWI_getRegisterByte(GY271_SLAVE_ADDRESS, GY271_STATUS_REGISTER);
      USART_writeStringLn("Waiting for new data");
      _delay_ms(500);
      drdy = statusRegister & 0x01; // bit masking of drdy as it is the 0 bit within Status Register
      USART_writeString("DRDY: ");
      USART_writeStringLn(uint82str(drdy));
    } while (!drdy); 
// check if new measurement is ready - end


// read all 3 axis values
int16_t xValue, yValue, zValue;

uint8_t gy271RawOutput[6];
TWI_getRegisterBytes(GY271_SLAVE_ADDRESS, GY271_X_LSB, 6, &gy271RawOutput[0]);
for(int i = 0; i < 6; i++) {
    USART_writeStringLn(uint82str(gy271RawOutput[i]));
}
xValue = ((int16_t)gy271RawOutput[1] << 8) | ((int16_t)gy271RawOutput[0]);
yValue = ((int16_t)gy271RawOutput[3] << 8) | ((int16_t)gy271RawOutput[2]);
zValue = ((int16_t)gy271RawOutput[5] << 8) | ((int16_t)gy271RawOutput[4]);
// read all 3 axis values - end

// check if any output data was missed reading
      statusRegister = TWI_getRegisterByte(GY271_SLAVE_ADDRESS, GY271_STATUS_REGISTER);
      dor = (statusRegister >> 2);
      USART_writeString("DOR: ");
      USART_writeStringLn(uint82str(dor));
// check if any output data was missed reading - end


// Temperature reading
    uint8_t val7 = TWI_getRegisterByte(GY271_SLAVE_ADDRESS, GY271_TEMPERATURE_LSB);
    USART_writeString("Temperature LSB: ");
    USART_writeStringLn(uint82str(val7));
    uint8_t val8 = TWI_getRegisterByte(GY271_SLAVE_ADDRESS, GY271_TEMPERATURE_MSB);
    USART_writeString("Temperature MSB: ");
    USART_writeStringLn(uint82str(val8));    
// Temperature reading - end
    
    
    _delay_ms(10000);
    USART_newLine();
    USART_newLine();
    USART_newLine();
  }
}
