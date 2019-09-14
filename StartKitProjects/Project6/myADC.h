/*
 * myADC.h
 *
 * Created: 03.09.2019 22:50:56
 *  Author: Michael
 */ 


#ifndef MYADC_H_
#define MYADC_H_

void ADC_init(void);
void ADC_startConversionAndWait();
uint16_t ADC_readAnalogPin(uint8_t channel);
struct inputMinMax ADC_calibrateAnalogPin(uint8_t channel, int calibrations);
void ADC_disableDigitalInput(uint8_t channel);

struct inputMinMax {
	uint16_t sensorLowerBound;
	uint16_t sensorUpperBound;
};

#endif /* MYADC_H_ */