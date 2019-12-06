## Sensor Description
BME680 - Low power gas, pressure, temperature & humidity sensor

The BME680 is a digital 4-in-1 sensor with gas, humidity, pressure and temperature measurement based on
proven sensing principles. The sensor module is housed in an extremely compact metal-lid LGA package with a
footprint of only 3.0 × 3.0 mm2 with a maximum height of 1.00 mm (0.93 ± 0.07 mm). Its small dimensions and its
low power consumption enable the integration in battery-powered or frequency-coupled devices, such as
handsets or wearables.

Key features:
* Package
   - 3.0 mm x 3.0 mm x 0.93 mm metal lid LGA
* Digital interface
  - I2C (up to 3.4 MHz) and SPI (3 and 4 wire, up to 10 MHz)
* Supply voltage
  - V DD main supply voltage range: 1.71 V to 3.6 V
  - V DDIO interface voltage range: 1.2 V to 3.6 V 
* Current consumption
  - 2.1 μA at 1 Hz humidity and temperature
  - 3.1 μA at 1 Hz pressure and temperature
  - 3.7 μA at 1 Hz humidity, pressure and temperature
  - 0.09‒12 mA for p/h/T/gas depending on operation mode
  - 0.15 μA in sleep mode
* Operating range
  - -40‒+85 °C, 0‒100% r.H., 300‒1100 hPa
* Individual humidity, pressure and gas sensors can be independently enabled/disabled
* The product is RoHS compliant, halogen-free, MSL1

Key parameters for gas sensor:
* Response time (τ 33−63% )
  - < 1 s (for new sensors)
* Power consumption
  - < 0.1 mA in ultra-low power mode
* Output data processing
  - direct indoor air quality (IAQ) index output

Key parameters for humidity sensor:
* Response time (τ 0−63% )
  - ~8 s
* Accuracy tolerance
  - ±3% r.H.
* Hysteresis
  - ±1.5% r.H.

Key parameters for pressure sensor:
* RMS Noise
  - 0.12 Pa, equiv. to 1.7 cm
* Offset temperature coefficient
  - ±1.3 Pa/K, equiv. to ±10.9 cm at 1 °C temperature change

Source: [Bosch Data Sheet](https://cdn-shop.adafruit.com/product-files/3660/BME680.pdf)

### Link to data sheet
[DataSheet used for my Projects](https://github.com/michelheil/Arduino/lib/myBME680/datasheet)

### Picture of sensor
<x.jpeg>

### Other Links and References
[Bosch BME680 driver on GitHub](https://github.com/BoschSensortec/BME680_driver)


## Library Documentation

### Dependencies
tbd

### Init function
tbd


### APIs
tbd

#### Helper Functions
tbd 


## Example
Motivation and what to do
### Picture of wiring
Fritzing
### Dependencies
Which other utils are required
