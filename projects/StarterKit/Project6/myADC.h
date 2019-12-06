/*
 * myADC.h
 *
 * Created: 03.09.2019 22:50:56
 *  Author: Michael
 */ 


#ifndef MYADC_H_
#define MYADC_H_

#include "globalDefines.h"
#include "myVariableTypes.h"

#include <avr/io.h>
#include <util/delay.h>

void ADC_init(void);
void ADC_startConversionAndWait();
uint16_t ADC_readAnalogPin(uint8_t channel);
struct pairOfTwoUint16 ADC_calibrateAnalogPin(uint8_t channel, int calibrations);
void ADC_disableDigitalInput(uint8_t channel);

#endif /* MYADC_H_ */