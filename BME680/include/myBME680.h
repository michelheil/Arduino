/*! @file myBME680.h
 * @mainpage Library to read sensor data from Bosch BME680 through I2C from Arduino UNO
 * @section myBME680 Description
 * @brief Sensor for:
 *        Temperature: -40 - +85 Celcius
 *        Humidity: 0 - 100% r.H. (relative Humidity)
 *        Pressure: 300 - 1100 hPa (hecto Pascal)
Data Sheet: https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME680-DS001.pdf
Bosch BME680_Driver: https://github.com/BoschSensortec/BME680_driver
*/

#ifndef MYBME680_H_
#define MYBME680_H_

#include "Arduino.h" // Arduino data type definitions
#include <util/twi.h> // required for TW_READ and TW_WRITE

// BME680 I2C Slave Address
// The 7-bit device address is 111011x. The 6 MSB bits are fixed. 
// The last bit is changeable by SDO value and can be changedduring operation. 
// Connecting SDO to GND results in slave address 1110110 (0x76);
// connection it to VDDIO results in slave address 1110111 (0x77)
#define BME680_SLAVE_ADDRESS                0x77 //0x76
#define BME680_SLA_W                        ((BME680_SLAVE_ADDRESS << 1) | TW_WRITE) // shifted by one as the address is stored in the higher 7-bits
#define BME680_SLA_R                        ((BME680_SLAVE_ADDRESS << 1) | TW_READ)  // TW_WRITE (=0) and TW_READ (=1) are defined in util/twi.h

// Software Reset
// Writing 0xB6 to this register initiates a soft-reset procedure, which has the same effect like power-on reset.
#define BME680_RESET_REG                    0xE0 // for I2C
#define BME680_RESET_SOFT                   0xB6

// Power Mode; TPHG = Temperature, Pressure, Humidity and Gas
//
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
#define BME680_CONTROL_MEASURE_REG          0x74 // contains osrs_t<2:0>, osrs_p<2:0>, mode<1:0>
#define BME680_TEMP_OVER_SAMPLING_OFFSET    5 // reflecting the bit positions of osrs_t<2:0> in CONTROL_MEASURE_REG
#define BME680_PRES_OVER_SAMPLING_OFFSET    2 // reflecting the bit positions of osrs_p<2:0> in CONTROL_MEASURE_REG
#define BME680_CONTROL_HUMIDITY_REG         0x72  // contains osrs_h<2:0>

#define BME680_POWER_MODE_SLEEP             0x00 // No measurements are performed; Minimal power consumption
#define BME680_POWER_MODE_FORCED            0x01 // Single TPHG cycle is performed; Sensor automatically returns to sleep mode afterwards; Gas sensor heater only operates during gas measurement
#define BME680_OVER_SAMPLING_0X             0b000 // measurement skipped
#define BME680_OVER_SAMPLING_1X             0b001
#define BME680_OVER_SAMPLING_2X             0b010
#define BME680_OVER_SAMPLING_4X             0b011
#define BME680_OVER_SAMPLING_8X             0b100
#define BME680_OVER_SAMPLING_16X            0b101

// Next, the user shall set at least one gas sensor hot plate temperature set-point and heating duration.
// Heating Duration
// Set gas_wait_0<7:0> to 0x59 to select 100 ms heat up duration
#define BME680_GAS_WAIT_0_REG               0x64
#define BME680_GAS_WAIT_MULT_FACTOR_OFFSET  6
#define BME680_GAS_WAIT_MULT_FACTOR_1X      0b00
#define BME680_GAS_WAIT_MULT_FACTOR_4X      0b01
#define BME680_GAS_WAIT_MULT_FACTOR_16X     0b10
#define BME680_GAS_WAIT_MULT_FACTOR_64X     0b11

// Heater Resistance
// Set the corresponding heater set-point by writing the target heater resistance to res_heat_0<7:0>
#define BME680_RES_HEAT_0_REG               0x5A

#define BME680_PAR_G1_REG                   0xED
#define BME680_PAR_G2_REG_LSB               0xEB
#define BME680_PAR_G2_REG_MSB               0xEC
#define BME680_PAR_G3_REG                   0xEE
#define BME680_RES_HEAT_RANGE_REG           0x02 // bits <5:4>
#define BME680_RES_HEAT_VAL_REG             0x00


