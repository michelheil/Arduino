/*
 * myGY271.c
 *
 * Created: 16.11.2019 10:50:07
 *  Author: Michael
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "myTWI.h"

#include "myGY271.h"

// check if new measurement is ready
void GY271_waitForNewData(void)
{
  uint8_t statusRegister, drdy;

  do {
    statusRegister = TWI_getRegisterByte(GY271_SLAVE_ADDRESS, GY271_STATUS_REGISTER);

    // bit masking of drdy as it is the 0 bit within Status Register
    drdy = statusRegister & 0x01;
  } while (!drdy);

  _delay_ms(1);
}

// check if any output data was missed reading
int GY271_checkIfMissedReading(void)
{
  uint8_t statusRegister, dor;

  statusRegister = TWI_getRegisterByte(GY271_SLAVE_ADDRESS, GY271_STATUS_REGISTER);
  dor = (statusRegister >> 2);

  return dor == 1 ? 1 : 0;
}


void GY271_init(void)
{
  // Soft Reset
  TWI_setRegisterByte(GY271_SLAVE_ADDRESS, GY271_CONTROL_REGISTER_2, GY271_CR2_SOFT_RESET);
  _delay_ms(1);

  // configuration (oversampling, range, rate, mode)
  TWI_setRegisterByte(GY271_SLAVE_ADDRESS, GY271_CONTROL_REGISTER_1, GY271_CR1_CONTINUOUS_MODE);

  // reset
  TWI_setRegisterByte(GY271_SLAVE_ADDRESS, GY271_SET_RESET_PERIOD_REGISTER, GY271_FBR_RECOMMENDATION);
  _delay_ms(1);
}

// get (x,y,z) values
struct xyzInt16Values GY271_getThreeAxisValues(void)
{
  struct xyzInt16Values magneticGY271;

  // wait for new measurment data put into the output registers
  GY271_waitForNewData();

  // read out raw values
  uint8_t gy271RawOutput[6];
  TWI_getRegisterBytes(GY271_SLAVE_ADDRESS, GY271_X_LSB, 6, &gy271RawOutput[0]);

  magneticGY271.x = CONCATENATE_INT16_BYTES(gy271RawOutput[1], gy271RawOutput[0]);
  magneticGY271.y = CONCATENATE_INT16_BYTES(gy271RawOutput[3], gy271RawOutput[2]);
  magneticGY271.z = CONCATENATE_INT16_BYTES(gy271RawOutput[5], gy271RawOutput[4]);

  return magneticGY271;
}

// Temperature reading
// Only relative temperature is correct. The temperature coefficient is about 100 LSB/°C.
int16_t GY271_getRelativeTemperature(void)
{
  uint8_t lsb = TWI_getRegisterByte(GY271_SLAVE_ADDRESS, GY271_TEMPERATURE_LSB);
  uint8_t msb = TWI_getRegisterByte(GY271_SLAVE_ADDRESS, GY271_TEMPERATURE_MSB);
  return CONCATENATE_INT16_BYTES(msb, lsb);
}
