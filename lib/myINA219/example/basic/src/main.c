/*
 * Example for using library INA219
 * Created: 26.12.2019 22:23:18
 * Author : Michael
 */ 

#include "myGlobalDefines.h"
#include <util/delay.h>

#include "myTWI.h"
#include "myUSART.h"
#include "myINA219.h"
#include "myLOG.h"

#define INA219_SLAVE_ADDRESS 0x40

#define INA219_CONFIGURATION_REGISTER 0x00
#define INA219_SHUNT_VOLTAGE_REGISTER 0x01
#define INA219_BUS_VOLTAGE_REGISTER   0x02
#define INA219_POWER_REGISTER         0x03
#define INA219_CURRENT_REGISTER       0x04
#define INA219_CALIBRATION_REGISTER   0x05

/**
 * Calibration Register
 * 
 * RST - Bit 15 Setting this bit to '1' generates a system reset that is the same as power-on reset. 
 * Resets all registers to default values; this bit self-clears.
 * 
 * BRNG - Bus Voltage Range
Bit 13 0 = 16V FSR
1 = 32V FSR (default value)
 * 
 */
#define INA219_CONFIG_BVOLTAGERANGE_16V 0x0000 // BRNG (Bit 13) Bus Voltage Range
#define INA219_CONFIG_GAIN_1_40MV 0x0000 // PG (Bit 12..11) PGA (Shunt Voltage Only)
#define INA219_CONFIG_BADCRES_12BIT 0x0180 // BADC (Bit 10..7) Bus ADC Resolution/Averaging
#define INA219_CONFIG_SADCRES_12BIT_1S_532US 0x0018 // SADC (Bit 6..3) Shunt ADC Resolution/Averaging
#define INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS 7 // MODE (Bit 2..0)

void INA219_init(void); // public
void INA219_setCalibration_16V_400mA(); // private
int16_t INA219_getBusVoltageRaw(void); // private
int16_t INA219_getShuntVoltageRaw(void); // private
int16_t INA219_getCurrentRaw(void); // private
int16_t INA219_getPowerRaw(void); // private
float INA219_getShuntVoltage_mV(void); // public
float INA219_getBusVoltage_V(void); // public
float INA219_getCurrent_mA(void); // public
float INA219_getPower_mW(void); // public

volatile uint16_t ina219_calValue = 8192;
volatile uint32_t ina219_currentDivider_mA = 20; // Current LSB = 50uA per bit (1000/50 = 20)
volatile float ina219_powerMultiplier_mW = 1.0f;

int main(void)
{
  // Initialize USART
  USART_init();

  // Initialize I2C communication
  TWI_init();

  // Initialize INA219 device by setting calibration
  INA219_init();

  while(1)
  {
    _delay_ms(2000);
    USART_newLine();
  }
}


// Initialize
void INA219_init(void)
{
  INA219_setCalibration_16V_400mA();
}

void INA219_setCalibration_16V_400mA(void) 
{
  // Calibration which uses the highest precision for
  // current measurement (0.1mA), at the expense of
  // only supporting 16V at 400mA max.

  // VBUS_MAX = 16V
  // VSHUNT_MAX = 0.04          (Assumes Gain 1, 40mV)
  // RSHUNT = 0.1               (Resistor value in ohms)

  // 1. Determine max possible current
  // MaxPossible_I = VSHUNT_MAX / RSHUNT
  // MaxPossible_I = 0.4A

  // 2. Determine max expected current
  // MaxExpected_I = 0.4A

  // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
  // MinimumLSB = MaxExpected_I/32767
  // MinimumLSB = 0.0000122              (12uA per bit)
  // MaximumLSB = MaxExpected_I/4096
  // MaximumLSB = 0.0000977              (98uA per bit)

  // 4. Choose an LSB between the min and max values
  //    (Preferrably a roundish number close to MinLSB)
  // CurrentLSB = 0.00005 (50uA per bit)

  // 5. Compute the calibration register
  // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
  // Cal = 8192 (0x2000)

  uint16_t ina219_calValue = 8192;

  // 6. Calculate the power LSB
  // PowerLSB = 20 * CurrentLSB
  // PowerLSB = 0.001 (1mW per bit)

  // 7. Compute the maximum current and shunt voltage values before overflow
  //
  // Max_Current = Current_LSB * 32767
  // Max_Current = 1.63835A before overflow
  //
  // If Max_Current > Max_Possible_I then
  //    Max_Current_Before_Overflow = MaxPossible_I
  // Else
  //    Max_Current_Before_Overflow = Max_Current
  // End If
  //
  // Max_Current_Before_Overflow = MaxPossible_I
  // Max_Current_Before_Overflow = 0.4
  //
  // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
  // Max_ShuntVoltage = 0.04V
  //
  // If Max_ShuntVoltage >= VSHUNT_MAX
  //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
  // Else
  //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
  // End If
  //
  // Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
  // Max_ShuntVoltage_Before_Overflow = 0.04V

  // 8. Compute the Maximum Power
  // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
  // MaximumPower = 0.4 * 16V
  // MaximumPower = 6.4W

  // Set multipliers to convert raw current/power values
  uint32_t ina219_currentDivider_mA = 20; // Current LSB = 50uA per bit (1000/50 = 20)
  float ina219_powerMultiplier_mW = 1.0f; // Power LSB = 1mW per bit

  // Set Calibration register to calculated value (ina219_calValue) above
  TWI_setRegisterTwoBytes(INA219_SLAVE_ADDRESS, INA219_CALIBRATION_REGISTER, ina219_calValue);

  _delay_ms(5);

  // Set Config register to take into account the settings above
  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V |
                    INA219_CONFIG_GAIN_1_40MV | INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
  TWI_setRegisterTwoBytes(INA219_SLAVE_ADDRESS, INA219_CONFIGURATION_REGISTER, config);
}

