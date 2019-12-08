/*
    Using 10-bit Analog-to-digital Converter of Arduino Uno
    Copyright (C) 2019  Michael Heil

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Created: 03.09.2019 22:50:56
 *  Author: Michael
 */ 


#ifndef MYADC_H_
#define MYADC_H_

#include "myGlobalDefines.h" // required for struct pairOfTwoUint16
#include <avr/io.h> // required for uint types

/*
 * Initialise Analog-to-Digital Converter (ADC) with 10-bit resolution
 */
void ADC_init(void);

/**
 * @brief ADC measurement for a single channel
 * 
 * @param channel input pin (0 for A0 etc.)
 * @return uint16_t 10-bit result of the conversion stored in a uint16_t
 */
uint16_t ADC_readAnalogPin(uint8_t channel);

/*
 * Prevents from accidentally reading and processing the digital input signal from the analog pin
 * 
 * channel: input pin (0 for A0 etc.)
 */
void ADC_disableDigitalInput(uint8_t channel);

/*
 * Measure the values of a flaky actor for calibration
 * 
 * channel: input pin (0 for A0 etc.)
 * calibrations: number of measurements during calibration (each with 500ms pause)
 * 
 * returns: a strcut of an upper and lower bound with the detected values
 */
struct pairOfTwoUint16 ADC_calibrateAnalogPin(uint8_t channel, int calibrations);

/*
 * Scales a value within a range to an expected range of uint16_t values
 * 
 * sValue: actual value that need to be scaled
 * detectedMinValue: minimal detected value
 * detectedMaxValue: maximal detected value
 * minFullRangeValue: lower bound of expected range
 * maxFullRangeValue: upper bound of expected range
 * 
 * return: scaled input value
 */
uint16_t mapSensorValueToFullRange(uint16_t sValue, uint16_t detectedMinValue, uint16_t detectedMaxValue, uint16_t minFullRangeValue, uint16_t maxFullRangeValue);

#endif /* MYADC_H_ */
