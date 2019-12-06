#include "myBME680.h" // Include the BME680 Sensor library
#include "myTWI.h"
#include "myUSART.h"
#include "myHelperFunctions.h"

BME680 myBME680; // Create instance of the BME680 class

float altitude(const float seaLevel = 1013.25);


/*!
    @brief    Arduino method called once at startup to initialize the system
    @details  This is an Arduino IDE method which is called first upon boot or restart. It is only called one time
              and then control goes to the main "loop()" method, from which control never returns
    @return   void
*/
void setup()
{

  TWI_init();
  USART_init();
  myBME680.init();

  USART_writeStringLn(" Starting I2CDemo example program for BME680");
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
} // of method setup()

/*!
    @brief    Arduino method for the main program loop
    @details  This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating. 
    @return   void
*/
void loop() 
{
  static int32_t temperature, humidity, pressure, gas;     // Variable to store readings
  myBME680.getSensorData(temperature,humidity,pressure,gas); // Get most recent readings
  Serial.print(temperature/100.0,2);                       // Temperature in deci-degrees
  USART_writeString("\xC2\xB0 ");                          // Representation of the ° symbol
  Serial.print(humidity/1000.0,2);                         // Humidity in milli-percent
  USART_writeString("%Hum ");
  Serial.print(pressure/100.0,2);                          // Pressure in Pascals
  USART_writeString("hPa ");
  Serial.print(altitude(),2);
  USART_writeString("m ");
  Serial.print(gas/100.0,2);
  USART_writeStringLn("mOhm");
  delay(5000);
} // of method loop()


/*!
* @brief     This converts a pressure measurement into a height in meters
* @details   The corrected sea-level pressure can be passed into the function if it is know, otherwise the standard 
*            atmospheric pressure of 1013.25hPa is used (see https://en.wikipedia.org/wiki/Atmospheric_pressure
* @param[in] seaLevel Sea-Level pressure in millibars
* @return    floating point altitude in meters.
*/
float altitude(const float seaLevel = 1013.25) 
{
  static float altitudeResult;
  int32_t temp, hum, press, gas;
  
  // Get all sensor values (including pressure) from BME680
  myBME680.getSensorData(temp,hum,press,gas);

  // calculate altidude
  altitudeResult = 44330.0*(1.0-pow(((float)press/100.0)/seaLevel,0.1903)); // Convert into altitude in meters
  return(altitudeResult);
} // of method altitude()