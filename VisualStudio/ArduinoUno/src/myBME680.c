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
    uint8_t gas_wait = (BME680_GAS_WAIT_MULT_FACTOR_4X << BME680_GAS_WAIT_MULT_FACTOR_OFFSET) | ((uint8_t) 25); // to 100ms (= 4 * 25)
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_GAS_WAIT_0_REG, gas_wait);

    // Setting Heater Resistance
    uint8_t res_heat_0 = 0b10101111;
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_RES_HEAT_0_REG, res_heat_0);

    // Select Heater Profile and enable Gas Measurement
    uint8_t ctrl_gas_1 = (BME680_GAS_MEASUREMENT_ENABLE << BME680_GAS_MEASUREMENT_OFFSET) | ((uint8_t) BME680_HEATER_PROFILE_0);
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CTRL_GAS_1_REG, ctrl_gas_1);
}






#include <Arduino.h>

#include "globalDefines.h"
#include "myHelperFunctions.h"

#include <util/delay.h>
#include "myTWI.h"
#include "myUSART.h"
#include "myBME680.h"

void setup() {
  USART_init();
  TWI_init();
  BME680_init(BME680_HUM_OVER_SAMPLING_1X, BME680_TEMP_OVER_SAMPLING_2X, BME680_PRES_OVER_SAMPLING_16X);

  uint8_t ctrl_meas = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_POWER_MODE_REG);
  USART_writeString("ctrl_meas: ");
  USART_writeStringLn(uint82str(ctrl_meas));

  uint8_t ctrl_hum = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_HUM_OVER_SAMPLING_REG);
  USART_writeString("ctrl_hum: ");
  USART_writeStringLn(uint82str(ctrl_hum));

  uint8_t par_g1 = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_PAR_G1_REG);
  USART_writeString("par_g1: ");
  USART_writeStringLn(uint82str(par_g1));

  uint8_t par_g2_lsb = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_PAR_G2_REG_LSB);
  USART_writeString("par_g2_lsb: ");
  USART_writeStringLn(uint82str(par_g2_lsb));

  uint8_t par_g2_msb = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_PAR_G2_REG_MSB);
  USART_writeString("par_g2_msb: ");
  USART_writeStringLn(uint82str(par_g2_msb));

  uint8_t par_g3 = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_PAR_G3_REG);
  USART_writeString("par_g3: ");
  USART_writeStringLn(uint82str(par_g3));

  uint8_t res_heat_range = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_RES_HEAT_RANGE_REG); // bits <5:4>
  USART_writeString("res_heat_range: ");
  USART_writeStringLn(uint82str(res_heat_range));

  uint8_t res_heat_val = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_RES_HEAT_VAL_REG);
  USART_writeString("res_heat_val: ");
  USART_writeStringLn(uint82str(res_heat_val));

double target_temp = 300;
double amb_temp = 25;

double par_g1_2 = 239;
double par_g2_2 = 54214; // (211 << 8) | 198
double par_g3_2 = 28;
double res_heat_range2 = 2;
double res_heat_val2 = 46;

double var1 = ((double)par_g1_2 / 16.0) + 49.0;
double var2 = (((double)par_g2_2 / 32768.0) * 0.0005) + 0.00235;
double var3 = (double)par_g3_2 / 1024.0;
double var4 = var1 * (1.0 + (var2 * (double) target_temp));
double var5 = var4 + (var3 * (double)amb_temp);
uint8_t res_heat_x = (uint8_t)(3.4 * ((var5 * (4.0 / (4.0 + (double)res_heat_range2)) * (1.0/(1.0 + ((double)res_heat_val2 * 0.002)))) - 25));
 /*
(3.4 * ((125 * (4.0 / (4.0 + 2)) * (1.0/(1.0 + (46 * 0.002)))) - 25));
3.4 * ((125 * (4.0 / 6) * (1.0/(1.092))) - 25);
3.4 * ((125 * 0.66667 * (1.0/1.092)) - 25);
3.4 * ((125 * 0.66667 * 0.9157) - 25);
*/

  USART_writeString("var1: "); USART_writeStringLn(float2str(var1));
  USART_writeString("var2: "); USART_writeStringLn(float2str(var2));
  USART_writeString("var3: "); USART_writeStringLn(float2str(var3));
  USART_writeString("var4: "); USART_writeStringLn(float2str(var4));
  USART_writeString("var5: "); USART_writeStringLn(float2str(var5));
  USART_writeString("res_heat_x: "); USART_writeStringLn(uint82str(res_heat_x));


    uint8_t res_heat_0 = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_RES_HEAT_0_REG);
  USART_writeString("res_heat_0: ");
  USART_writeStringLn(uint82str(res_heat_0));

