/** 
 * @page TCS3200_page myTCS3200.h
 * @section xxx
 * @brief xxx
 * 
 * @date 20.12.2019 18:41:08
 * @author Michael
 * @copyright GNU General Public License
 */
#ifndef MYTCS3200_H_
#define MYTCS3200_H_

#include "myGlobalDefines.h"
#include <avr/io.h>

class TCS3200
{
  public:
    TCS3200(int * counter, uint8_t s0p, uint8_t s1p, uint8_t s2p, uint8_t s3p, uint8_t outp);
    ~TCS3200();                           // destructor
    void calibrate(void);                 // fills calibResult
    int measureColor(int color);          // get calibrated color value
    int redValue, greenValue, blueValue;  // store last calibrated color measurement

  private:
    int * interruptCounter;
    const uint8_t s0Pin;
    const uint8_t s1Pin;
    const uint8_t s2Pin;
    const uint8_t s3Pin;
    const uint8_t outPin;
    const int max_color_value = 255;
        
    struct rgb calibResult;               // store calibration result
    int redTicks, greenTicks, blueTicks;  // store last color measurement in Ticks
    
    int measureTicks(int color);          // count ticks per color during measurement
    void colorSelection(int s2, int s3);  // select color photodiodes befor measurement
};

#endif /* MYTCS3200_H_ */
