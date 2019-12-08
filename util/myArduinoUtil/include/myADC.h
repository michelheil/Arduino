/**
 * @page ADC_page myADC.h
 * @brief Programm to apply Analog-to-Digital Converter (ADC) of ATMega328P
 * 
 * @date 09.10.2019 18:41:08
 * @author Michael
 * @copyright GNU General Public License
 */

#ifndef MYADC_H_
#define MYADC_H_

#include "myGlobalDefines.h" // required for struct pairOfTwoUint16
#include <avr/io.h> // required for uint types

/**
 * @brief Initialise Analog-to-Digital Converter (ADC) with 10-bit resolution
 */
void ADC_init(void);

/**
 * @brief ADC measurement for a single channel
 * 
 * @param channel input pin (0 for A0 etc.)
 * @return uint16_t 10-bit result of the conversion stored in a uint16_t
 */
uint16_t ADC_readAnalogPin(uint8_t channel);

/**
 * @brief Prevents from accidentally reading and processing the digital input signal from the analog pin
 * 
 * @param channel input pin (0 for A0 etc.)
 */
void ADC_disableDigitalInput(uint8_t channel);

/**
 * @brief Measure the values of a flaky actor for calibration
 * 
 * @param channel input pin (0 for A0 etc.)
 * @param calibrations number of measurements during calibration (each with 500ms pause)
 * 
 * @return struct of an upper and lower bound with the detected values
 */
struct pairOfTwoUint16 ADC_calibrateAnalogPin(uint8_t channel, int calibrations);

/**
 * @brief Scale a value within a range to an expected range of uint16_t values
 * 
 * @param sValue actual value that need to be scaled
 * @param detectedMinValue minimal detected value
 * @param detectedMaxValue maximal detected value
 * @param minFullRangeValue lower bound of expected range
 * @param maxFullRangeValue upper bound of expected range
 * 
 * @return scaled input value
 */
uint16_t mapSensorValueToFullRange(uint16_t sValue, uint16_t detectedMinValue, uint16_t detectedMaxValue, uint16_t minFullRangeValue, uint16_t maxFullRangeValue);

#endif /* MYADC_H_ */
