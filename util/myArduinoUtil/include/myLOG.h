/**
 * @page LOG_page myLOG.h
 * @brief Programm for logging
 * 
 * @date 09.10.2019 18:41:08
 * @author Michael
 * @copyright GNU General Public License
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MYLOG_H_
#define MYLOG_H_

/**
 * @brief Log an Error message
 * 
 * @param message Pointer of char array containing the log message
 */
void LOG_error(char * message);

/**
 * @brief Log an Info message
 * 
 * @param message Pointer of char array containing the log message
 */
void LOG_info(char * message);

/**
 * @brief Log a Debug message
 * 
 * @param message Pointer of char array containing the log message
 */
void LOG_debug(char * message);

#endif /* MYLOG_H_ */

#ifdef __cplusplus
}
#endif