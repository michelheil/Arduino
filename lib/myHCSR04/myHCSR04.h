/** 
 * @page HCSR04_page myHCSR04.h
 * @section xxx
 * @brief xxx
 * 
 * @date 10.12.2019 18:41:08
 * @author Michael
 * @copyright GNU General Public License
 */

#ifndef MYHCSR04_H_
#define MYHCSR04_H_

#include <avr/io.h>
typedef unsigned char Byte;

class HCSR04
{
  public:
    HCSR04(uint8_t triggerPin, uint8_t echoPin, float ticksPerSecond);
    ~HCSR04();

    float measureDistanceInCm();

  private:
    uint8_t _triggerPin;
    uint8_t _echoPin;
    float _ticksPerSecond;
    float _lastMeasuredDistanceInCm = 0;
    const int _triggerDurationUs = 50;
    const float _speedOfSound = 343.0f;

    const Byte _triggerPortAddress = 0x0B; // PORTD
    const Byte _triggerDDRAddress = 0x0A; // DDRD
    const Byte _echoPortAddress = 0x05; // PORTB
    const Byte _echoDDRAddress = 0x04; // DDRB
    const Byte _echoInputPinAddress = 0x03; // PINB

    void triggerMeasurement();
    uint16_t measureEchoDuration();
};

#endif /* MYHCSR04_H_ */