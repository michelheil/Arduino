/*
 * myAMG8833.c
 *
 * Created: 10.10.2019 19:50:07
 *  Author: Michael
 */ 
#include "myAMG8833.h"

#include <util/delay.h>
#include "myUSART.h"
#include "myTWI.h"


// entering normal mode for AMG8833, restarting and setting general registers
int AMG8833_init()
{
    // initialize I2C for communication with AMG8833
    TWI_init();
    
    /////////////////////////////////////////////////
    // set Operating Mode
    USART_Headline("Set Operating Mode:");
    TWI_startTransmission();
    TWI_writeSlaW(AMG8833_SLA_W);
    TWI_writeRegisterAddress(AMG8833_PCTL);
    TWI_writeByte(AMG8833_PCTL_NORMAL_MODE);
    TWI_stopTransmission();


    /////////////////////////////////////////////////
    // software reset
    USART_Headline("Perform Software Reset:");
    TWI_startTransmission();
    TWI_writeSlaW(AMG8833_SLA_W);
    TWI_writeRegisterAddress(AMG8833_RST);
    TWI_writeByte(AMG8833_RST_INITIAL_RESET);
    TWI_stopTransmission();
    _delay_ms(500);

    /////////////////////////////////////////////////
    // set frame rate
    USART_Headline("Set Frame Rate:");
    TWI_startTransmission();
    TWI_writeSlaW(AMG8833_SLA_W);
    TWI_writeRegisterAddress(AMG8833_FPSC);
    TWI_writeByte(AMG8833_FPSC_10FPS);
    TWI_stopTransmission();
    _delay_ms(500);

    /////////////////////////////////////////////////
    // disable interrupts
    USART_Headline("Disable Interrupts:");
    TWI_startTransmission();
    TWI_writeSlaW(AMG8833_SLA_W);
    TWI_writeRegisterAddress(AMG8833_INTC);
    TWI_writeByte(AMG8833_INTC_INTEN_REACTIVE);
    TWI_stopTransmission();

    _delay_ms(100);

    return 0;
}