// Set nb_conv<3:0> to 0x0 to select the previously defined heater settings
#define BME680_CTRL_GAS_1_REG               0x71
#define BME680_HEATER_PROFILE_0             0b0000 // I am using profile 0
#define BME680_HEATER_PROFILE_1             0b0001
// ...
#define BME680_HEATER_PROFILE_9             0b1001

// Set run_gas_l to 1 to enable gas measurements
#define BME680_GAS_MEASUREMENT_OFFSET       4
#define BME680_GAS_MEASUREMENT_ENABLE       1
#define BME680_GAS_MEASUREMENT_DISABLE      0


// Measure Registers
#define BME680_PRESS_MSB_REG                0x1F
#define BME680_PRESS_LSB_REG                0x20
#define BME680_PRESS_XLSB_REG               0x21
#define BME680_TEMP_MSB_REG                 0x22
#define BME680_TEMP_LSB_REG                 0x23
#define BME680_TEMP_XLSB_REG                0x24
#define BME680_HUM_MSB_REG                  0x25
#define BME680_HUM_LSB_REG                  0x26
#define BME680_GAS_R_MSB                    0x2A
#define BME680_GAS_R_LSB                    0x2B

#define CONCAT_BYTES(msb, lsb) (((uint16_t)msb << 8) | (uint16_t)lsb) // combine msb and lsb

#define BME680_STATUS_REGISTER              0x1D // Device status register
#define BME680_CONFIG_REGISTER              0x75 // Configuration register

#define BME680_CONTROL_GAS_REGISTER1        0x70 // Gas control register on/off
#define BME680_CONTROL_GAS_REGISTER2        0x71 // Gas control register settings

#define BME680_CHIPID_REGISTER              0xD0 // Chip-Id register
#define BME680_CHIPID                       0x61 // Hard-coded value 0x61 for BME680


/*****************************************************************************************************************
 ** Declare enumerated types used in the class                                                                   **
*****************************************************************************************************************/
/*! @brief  Enumerate the sensor type */
enum sensorTypes       {TemperatureSensor,HumiditySensor,PressureSensor,GasSensor,UnknownSensor};
/*! @brief  Enumerate the Oversampling types */
enum oversamplingTypes {SensorOff,Oversample1,Oversample2,Oversample4,Oversample8,Oversample16,UnknownOversample };
/*! @brief  Enumerate the iir filter types */
enum iirFilterTypes    {IIROff,IIR2,IIR4,IIR8,IIR16,IIR32,IIR64,IIR128,UnknownIIR };

 /*!
* @class BME680
* @brief Main BME680 class for the temperature, humidity, pressure, gas sensor
*/
  class BME680 
  {
    public:
      BME680(); // constructor
      ~BME680(); // de-constructor
      
      void     init();                                                                  // initialization code
      bool     setOversampling(const uint8_t sensor, const uint8_t sampling);           // Set enum sensorType Oversampling
      bool     setGas(uint16_t GasTemp, uint16_t GasMillis);                            // Gas heating temperature and time
      uint8_t  setIIRFilter(const uint8_t iirFilterSetting=UINT8_MAX);                  // Set IIR Filter and return value
      void     getSensorData(int32_t &temp, int32_t &hum,                               // get most recent readings
                             int32_t &press, int32_t &gas,
                             const bool waitSwitch = true);
      void     reset();                                                                 // Reset the BME680

    private:
      uint8_t  readBME680Byte(const uint8_t addr);                                      // Read byte from register address
      void     readSensors(const bool waitSwitch);                                      // read the registers in one burst
      void     waitForReadings();                                                       // Wait for readings to finish
      void     getCalibration();                                                        // Load calibration from registers
      uint8_t  _I2CAddress         = 0;                                                 // Default is no I2C address known
      uint8_t  _H6,_P10,_res_heat_range;                                                // unsigned configuration variables
      int8_t   _H3,_H4,_H5,_H7,_G1,_G3,_T3,_P3,_P6,_P7,_res_heat_val,_range_sw_error;   // signed configuration variables
      uint16_t _H1,_H2,_T1,_P1;                                                         // unsigned 16bit configuration variables
      int16_t  _G2,_T2,_P2,_P4,_P5,_P8,_P9;                                             // signed 16bit configuration variables
      int32_t  _tfine,_Temperature,_Pressure,_Humidity,_Gas;                            // signed 32bit configuratio variables
  }; // of BME680 class definition

#endif /* MYBME680_H_ */