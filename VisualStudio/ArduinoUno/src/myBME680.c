/*
 * myBME680.c
 *
 * Created: 17.11.2019 10:47:24
 *  Author: Michael
 */ 

#include "myBME680.h"
#include "myTWI.h"

void BME680_init(uint8_t hum_over_sampling, uint8_t temp_over_sampling, uint8_t pres_over_sampling)
{
    // Setting Humidity Oversampling
    uint8_t ctrl_hum = hum_over_sampling;
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_HUM_OVER_SAMPLING_REG, ctrl_hum);
    
    // Setting Power Mode, Temperature Oversampling and Pressure Oversampling
    uint8_t ctrl_meas = (temp_over_sampling << BME680_TEMP_OVER_SAMPLING_OFFSET) | (pres_over_sampling << BME680_PRES_OVER_SAMPLING_OFFSET) | BME680_POWER_MODE_FORCED;
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_POWER_MODE_REG, ctrl_meas); // registers for PowerMode, Temp and Pres are all 0x74

    // Setting Gas Wait Duration
    uint8_t gas_wait = (BME680_GAS_WAIT_MULT_FACTOR_4X << BME680_GAS_WAIT_MULT_FACTOR_OFFSET) | ((uint8_t) 25);
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_GAS_WAIT_0_REG, gas_wait);

    //

    // Select Heater Profile and enable Gas Measurement
    uint8_t ctrl_gas_1 = (BME680_GAS_MEASUREMENT_ENABLE << BME680_GAS_MEASUREMENT_OFFSET) | ((uint8_t) BME680_HEATER_PROFILE_0);
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CTRL_GAS_1_REG, ctrl_gas_1);
}