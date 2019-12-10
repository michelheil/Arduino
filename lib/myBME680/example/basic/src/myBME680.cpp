/** @file myBME680.cpp
 * @section myBME680_intro_section Description
 * Library to read sensor data from Bosch BME680 through I2C from Arduino UNO
 */
#include "myBME680.h" // Include the header definition
#include "myTWI.h"

/**
 * @brief   Class constructor
 * @details Currently empty and unused
 */
BME680::BME680() {}

/**
 * @brief   Class destructor
 * @details Currently empty and unused
 */
BME680::~BME680() {}

/**
 * @brief   BME680 initialization function
 * @details Called from all of the overloaded "begin()" functions once protocol has been selected
 * @return "true" if successful otherwise false
 */
void BME680::init() 
{
  TWI_init(); // Initialise two-way interface of Arduino Uno
  getCalibration(); // get the calibration values only once at initialisation
}

/**
 * @brief   Read a single byte from the give address
 * @details interlude function to the "getData()" function
 * @param[in] addr Address of device
 * @return    single byte read
 */
uint8_t BME680::readBME680Byte(const uint8_t addr) 
{
  return TWI_getRegisterByte(BME680_SLAVE_ADDRESS, addr);
}

/**
 * @brief   Performs a soft device reset
 */
void BME680::reset()
{
  TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_RESET_REG, BME680_RESET_SOFT); // write reset code to device
  init(); // Start device again
}

/**
 * @brief     reads the calibration register data into local variables for use in converting readings
 * @details   The calibration registers are read into a temporary array and then parsed into the appropriate calibration
 *            variables, this was taken from the example BOSCH software and minimizes register reads, but makes it 
 *            rather difficult to read. This will be redone for legibility at some point in the future
 * @param[in] addr Address of device
 * @return    single byte read
 */
