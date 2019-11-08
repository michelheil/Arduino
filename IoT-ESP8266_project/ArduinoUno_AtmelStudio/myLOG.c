/*
 * myLOG.c
 *
 * Created: 08.11.2019 10:50:29
 *  Author: Michael
 */ 
#include "globalDefines.h"

#include "myLOG.h"
#include "myUSART.h"

typedef enum {ALL = 1, DEBUGG = 2, INFO = 3, WARN = 4, ERROR = 5, FATAL = 6, OFF = 7} logLevel; // DEBUG does not work => debugG
    
void LOG_format(char * tag, char * message)
{
    char swTimestamp[32];
    sprintf(swTimestamp, "Dated: %s", __TIMESTAMP__); // Compilation timestamp
    
    USART_writeString(&swTimestamp[0]);
    USART_writeString(" ");
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
    logLevel lvl = DEBUGG;
    if(GLOBAL_LOG_LEVEL <= lvl) {
        LOG_format("DEBUG", message);
    }
}