/**
 * @brief  Gets the raw bus voltage (16-bit signed integer, so +-32767)
 * @return the raw bus voltage reading
 */
int16_t INA219_getBusVoltageRaw(void)
{
  // read raw bytes
  uint8_t raw[2];
  TWI_getRegisterBytes(INA219_SLAVE_ADDRESS, INA219_BUS_VOLTAGE_REGISTER, 2, &raw[0]);

  // concatenate raw bytes
  uint16_t value = CONCATENATE_BYTES(raw[0], raw[1]);

  // Shift to the right 3 to drop CNVR and OVF and multiply by LSB
  return (int16_t)((value >> 3) * 4);
}

/**
 * @brief  Gets the raw shunt voltage (16-bit signed integer, so +-32767)
 * @return the raw shunt voltage reading
 */
int16_t INA219_getShuntVoltageRaw(void)
{
  // read raw bytes
  uint8_t raw[2];
  TWI_getRegisterBytes(INA219_SLAVE_ADDRESS, INA219_SHUNT_VOLTAGE_REGISTER, 2, &raw[0]);

  // concatenate raw bytes
  uint16_t value = CONCATENATE_BYTES(raw[0], raw[1]);

  return (int16_t)value;
}

/**
 * @brief  Gets the raw current value (16-bit signed integer, so +-32767)
 * @return the raw current reading
 */
int16_t INA219_getCurrentRaw(void)
{
  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  TWI_setRegisterByte(INA219_SLAVE_ADDRESS, INA219_CALIBRATION_REGISTER, ina219_calValue);

  // Now we can safely read the CURRENT register!
  uint8_t raw[2];
  TWI_getRegisterBytes(INA219_SLAVE_ADDRESS, INA219_CURRENT_REGISTER, 2, &raw[0]);

  // concatenate raw bytes
  uint16_t value = CONCATENATE_BYTES(raw[0], raw[1]);

  return (int16_t)value;
}

/**
 * @brief  Gets the raw power value (16-bit signed integer, so +-32767)
 * @return raw power reading
 */
int16_t INA219_getPowerRaw(void)
{
  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  TWI_setRegisterByte(INA219_SLAVE_ADDRESS, INA219_CALIBRATION_REGISTER, ina219_calValue);

  // Now we can safely read the POWER register!
  uint8_t raw[2];
  TWI_getRegisterBytes(INA219_SLAVE_ADDRESS, INA219_POWER_REGISTER, 2, &raw[0]);

  // concatenate raw bytes
  uint16_t value = CONCATENATE_BYTES(raw[0], raw[1]);

  return (int16_t)value;
}

/**
 * @brief  Gets the shunt voltage in mV (so +-327mV)
 * @return the shunt voltage converted to millivolts
 */
float INA219_getShuntVoltage_mV(void) {
  int16_t value = INA219_getShuntVoltageRaw();
  return value * 0.01;
}

/**
 * @brief  Gets the shunt voltage in volts
 * @return the bus voltage converted to volts
 */
float INA219_getBusVoltage_V(void) {
  int16_t value = INA219_getBusVoltageRaw();
  return value * 0.001;
}

/**
 * @brief  Gets the current value in mA, taking into account the
 *         config settings and current LSB
 * @return the current reading convereted to milliamps
 */
float INA219_getCurrent_mA(void) {
  float valueDec = INA219_getCurrentRaw();
  valueDec /= ina219_currentDivider_mA;
  return valueDec;
}

/**
 * @brief  Gets the power value in mW, taking into account the
 *         config settings and current LSB
 * @return power reading converted to milliwatts
 */
float INA219_getPower_mW(void) {
  float valueDec = INA219_getPowerRaw();
  valueDec *= ina219_powerMultiplier_mW;
  return valueDec;
}