void BME680::getCalibration()
{
  const uint8_t BME680_COEFF_SIZE1              =    25; /**< First array with coefficients */
  const uint8_t BME680_COEFF_SIZE2              =    16; /**< Second array with coefficients */
  const uint8_t BME680_COEFF_START_ADDRESS1     =  0x89; /**< start address for array 1 */
  const uint8_t BME680_COEFF_START_ADDRESS2     =  0xE1; /**< start address for array 2 */
  const uint8_t BME680_HUM_REG_SHIFT_VAL        =     4; /**< Ambient humidity shift value */
  const uint8_t BME680_BIT_H1_DATA_MSK          =  0x0F;
  const uint8_t BME680_T2_LSB_REG               =     1;
  const uint8_t BME680_T2_MSB_REG               =     2;
  const uint8_t BME680_T3_REG		                =     3;
  const uint8_t BME680_P1_LSB_REG   	          =     5;
  const uint8_t BME680_P1_MSB_REG	              =     6;
  const uint8_t BME680_P2_LSB_REG	              =     7;
  const uint8_t BME680_P2_MSB_REG	              =     8;
  const uint8_t BME680_P3_REG		                =     9;
  const uint8_t BME680_P4_LSB_REG         	    =    11;
  const uint8_t BME680_P4_MSB_REG        	      =    12;
  const uint8_t BME680_P5_LSB_REG           	  =    13;
  const uint8_t BME680_P5_MSB_REG         	    =    14;
  const uint8_t BME680_P7_REG  	                =    15;
  const uint8_t BME680_P6_REG	                  =    16;
  const uint8_t BME680_P8_LSB_REG    	          =    19;
  const uint8_t BME680_P8_MSB_REG	              =    20;
  const uint8_t BME680_P9_LSB_REG    	          =    21;
  const uint8_t BME680_P9_MSB_REG	              =    22;
  const uint8_t BME680_P10_REG		              =    23;
  const uint8_t BME680_H2_MSB_REG	              =     0;
  const uint8_t BME680_H2_LSB_REG	              =     1;
  const uint8_t BME680_H1_LSB_REG    	          =     1;
  const uint8_t BME680_H1_MSB_REG	              =     2;
  const uint8_t BME680_H3_REG	                  =     3;
  const uint8_t BME680_H4_REG    	              =     4;
  const uint8_t BME680_H5_REG	                  =     5;
  const uint8_t BME680_H6_REG                   =     6;
  const uint8_t BME680_H7_REG                   =     7;
  const uint8_t BME680_T1_LSB_REG	              =     8;
  const uint8_t BME680_T1_MSB_REG    	          =     9;
  const uint8_t BME680_GH2_LSB_REG              =    10;
  const uint8_t BME680_GH2_MSB_REG              =    11;
  const uint8_t BME680_GH1_REG	                =    12;
  const uint8_t BME680_GH3_REG	                =    13;
  const uint8_t BME680_ADDR_RES_HEAT_RANGE_ADDR =  0x02;
  const uint8_t BME680_RHRANGE_MSK              =  0x30;
  const uint8_t BME680_ADDR_RES_HEAT_VAL_ADDR   =  0x00;
  const uint8_t BME680_ADDR_RANGE_SW_ERR_ADDR   =  0x04;
  const uint8_t BME680_RSERROR_MSK	            =  0xF0;

  uint8_t coeff_array1[BME680_COEFF_SIZE1]  = {0}; // Define temporary array 1
  uint8_t coeff_array2[BME680_COEFF_SIZE2]  = {0}; // Define temporary array 2
  
  // Split reading registers into 2 one 25 bytes and the other 16 bytes
  TWI_getRegisterBytes(BME680_SLAVE_ADDRESS, BME680_COEFF_START_ADDRESS1, BME680_COEFF_SIZE1, &coeff_array1[0]); 
  TWI_getRegisterBytes(BME680_SLAVE_ADDRESS, BME680_COEFF_START_ADDRESS2, BME680_COEFF_SIZE2, &coeff_array2[0]);

  /**************************************
   ** Temperature related coefficients **
   *************************************/
  _T1  = (uint16_t) (CONCAT_BYTES(coeff_array2[BME680_T1_MSB_REG], coeff_array2[BME680_T1_LSB_REG]));
  _T2  = (int16_t)  (CONCAT_BYTES(coeff_array1[BME680_T2_MSB_REG], coeff_array1[BME680_T2_LSB_REG]));
  _T3  = (int8_t)   (coeff_array1[BME680_T3_REG]);
  
  /**************************************
   ** Pressure related coefficients    **
   *************************************/
  _P1  = (uint16_t) (CONCAT_BYTES(coeff_array1[BME680_P1_MSB_REG], coeff_array1[BME680_P1_LSB_REG]));
  _P2  = (int16_t)  (CONCAT_BYTES(coeff_array1[BME680_P2_MSB_REG], coeff_array1[BME680_P2_LSB_REG]));
  _P3  = (int8_t)    coeff_array1[BME680_P3_REG];
  _P4  = (int16_t)  (CONCAT_BYTES(coeff_array1[BME680_P4_MSB_REG], coeff_array1[BME680_P4_LSB_REG]));
  _P5  = (int16_t)  (CONCAT_BYTES(coeff_array1[BME680_P5_MSB_REG], coeff_array1[BME680_P5_LSB_REG]));
  _P6  = (int8_t)   (coeff_array1[BME680_P6_REG]);
  _P7  = (int8_t)   (coeff_array1[BME680_P7_REG]);
  _P8  = (int16_t)  (CONCAT_BYTES(coeff_array1[BME680_P8_MSB_REG], coeff_array1[BME680_P8_LSB_REG]));
  _P9  = (int16_t)  (CONCAT_BYTES(coeff_array1[BME680_P9_MSB_REG], coeff_array1[BME680_P9_LSB_REG]));
  _P10 = (uint8_t)  (coeff_array1[BME680_P10_REG]);
  
  /***********************************
   ** Humidity related coefficients **
   **********************************/
  _H1  = (uint16_t) (((uint16_t) coeff_array2[BME680_H1_MSB_REG] << BME680_HUM_REG_SHIFT_VAL)	|
         (coeff_array2[BME680_H1_LSB_REG]&BME680_BIT_H1_DATA_MSK));
  _H2  = (uint16_t) (((uint16_t) coeff_array2[BME680_H2_MSB_REG] << BME680_HUM_REG_SHIFT_VAL) |
         ((coeff_array2[BME680_H2_LSB_REG])>>BME680_HUM_REG_SHIFT_VAL));
  _H3  = (int8_t)   coeff_array2[BME680_H3_REG];
  _H4  = (int8_t)   coeff_array2[BME680_H4_REG];
  _H5  = (int8_t)   coeff_array2[BME680_H5_REG];
  _H6  = (uint8_t)  coeff_array2[BME680_H6_REG];
  _H7  = (int8_t)   coeff_array2[BME680_H7_REG];
  
  /*************************************
   ** Gas heater related coefficients **
   ************************************/
  _G1  = (int8_t)   coeff_array2[BME680_GH1_REG];
  _G2  = (int16_t)  (CONCAT_BYTES(coeff_array2[BME680_GH2_MSB_REG], coeff_array2[BME680_GH2_LSB_REG]));
  _G3  = (int8_t)   coeff_array2[BME680_GH3_REG];

  uint8_t temp_var1 = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_ADDR_RES_HEAT_RANGE_ADDR);
  _res_heat_range = ((temp_var1 & BME680_RHRANGE_MSK) / 16);
  
  uint8_t temp_var2 = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_ADDR_RES_HEAT_VAL_ADDR);
  _res_heat_val = (int8_t) temp_var2;
  
  uint8_t temp_var3 = TWI_getRegisterByte(BME680_SLAVE_ADDRESS, BME680_ADDR_RANGE_SW_ERR_ADDR);
  _range_sw_error = ((int8_t) temp_var3 & (int8_t) BME680_RSERROR_MSK) / 16;
}

