
/* TEST MOISTURE SENSOR - Start

#define LED0 PB0
#define LED1 PB1
#define LED2 PB2

ADC_init();

    // Start Conversion
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC)) {} // Wait for conversion to complete
    moistValue = ADC;

    if (moistValue < 1) {
        PORTB |= (1 << LED0);
        } else if (moistValue < 50) {
        PORTB |= (1 << LED1);
        } else if (moistValue < 300) {
        PORTB |= (1 << LED1) | (1 << LED0);
        } else if (moistValue < 500) {
        PORTB |= (1 << LED2);
        } else if (moistValue < 600) {
        PORTB |= (1 << LED2) | (1 << LED0);
        } else if (moistValue < 700) {
        PORTB |= (1 << LED2) | (1 << LED1);
        } else {
        PORTB |= (1 << LED2) | (1 << LED1) | (1 << LED0);
    }

    _delay_ms(3000);
    PORTB |= (1 << LED2) | (1 << LED1) | (1 << LED0);
    _delay_ms(100);
    PORTB = 0;
    _delay_ms(100);
TEST MOISTURE SENSOR - End */    
    
