/**
 * @page TC16_page myTC16.h
 * @brief Programm to apply 16-bit Timer/Counter of ATMega328P
 * @details Here I can add some details to this library
 * 
 * @date 10.12.2019 18:41:08
 * @author Michael
 * @copyright GNU General Public License
 * 
 * @section 16-bit Timer/Counter
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MYTC16_H_
#define MYTC16_H_

/**
 * @brief initialize Timer 1A without interrupt and a Clear Timer on Compare Match and a pre-scaler
 * 
 * @param factor set prescale factor to 1, 8, 64, 256, or 1024 (default) for Timer/Counter
 * @return float conversion from seconds to number of ticks
 */
float TC16_init(int factor);


#endif /* MYTC16_H_ */

#ifdef __cplusplus
}
#endif