/**
 * @brief   sets the oversampling mode for the sensor
 * @details See enumerated "sensorTypes" for a list of values. Set to a valid oversampling rate as defined in the 
 *          enumerated type oversamplingTypes. If either value is out of range or another error occurs then the 
 *          return value is false.
 * @param sensor Enumerated sensor type
 * @param sampling Sampling rate
 * @return "true" if successful otherwise false
 */
bool BME680::setOversampling(const uint8_t sensor, const uint8_t sampling) 
{
  if (sensor>=UnknownSensor || sampling>=UnknownOversample) return(false); // return error if out of range
  uint8_t tempRegister;
  switch (sensor) 
  {
    case HumiditySensor :
    {
      tempRegister = readBME680Byte(BME680_CONTROL_HUMIDITY_REG) & B11111000;  // Get contents, mask bits 7-3
      uint8_t valH = (uint8_t)(tempRegister|sampling);
      TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CONTROL_HUMIDITY_REG, valH);// Update humidity bits 2:0
      break;
    } // of HumiditySensor
    case PressureSensor : 
    {
      tempRegister = readBME680Byte(BME680_CONTROL_MEASURE_REG) & B11100011; // Get contents, mask unused bits
      uint8_t valP = (uint8_t)(tempRegister|(sampling << BME680_PRES_OVER_SAMPLING_OFFSET));
      TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CONTROL_MEASURE_REG, valP); // Update pressure bits
      break;
    } // of PressureSensor
    case TemperatureSensor : 
    {
      tempRegister = readBME680Byte(BME680_CONTROL_MEASURE_REG) & B00011111; // Get contents, mask bits 4-0
      uint8_t valT = (uint8_t)(tempRegister|(sampling << BME680_TEMP_OVER_SAMPLING_OFFSET));
      TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CONTROL_MEASURE_REG, valT);// Update humidity bits 7:5
      break;
    } // of TemperatureSensor
    default: return(false); // Return an error if no match
  } // of switch the sensor type
  return(true);
} // of method setOversampling()

