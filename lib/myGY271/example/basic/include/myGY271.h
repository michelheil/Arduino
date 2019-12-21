/** 
 * @page HCSR04_page myGY271.h
 * @section xxx
 * @brief xxx
 * 
 * @date 20.12.2019 18:41:08
 * @author Michael
 * @copyright GNU General Public License
 * 
 * @section GY271
 * 
 * Communication via I2C:
 * This device supports standard and fast speed modes, 100kHz and 400kHz, respectively
 * The default I C address is 0D: 0001101
 * 
 * Temperature Output:
 * QMC5883L has a built-in temperature sensor, it can provide temperature reading 
 * for other applications. The output is placed in Temperature Output Registers (07H and 08H). 
 * The temperature is calibrated for its sensitivity.
 * 
 */

#ifndef MYGY271_H_
#define MYGY271_H_

#include "myGlobalDefines.h"

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

void GY271_init(void);
struct xyzInt16Values GY271_getThreeAxisValues(void);
int16_t GY271_getRelativeTemperature(void);

#endif /* MYGY271_H_ */
