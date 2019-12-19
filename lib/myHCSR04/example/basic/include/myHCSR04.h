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
    HCSR04(uint8_t triggerP, uint8_t echoP, float ticksPerSec);   // constructor
    ~HCSR04();                                                    // destructor
    float       measureDistanceInCm(int percent);                 // measuring distance

  private:
    uint8_t     triggerPin;                                       // I/O pin of trigger
    uint8_t     echoPin;                                          // I/O pin of echo
    float       ticksPerSecond;                                   // conversion factor
    float       lastMeasuredDistInCm = 0.0f;                      // store last measure
    const int   triggerDurationUs = 15;                           // trigger duration [us]
    const float speedOfSound = 343.0f;                            // speed of sound [m/s]

    void        triggerMeasurement();                             // initialize trigger
    uint16_t    measureEchoDuration();                            // count echo duration
};

#endif /* MYHCSR04_H_ */