/*
 * myLOG.c
 *
 * Created: 08.11.2019 10:50:29
 *  Author: Michael
 */ 
#include "myLOG.h"

#include "myGlobalDefines.h"
#include "myUSART.h"

typedef enum {ALL = 1, DEBUG = 2, INFO = 3, WARN = 4, ERROR = 5, FATAL = 6, OFF = 7} logLevel;

#ifndef GLOBAL_LOG_LEVEL
#define GLOBAL_LOG_LEVEL 3 // default value if not specified differently
#endif // GLOBAL_LOG_LEVEL
    
void LOG_format(char * tag, char * message)
{
    USART_writeString(&tag[0]);
    USART_writeString(": ");
    USART_writeStringLn(&message[0]);
}

void LOG_error(char * message)
{
    logLevel lvl = ERROR;
    if(GLOBAL_LOG_LEVEL <= lvl) {
        LOG_format("ERROR", message);    
    }
}

void LOG_info(char * message)
{
    logLevel lvl = INFO;
    if(GLOBAL_LOG_LEVEL <= lvl) {
        LOG_format("INFO", message);
    }
}

void LOG_debug(char * message)
{
    logLevel lvl = DEBUG;
    if(GLOBAL_LOG_LEVEL <= lvl) {
        LOG_format("DEBUG", message);
    }
}