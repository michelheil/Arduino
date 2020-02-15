## Sensor Description
The QMC5883L is a multi-chip three-axis magnetic sensor. This
surface -mount, small sized chip has integrated magnetic sensors with signal condition ASIC, targeted for high precision applications such as compassing, navigation and gaming in drone, robot, mobile and personal hand-held devices.

The QMC5883L is based on our state-of-the-art, high resolution,
magneto-resistive technology licensed from Honeywell AMR technology.
Along with custom-designed 16-bit ADC ASIC, it offers the advantages of low noise, high accuracy, low power consumption, offset cancellation and temperature compensation. QMC5883L enables 1° to 2° compass heading accuracy. The I2C serial bus allows for easy interface.

The QMC5883L is in a 3x3x0.9mm3 surface mount 16-pin land grid
array (LGA) package.

### FEATURES
* 3-Axis Magneto-Resistive Sensors in a 3x3x0.9mm3 Land Grid Array Package (LGA), guaranteed to operate over an extended temperature range of -40 °C to +85 °C.
* 16 Bit ADC With Low Noise AMR Sensors Achieves 5 Milli-Gauss Field Resolution
* Wide Magnetic Field Range (±8 Gauss)
* Temperature Compensated Data Output and Temperature Output
* I2C Interface with Standard and Fast Modes.
* Wide Range Operation Voltage (2.16V To 3.6V) and Low Power Consumption (75uA)
* Lead Free Package Construction
* Software And Algorithm Support Available

### BENEFIT
* Small Size for Highly Integrated Products. Signals Have
Been Digitized And Calibrated.
* Enables 1° To 2° Degree Compass Heading Accuracy, Allows for Navigation and LBS Applications
* Maximizes Sensor’s Full Dynamic Range and Resolution
* Automatically Maintains Sensor’s Sensitivity Under Wide Operating Temperature Range
* High-Speed Interfaces for Fast Data Communications. Maximum 200Hz Data Output Rate
* Compatible with Battery Powered Applications
* RoHS Compliance
* Compassing Heading, Hard Iron, Soft Iron, and Auto Calibration Libraries Available


Source: [DataSheet](https://github.com/michelheil/Arduino/lib/myGY271/datasheet)


### Link to data sheet
[DataSheet used for my Projects](https://github.com/michelheil/Arduino/lib/myGY271/datasheet)

### Pictures of sensor
<img src="https://github.com/michelheil/Arduino/blob/master/lib/myGY271/pictures/GY271_Front.jpg" width="250"> <img src="https://github.com/michelheil/Arduino/blob/master/lib/myGY271/pictures/GY271_Back.jpg" width="250">

### Other Links and References



## Library Documentation

### Dependencies
* <avr/io.h>
* <util/delay.h>
* "myTWI.h"


### Init function
```void GY271_init(void)
{
  // Soft Reset
  TWI_setRegisterByte(GY271_SLAVE_ADDRESS, GY271_CONTROL_REGISTER_2, GY271_CR2_SOFT_RESET);
  _delay_ms(1);

  // configuration (oversampling, range, rate, mode)
  TWI_setRegisterByte(GY271_SLAVE_ADDRESS, GY271_CONTROL_REGISTER_1, GY271_CR1_CONTINUOUS_MODE);

  // reset
  TWI_setRegisterByte(GY271_SLAVE_ADDRESS, GY271_SET_RESET_PERIOD_REGISTER, GY271_FBR_RECOMMENDATION);
  _delay_ms(1);
}
```

### APIs
Get raw x,y,z values from device

```struct xyzInt16Values GY271_getThreeAxisValues(void);```

Get relative raw temperature

```int16_t GY271_getRelativeTemperature(void);```


#### Helper Functions


## Example
* Initialize Serial (USART)
* Initialiye TWI communication (default 400kHz)
* Initialize GY271 module by doing software reset and setting operating mode
* Get three axis values
* Print values to USART

### Picture of wiring
Fritzing

### Dependencies
* "myGlobalDefines.h"
* <util/delay.h>
* "myTWI.h"
* "myUSART.h"
* "myGY271.h"

