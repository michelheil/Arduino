/*
 * myBME680.h
 *
 * Created: 16.11.2019 10:47:24
 *  Author: Michael
 */ 


#ifndef MYBME680_H_
#define MYBME680_H_

#include <util/twi.h> // required for TW_READ and TW_WRITE

// BME680 I2C Slave Address
// The 7-bit device address is 111011x. The 6 MSB bits are fixed. 
// The last bit is changeable by SDO value and can be changedduring operation. 
// Connecting SDO to GND results in slave address 1110110 (0x76);
// connection it to VDDIO results in slave address 1110111 (0x77)
#define BME680_SLAVE_ADDRESS            0x77 //0x76
#define BME680_SLA_W                    ((BME680_SLAVE_ADDRESS << 1) | TW_WRITE) // shifted by one as the address is stored in the higher 7-bits
#define BME680_SLA_R                    ((BME680_SLAVE_ADDRESS << 1) | TW_READ)  // TW_WRITE (=0) and TW_READ (=1) are defined in util/twi.h

// Software Reset
// Writing 0xB6 to this register initiates a soft-reset procedure, which has the same effect like power-on reset.
#define BME680_RESET_REG                0xE0 // for I2C
#define BME680_RESET_SOFT               0xB6

// Power Mode; TPHG = Temperature, Pressure, Humidity and Gas
#define BME680_POWER_MODE_REG           0x74
#define BME680_POWER_MODE_SLEEP         0x00 // No measurements are performed; Minimal power consumption
#define BME680_POWER_MODE_FORCED        0x01 // Single TPHG cycle is performed; Sensor automatically returns to sleep mode afterwards; Gas sensor heater only operates during gas measurement

// Configuration
// The sensor is configured by writing to a set of control registers.
//
// In this example, the sensor will be configured to use 2x oversampling for its temperature measurements, 16x oversampling
// for the pressure signal, and 1x oversampling for humidity. Moreover, the gas sensor hot plate will be configured to be heated
// for 100 ms at 300 °C before the gas measurement is performed.
// First, the user must configure the oversampling settings for temperature, pressure and humidity by setting the control
// registers osrs_t<2:0> and osrs_h<2:0>, respectively. Supported settings range from 16x oversampling down to 0x, which is
// equivalent to skipping the corresponding sub-measurement.
//
// It is highly recommended to set first osrs_h<2:0> followed by osrs_t<2:0> and osrs_p<2:0>.

// Humidity
#define BME680_HUM_OVER_SAMPLING_REG        0x72 // osrs_h<2:0>
#define BME680_HUM_OVER_SAMPLING_0X         0b000 // measurement skipped
#define BME680_HUM_OVER_SAMPLING_1X         0b001
#define BME680_HUM_OVER_SAMPLING_2X         0b010
#define BME680_HUM_OVER_SAMPLING_4X         0b011
#define BME680_HUM_OVER_SAMPLING_8X         0b100
#define BME680_HUM_OVER_SAMPLING_16X        0b101

// Temperature
#define BME680_TEMP_OVER_SAMPLING_REG       0x74 // osrs_t<7:5>
#define BME680_TEMP_OVER_SAMPLING_OFFSET    5 
#define BME680_TEMP_OVER_SAMPLING_0X        0b000 // measurement skipped
#define BME680_TEMP_OVER_SAMPLING_1X        0b001
#define BME680_TEMP_OVER_SAMPLING_2X        0b010
#define BME680_TEMP_OVER_SAMPLING_4X        0b011
#define BME680_TEMP_OVER_SAMPLING_8X        0b100
#define BME680_TEMP_OVER_SAMPLING_16X       0b101

// Pressure
#define BME680_PRES_OVER_SAMPLING_REG       0x74 // osrs_p<4:2>
#define BME680_PRES_OVER_SAMPLING_OFFSET    2
#define BME680_PRES_OVER_SAMPLING_0X        0b000 // measurement skipped
#define BME680_PRES_OVER_SAMPLING_1X        0b001
#define BME680_PRES_OVER_SAMPLING_2X        0b010
#define BME680_PRES_OVER_SAMPLING_4X        0b011
#define BME680_PRES_OVER_SAMPLING_8X        0b100
#define BME680_PRES_OVER_SAMPLING_16X       0b101

// Next, the user shall set at least one gas sensor hot plate temperature set-point and heating duration.
// Heating Duration
// Set gas_wait_0<7:0> to 0x59 to select 100 ms heat up duration
#define BME680_GAS_WAIT_0_REG               0x64
#define BME680_GAS_WAIT_MULT_FACTOR_OFFSET  6
#define BME680_GAS_WAIT_MULT_FACTOR_1X      0b00
#define BME680_GAS_WAIT_MULT_FACTOR_4X      0b01
#define BME680_GAS_WAIT_MULT_FACTOR_16X     0b10
#define BME680_GAS_WAIT_MULT_FACTOR_64X     0b11

// Target Heater Resistance
// Set the corresponding heater set-point by writing the target heater resistance to res_heat_0<7:0>


// Set nb_conv<3:0> to 0x0 to select the previously defined heater settings
#define BME680_CTRL_GAS_1_REG               0x71
#define BME680_HEATER_PROFILE_0             0b0000
#define BME680_HEATER_PROFILE_1             0b0001
// ...
#define BME680_HEATER_PROFILE_9             0b1001

// Set run_gas_l to 1 to enable gas measurements
#define BME680_GAS_MEASUREMENT_OFFSET       4
#define BME680_GAS_MEASUREMENT_ENABLE       1
#define BME680_GAS_MEASUREMENT_DISABLE      0


void BME680_init(uint8_t hum_over_sampling, uint8_t temp_over_sampling, uint8_t pres_over_sampling);

#endif /* MYBME680_H_ */