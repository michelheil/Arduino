#include "myGlobalDefines.h"
#include "myBME680.h" // Include the BME680 Sensor library

#include "myUSART.h"

/**
 * @brief     This converts a pressure measurement into height in meters
 * @details   The sea-level pressure is set to 1013.25hPa
 * @param pressure Pressure value in hPa
 * @return    altitude in meters as float
 */
float altitude(int32_t pressure = 977.20);

BME680 myBME680; /**< Create instance of the BME680 class */


/**
 * @brief    Arduino method called once at startup to initialize the system
 * @details  This is an Arduino IDE method which is called first upon boot or restart. It is only called one time
 *           and then control goes to the main "loop()" method, from which control never returns
 * @return   void
 */
void setup()
{
  myBME680.init();

  USART_init();

  USART_writeStringLn("Starting I2CDemo example program for BME680");
  USART_writeStringLn("- Initializing BME680 sensor");
  USART_writeStringLn("- Setting 16x oversampling for all sensors");
  myBME680.setOversampling(TemperatureSensor,Oversample16); // Use enumerated type values
  myBME680.setOversampling(HumiditySensor,   Oversample16);
  myBME680.setOversampling(PressureSensor,   Oversample16);
  USART_writeStringLn("- Setting IIR filter to a value of 4 samples");
  myBME680.setIIRFilter(IIR4);
  USART_writeStringLn("- Setting gas measurement to 320\xC2\xB0 for 150ms");
  myBME680.setGas(320,150); // 320°c for 150 milliseconds
  USART_newLine();
}

/**
 * @brief    Arduino method for the main program loop
 * @details  This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating. 
 * @return   void
 */
void loop() 
{
  static int32_t temperature, humidity, pressure, gas; // variables to store readings
  float tempDisplay, humidDisplay, pressDisplay, gasDisplay; // variables for USART output

  myBME680.getSensorData(temperature, humidity, pressure, gas);
  
  delay(1000);
  
  tempDisplay = temperature/100.0; // Temperature in deci-degrees
  USART_writeFloat(tempDisplay);
  USART_writeString("\xC2\xB0 "); // Representation of the ° symbol
  
  humidDisplay = humidity/1000.0; // Humidity in milli-percent
  USART_writeFloat(humidDisplay);
  USART_writeString("%Hum ");

  pressDisplay = pressure/100.0; // Pressure in Pascals
  USART_writeFloat(pressDisplay);
  USART_writeString("hPa ");

  USART_writeFloat(altitude(pressure));
  USART_writeString("m ");
  
  gasDisplay = gas/100.0;
  USART_writeFloat(gasDisplay);  
  USART_writeStringLn("mOhm");
}


// Calculate altidude based on pressure
float altitude(int32_t pressure = 977.20) 
{
  const float seaLevel = 1013.25;

  static float altitudeResult = 44330.0 * (1.0 - pow( ((float)pressure/100.0) / seaLevel , 0.1903));
  return(altitudeResult);
}