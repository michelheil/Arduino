## Short Description
The 2-wire Serial Interface (TWI) is ideally suited for typical microcontroller applications. The TWI protocol allows the systems designer to interconnect up to 128 different devices using only two bi-directional bus lines, one for clock (SCL) and one for data (SDA). The only external hardware needed to implement the bus is a single pull-up resistor for each of the TWI bus lines. All devices connected to the bus have individual addresses, and mechanisms for resolving bus contention are inherent in the TWI protocol.

### Reference to Data Sheet ATMega328P
[Download ATMega328P data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)

Chapter 22. 2-wire Serial Interface

Features
- Simple Yet Powerful and Flexible Communication Interface, only two Bus Lines Needed
- Both Master and Slave Operation Supported
- Device can Operate as Transmitter or Receiver
- 7-bit Address Space Allows up to 128 Different Slave Addresses
- Multi-master Arbitration Support
- Up to 400kHz Data Transfer Speed
- Slew-rate Limited Output Drivers
- Noise Suppression Circuitry Rejects Spikes on Bus Lines
- Fully Programmable Slave Address with General Call Support
- Address Recognition Causes Wake-up

## Library Documentation

### Dependencies
* <avr/io.h>
* <util/twi.h>
* "myLOG.h"

### Init function
Initializes the TWI (I2C) communication on Arduino

According to the AMG88** data sheet the SCL clock frequency (f_SCL) has min = 0 kHz and max = 400 kHZ

According to the ATmega328p data sheet the SCL clock frequency is 

f_SCL = CPU_ClockFrequency / (16 + 2 * TWBR * PrescalerValue)

where TWBR is the value of the TWI Bit Rate Register and the PrescaleValue (in {1, 4, 16, 64}) 
can be set through the TWSR (TWI Status Register)

Assuming we want to work with a f_SCL of 100kHz

=> TWBR * PrescalerValue = 72

=> TWBR = 18 = 0b00010010

=> PrescalerValue: TWPS1 = 0, TWPS0 = 1 (both bits are part of byte TWSR)

'#define TWI_PRESCALER_VALUE     0x01 // PrescalerValue: 4

'#define TWI_BIT_RATE            0x12 // Decimal: 18

In General f_SCL = x (known)

TWBR * PrescalerValue =: y (unknown)

x = CPU_ClockFrequency / (16 + 2y)

<=> 16x + 2xy = CPU_ClockFrequency

<=> 2xy = CPU_ClockFrequency - 16x

<=> y = (CPU_ClockFrequency - 16x) / 2x

x = 400kHz

=> y = 12 (= TWBR * PrescalerValue)


```c
int TWI_init(void)
{
    // set SCL frequency
    TWSR |= TWI_PRESCALER_VALUE; // using "|=" instead of only "=" because status bits are initially set to 1
    TWBR = TWI_BIT_RATE;
    
    // The TWEN bit enables TWI operation and activates the TWI interface. When TWEN is written to one, the TWI
    // takes control over the I/O pins connected to the SCL and SDA pins, enabling the slew-rate limiters and
    // spike filters. If this bit is written to zero, the TWI is switched off and all TWI transmissions are
    // terminated, regardless of any ongoing operation.
    TWCR = (1 << TWEN);
    
    // activate internal pull-up resistors on SDA (PC4) and SCL (PC5)
    //DDRC = (1 << DDC5) | (1 << DDC4);
    //PORTC = (1 << PORTC5) | (1 << PORTC4);
    
    return 0;
}
```

### APIs
Set one byte in register of the device through TWI (I2C)

```void TWI_setRegisterByte(uint8_t sla, uint8_t reg, uint8_t val);```

Read one byte register of the device through TWI (I2C)

```uint8_t TWI_getRegisterByte(uint8_t sla, uint8_t reg);```

Read multiple byte register of the device through TWI (I2C)

```int TWI_getRegisterBytes(uint8_t sla, uint8_t reg, int len, uint8_t * dest);```


### Helper Functions
Send Start message

```void TWI_startTransmission(void);```

Send Repeated Start message

```void TWI_repeatStartTransmission(void);```

Send Slave Address + Read-/Write-Flag 

```void TWI_writeSlaRW(uint8_t slarw);```

Send Register Address of device where data will be written into

```void TWI_writeRegisterAddress(uint8_t addr);```

Send Byte that should be written into Register Address of device

```void TWI_writeByte(uint8_t addr);```

Send Stop message

```void TWI_stopTransmission(void);```
