/**
 * @page LOG_page myLOG
 * @tableofcontents
 * @brief Programm for logging
 * 
 * @section Description
 * Currently, the logging programm is able to handle the levels error, info and debug. The idea is to cover all log levels such as: @n
 * @li ALL = 1
 * @li DEBUG = 2
 * @li INFO = 3
 * @li WARN = 4
 * @li ERROR = 5
 * @li FATAL = 6
 * @li OFF = 7
 * 
 * The log level is defined by setting the Variable GLOBAL_LOG_LEVEL to the metioned integer value.
 * 
 * @section Impressum Impressum
 * @date 09.10.2019 18:41:08
 * @author Michael Heil
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