## Sensor Description
Ultrasonic Ranging Module HC - SR04 

Ultrasonic ranging module HC - SR04 provides 2cm - 400cm non-contact 
measurement function, the ranging accuracy can reach to 3mm. The modules 
includes ultrasonic transmitters, receiver and control circuit. The basic
principle of work: 
(1) Using IO trigger for at least 10us high level signal, 
(2) The Module automatically sends eight 40 kHz and detect whether there 
is a pulse signal back. 
(3) IF the signal back, through high level , time of high output IO duration 
is the time from sending ultrasonic to returning. 
Test distance = (high level time×velocity of sound (340M/S) / 2,

### Link to data sheet
[DataSheet used for my Projects](https://github.com/michelheil/Arduino/lib/myHCSR04/datasheet)

### Pictures of sensor
<img src="https://github.com/michelheil/Arduino/blob/master/lib/myHCSR04/pictures/HCSR04_Front.jpg" width="250"> <img src="https://github.com/michelheil/Arduino/blob/master/lib/myHCSR04/pictures/HCSR04_Back.jpg" width="250">


## Library Documentation

### Dependencies
* "myGlobalDefines.h"
* <avr/io.h>
* <util/delay.h>

### Init function
```/**
 * @brief Construct a new HCSR04::HCSR04 object
 * @details set wired pins and the conversion factor of the used Timer/Counter
 * 
 * @param triggerP name of wired trigger pin
 * @param echoP name of wired echo pin
 * @param ticksPerSec factor to convert ticks into seconds based on the used Timer/Counter
 */
HCSR04::HCSR04(uint8_t triggerP, uint8_t echoP, float ticksPerSec):
    triggerPin(triggerP), echoPin(echoP), ticksPerSecond(ticksPerSec) {

  sbi(DDRD, triggerPin);
  cbi(DDRB, echoPin);
}
```

### APIs
Main function of HCSR04 class that returns the distance in centimeters

```float HCSR04::measureDistanceInCm(int percent);```

#### Helper Functions
Trigger a distance measurement by setting trigger Pin at least 10us to high

```void HCSR04::triggerMeasurement();```

Measure the echo duration of HCSR04 device after it has been triggered

```uint16_t HCSR04::measureEchoDuration();```


## Example
* Initialize USART
* Initialize Timer/Counter 16-bit and store tickPerSecond conversion factor
* Initialize sensor by specifying the trigger and echo pins
* Measure Distance in cm
* Print result to USART

### Picture of wiring
Fritzing

### Dependencies
* "myGlobalDefines.h"
* <util/delay.h>
* "myTC16.h"
* "myHCSR04.h"
* "myUSART.h"
