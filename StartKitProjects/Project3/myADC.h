/*
 * myADC.h
 *
 * Created: 03.09.2019 22:50:56
 *  Author: Michael
 */ 


#ifndef MYADC_H_
#define MYADC_H_

void ADC_init(void);
uint16_t ADC_readAnalogPin(uint8_t channel);
void ADC_startConversionAndWait();

#endif /* MYADC_H_ */