// Temperature reading
// using 2x for over sampling => temperature resolution is 17 bits (= 16 + (2 - 1), where 2 comes from 0b010)
// 8 bits in TEMP_MSB_REG, 8 bits in TEMP_LSB_REG and 1 bit (msb) from TEMP_XLSB_REG
uint8_t temp_msb = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_TEMP_MSB_REG);
USART_writeString("temp_msb: ");
  USART_writeStringLn(uint82str(temp_msb));

  uint8_t temp_lsb = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_TEMP_LSB_REG);
USART_writeString("temp_lsb: ");
  USART_writeStringLn(uint82str(temp_lsb));

  uint8_t temp_xlsb = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_TEMP_XLSB_REG);
USART_writeString("temp_xlsb: ");
  USART_writeStringLn(uint82str(temp_xlsb));

// https://github.com/SV-Zanshin/BME680/blob/master/src/Zanshin_BME680.cpp
// BME680_T1_LSB_REG = 8, weil 8+25 = 33

#define BME680_CALIB_T1_LSB_REG   0xE9
#define BME680_CALIB_T1_MSB_REG   0xEA
#define BME680_CALIB_T2_LSB_REG   0x8A
#define BME680_CALIB_T2_MSB_REG   0x8B
#define BME680_CALIB_T3_REG       0x8C

uint8_t calib_t1_lsb = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_CALIB_T1_LSB_REG);
uint8_t calib_t1_msb = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_CALIB_T1_MSB_REG);
uint8_t calib_t2_lsb = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_CALIB_T2_LSB_REG);
uint8_t calib_t2_msb = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_CALIB_T2_MSB_REG);
uint8_t calib_t3     = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_CALIB_T3_REG);

#define BME680_CONCAT_BYTES(msb, lsb)	(((uint16_t)msb << 8) | (uint16_t)lsb)

uint16_t par_t1 = (uint16_t) (BME680_CONCAT_BYTES(calib_t1_msb, calib_t1_lsb));
int16_t par_t2 = (int16_t) (BME680_CONCAT_BYTES(calib_t2_msb, calib_t2_lsb));
int8_t par_t3 = (int8_t) (calib_t3);


uint32_t temp_adc = 0b00001111000100000011; //0xF103; //((uint32_t)(temp_msb << 9)) | ((uint32_t)(temp_lsb << 1)) | (uint32_t)(temp_xlsb >> 7);
USART_writeString("temp_adc: ");
USART_writeStringLn(uint322str(temp_adc));

	float var1_calc = 0;
	float var2_calc = 0;
	float calc_temp = 0;

	/* calculate var1 data */
	var1_calc  = ((((float)61699.0f / 16384.0f) - ((float)par_t1 / 1024.0f))
			* ((float)par_t2));

	/* calculate var2 data */
	var2_calc  = (((((float)61699.0f / 131072.0f) - ((float)par_t1 / 8192.0f)) *
		(((float)61699.0f / 131072.0f) - ((float)par_t1 / 8192.0f))) *
		((float)par_t3 * 16.0f));

	/* t_fine value*/
	float t_fine = (var1_calc + var2_calc);

	/* compensated temperature data*/
	calc_temp  = ((t_fine) / 5120.0f);

USART_writeString("calc_temp: ");
USART_writeFloat(calc_temp);





}

void loop() {
  _delay_ms(2000);
}