/**
 * @brief   Set or return the current IIR filter setting
 * @details When called with no parameters returns the current IIR Filter setting, otherwise when called with one 
 *          parameter will set the IIR filter value and return the new setting
 * @param iirFilterSetting New setting
 * @return   IIR Filter setting
 */
uint8_t BME680::setIIRFilter(const uint8_t iirFilterSetting ) 
{
  uint8_t returnValue = readBME680Byte(BME680_CONFIG_REGISTER);              // Get control register byte
  if (iirFilterSetting==UINT8_MAX) return((returnValue>>2)&B00000111); // return the current setting
  returnValue = returnValue&B11100011;                                 // Get control reg, mask IIR bits
  returnValue |= (iirFilterSetting&B00000111)<<2;                      // use 3 bits of iirFilterSetting
  TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CONFIG_REGISTER,returnValue);                         // Write new control register value
  returnValue = (returnValue>>2)&B00000111;                            // Extract IIR filter setting
  return(returnValue);                                                 // Return IIR Filter setting
} // of method setIIRFilter()

/**
 * @brief   Returns the most recent temperature, humidity and pressure readings
 * @param temp  Temperature reading
 * @param hum   Humidity reading
 * @param press Pressure reading
 * @param gas   Gas reading
 * @param waitSwitch Optional switch that, when set to "true" will not return until reading is finished
 */
void BME680::getSensorData(int32_t &temp, int32_t &hum, int32_t &press, int32_t &gas, const bool waitSwitch )
{
  readSensors(waitSwitch); // Get compensated data from BME680
  temp  = _Temperature;    // Copy global variables to parameters
  hum   = _Humidity;
  press = _Pressure;
  gas   = _Gas;
} // of method getSensorData()

/**
 * @brief   reads all 4 sensor values from the registers in one operation and then proceeds to convert the raw 
 *          temperature, pressure and humidity readings into standard metric units
 * @details The formula is written in the BME680's documentation but the math used below was taken from Adafruit's 
 *          Adafruit_BME680_Library at https://github.com/adafruit/Adafruit_BME680. I think it can be refactored 
 *          into more efficient code at some point in the future, but it does work correctly.
 * @param waitSwitch Optional switch that, when set to "true" will not return until reading is finished
 */
void BME680::readSensors(const bool waitSwitch) 
{
  /*! Lookup table for the possible gas range values */
const  uint32_t lookupTable1[16] = {
  UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2147483647), 
  UINT32_C(2126008810), UINT32_C(2147483647), UINT32_C(2130303777), UINT32_C(2147483647), UINT32_C(2147483647),
  UINT32_C(2143188679), UINT32_C(2136746228), UINT32_C(2147483647), UINT32_C(2126008810), UINT32_C(2147483647), 
  UINT32_C(2147483647) 
};
const uint32_t lookupTable2[16]  = {
  UINT32_C(4096000000), UINT32_C(2048000000), UINT32_C(1024000000), UINT32_C(512000000), UINT32_C(255744255),  
  UINT32_C(127110228) , UINT32_C(64000000),   UINT32_C(32258064),   UINT32_C(16016016),  UINT32_C(8000000),
  UINT32_C(4000000),    UINT32_C(2000000),    UINT32_C(1000000),    UINT32_C(500000),    UINT32_C(250000),
  UINT32_C(125000) 
};
  uint8_t buff[15],gas_range,status=0;                     // declare array for registers
  int64_t var1, var2, var3, var4, var5, var6, temp_scaled; // Work variables
  uint32_t adc_temp, adc_pres;                             // Raw ADC temperature and pressure
  uint16_t adc_hum, adc_gas_res;                           // Raw ADC humidity and gas
  if (waitSwitch) waitForReadings();                       // Don't return until readings done
  TWI_getRegisterBytes(BME680_SLAVE_ADDRESS, BME680_STATUS_REGISTER, 15, &buff[0]); // read all 15 bytes in one go
  adc_pres    = (uint32_t)(((uint32_t) buff[2]*4096)|((uint32_t)buff[3]*16)| ((uint32_t)buff[4]/16)); // put the 3 bytes of Pressure
  adc_temp    = (uint32_t)(((uint32_t) buff[5]*4096)|((uint32_t)buff[6]*16)| ((uint32_t)buff[7]/16)); // put the 3 bytes of Temperature
  adc_hum     = (uint16_t)(((uint32_t) buff[8]*256)|(uint32_t)buff[9]);                               // put the 2 bytes of Humidity
  adc_gas_res = (uint16_t)((uint32_t) buff[13]*4|(((uint32_t)buff[14])/64));                          // put the 2 bytes of Gas
  gas_range   = buff[14] & 0X0F; // Retrieve the range
  status |= buff[14] & 0X20;     // See if the gas range is valid
  status |= buff[14] & 0X10;     // and the measurement is valid

