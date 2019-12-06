/*
 * myVariableTypes.h
 *
 * Created: 22.09.2019 15:36:00
 *  Author: Michael
 */ 


#ifndef MYVARIABLETYPES_H_
#define MYVARIABLETYPES_H_

#include <avr/io.h>

struct pairOfTwoUint16 {
	uint16_t sensorLowerBound;
	uint16_t sensorUpperBound;
};


#endif /* MYVARIABLETYPES_H_ */