/*************************
 ** Compute temperature **
 ************************/
  var1         = ((int32_t)adc_temp>>3)-((int32_t)_T1<<1); // Perform calibration/adjustment
  var2         = (var1*(int32_t)_T2)>>11;                  // of Temperature values according
  var3         = ((var1>>1)*(var1>>1))>>12;                // to formula defined by Bosch
  var3         = ((var3)*((int32_t)_T3<<4))>>14;
  _tfine       = (int32_t)(var2+var3);
  _Temperature = (int16_t)(((_tfine*5)+128)>>8);

/**********************
 ** Compute pressure **
 *********************/
	var1      = (((int32_t)_tfine) >> 1) - 64000;
	var2      = ((((var1 >> 2)*(var1 >> 2)) >> 11)*(int32_t)_P6) >> 2;
	var2      = var2 + ((var1 * (int32_t)_P5) << 1);
	var2      = (var2 >> 2) + ((int32_t)_P4 << 16);
	var1      = (((((var1>>2)*(var1>>2))>>13)*((int32_t)_P3<<5))>>3)+(((int32_t)_P2*var1)>>1);
	var1      = var1 >> 18;
	var1      = ((32768 + var1) * (int32_t)_P1) >> 15;
	_Pressure = 1048576 - adc_pres;
	_Pressure = (int32_t)((_Pressure - (var2 >> 12)) * ((uint32_t)3125));
	var4      = (1UL << 31);
	if (_Pressure >= var4)
    _Pressure = ((_Pressure / (uint32_t)var1) << 1);
	else
    _Pressure = ((_Pressure << 1) / (uint32_t)var1);
	var1 = ((int32_t)_P9*(int32_t)(((_Pressure>>3)*(_Pressure>>3))>>13))>>12;
	var2 = ((int32_t)(_Pressure >> 2) * (int32_t)_P8) >> 13;
	var3 = ((int32_t)(_Pressure >> 8) * (int32_t)(_Pressure >> 8) *	(int32_t)(_Pressure >> 8) * (int32_t)_P10) >> 17;
	_Pressure = (int32_t)(_Pressure)+((var1+var2+var3+((int32_t)_P7<<7))>>4);

/**********************
 ** Compute humidity **
 *********************/
	temp_scaled = (((int32_t) _tfine * 5) + 128) >> 8;
	var1        = (int32_t)(adc_hum-((int32_t)((int32_t)_H1*16)))
                -(((temp_scaled*(int32_t)_H3)/((int32_t)100))>>1);
	var2        = ((int32_t)_H2*(((temp_scaled*(int32_t)_H4)/
                ((int32_t) 100))+(((temp_scaled*((temp_scaled*
                (int32_t)_H5)/((int32_t)100)))>>6)/((int32_t)100))+
                (int32_t) (1 << 14))) >> 10;
	var3        = var1 * var2;
	var4        = (int32_t) _H6 << 7;
	var4        = ((var4)+((temp_scaled*(int32_t)_H7)/((int32_t)100)))>>4;
	var5        = ((var3 >> 14) * (var3 >> 14)) >> 10;
	var6        = (var4 * var5) >> 1;
	_Humidity   = (((var3 + var6) >> 10) * ((int32_t) 1000)) >> 12;
	if (_Humidity > 100000) /* Cap at 100%rH */
	  _Humidity = 100000;
	else if (_Humidity < 0)
	  _Humidity = 0;

/*****************
 ** Compute gas **
 ****************/
	uint64_t uvar2;
	var1 = (int64_t)((1340+(5*(int64_t)_range_sw_error))*
	((int64_t) lookupTable1[gas_range])) >> 16;
	uvar2 = (((int64_t)((int64_t)adc_gas_res<<15)-(int64_t)(16777216))+var1);
	var3 = (((int64_t) lookupTable2[gas_range] * (int64_t) var1) >> 9);
	_Gas = (uint32_t) ((var3 + ((int64_t) uvar2 >> 1)) / (int64_t) uvar2);
  uint8_t workRegister = readBME680Byte(BME680_CONTROL_MEASURE_REG);   // Read the control measure
  uint8_t valToSet = (uint8_t)(workRegister|1);
  TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CONTROL_MEASURE_REG, valToSet); // Trigger start of next measurement
} // of method readSensors()

/**
 * @brief   Only returns once a measurement on the BME680 has completed
 */
void BME680::waitForReadings() 
{
  while ((readBME680Byte(BME680_STATUS_REGISTER)&B00100000)!=0); // Loop until readings bit is cleared by BME680
}

/**
 * @brief    sets the gas measurement target temperature and heating time
 * @param GasTemp  Target temperature in Celsius
 * @param GasMillis Milliseconds to turn on heater
 * @return Always returns "true"
 */
bool BME680::setGas(uint16_t GasTemp,  uint16_t GasMillis) 
{
  uint8_t gasRegister = readBME680Byte(BME680_CONTROL_GAS_REGISTER2); // Read current register values
  if ( GasTemp==0 || GasMillis==0 ) 
  {
    // If either input variable is zero //
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CONTROL_GAS_REGISTER1,(uint8_t)B00001000);               // Turn off gas heater
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CONTROL_GAS_REGISTER2,(uint8_t)(gasRegister&B11101111)); // Turn off gas measurements
  }
  else 
  {
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CONTROL_GAS_REGISTER1,(uint8_t)0); // Turn off heater bit to turn on
  	uint8_t heatr_res;
	  int32_t var1,var2,var3,var4,var5,heatr_res_x100;
	  if (GasTemp < 200) GasTemp = 200;	else if (GasTemp > 400) GasTemp = 400; // Clamp temperature to min/max
    var1 = (((int32_t)(_Temperature/100)*_H3)/1000)*256;
    var2 = (_H1+784)*(((((_H2+154009)*GasTemp*5)/100)+3276800)/10);
    var3 = var1 + (var2 / 2);
    var4 = (var3 / (_res_heat_range+4));
    var5 = (131 * _res_heat_val) + 65536;
    heatr_res_x100 = (int32_t) (((var4 / var5) - 250) * 34);
    heatr_res = (uint8_t) ((heatr_res_x100 + 50) / 100);
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_RES_HEAT_0_REG,heatr_res);
    uint8_t factor = 0;
    uint8_t durval;
    if (GasMillis >= 0xfc0) durval = 0xff; // Max duration
  	else 
    {
  	  while (GasMillis > 0x3F) 
      {
    	  GasMillis = GasMillis / 4;
    	  factor += 1;
    	} // of while loop
    	durval = (uint8_t) (GasMillis + (factor * 64));
  	} // of if-then-else duration exceeds max
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CONTROL_GAS_REGISTER1, (uint8_t)0); // then turn off gas heater
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_GAS_WAIT_0_REG, durval);
    TWI_setRegisterByte(BME680_SLAVE_ADDRESS, BME680_CONTROL_GAS_REGISTER2, (uint8_t)(gasRegister|B00010000));
  } // of if-then-else turn gas measurements on or off
  return true;
}