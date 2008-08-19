/* Name: main.c
 * Author: Some asshos
 * Copyright 2008
 * License: If you get the circuit wired up the code is as good as your own
 */

#include <util/delay.h>
#include <avr/interrupt.h>

#include "LEDCommon.h"
#include "LEDDriver.h"

#include "PulseColor.h"

volatile uint8_t intrCount;
volatile bool intrFlag;

void setupPins (void)
{
    intrCount = 0;
    intrFlag = false;

    cli(); 
    CLKPR = 0x80;
    CLKPR = 0x00;

    /* 
       timer1 prescaler = clk/1024, mode CTC, compare on 2
       triggers BLANK every 4096 clock cycles
       serves as BLANK (every 2^8 GS Clocks) 
       interrupt sets BLANK high, so clear immediately after 
    */
    //TCCR1A = _BV(COM1A0) | _BV(COM1A1); 
    TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10);
    OCR1AH = 0x0;
    OCR1AL = 0x7;
    TIMSK1 |= _BV(OCIE1A);

    /* 
       CTC with Toggle, clk/8
       Triggers GSClk once every 16 clock cycles
    */
    TCCR0A  = _BV(COM0A0) | _BV(WGM01);
    TCCR0B |= _BV(CS01);
    TCNT0   = 0;
    OCR0A   = 1;

    /* set  LED */
    DDRC |= _BV(PC5);
    /* Set  GSCLK      VPRG       ????GSClck timer?*/
    DDRD |= _BV(PD3) | _BV(PD7) | _BV(PD6);
    /* Set  !SS        MOSI       SCK        BLANK      DCPRG */ 
    DDRB |= _BV(PB2) | _BV(PB3) | _BV(PB5) | _BV(PB1) | _BV(PB0); 
    /* Enable SPI, Master, set clock rate fck/4 */ 
    SPCR = _BV(SPE) | _BV(MSTR);

    SS_ON;
    VPRG_ON;
    BLANK_ON;

    // display a power on blink so we know if the fucker resets itself.
    LED_ON;
    _delay_ms(200);
    int i;
    for (i = 0; i < 6; i++) {
        _delay_ms(50);
        LED_TOGGLE;
    }
    
    sei();

    return;    
}

typedef enum {
    PatternStatePulseColor
} PatternState;

static volatile bool intFired = false;

int main(void)
{
    setupPins();

    CreateChanArray(happyChan);
    setBrightnessForAllChannels(0x00, happyChan);
    writeDCToDriver(); 
    writeBrightnessToDriver(happyChan);

    PulseColorStart(0x00, 0xff, 0xff, 40);

    BLANK_OFF;
    for ( ; ; ) {
        if (intFired == true) {
            PulseColorStep();
            intFired = false;
        }
    }

    return 0;   /* never reached */
}

ISR(TIMER1_COMPA_vect) {
    intFired = true;
    BLANK_ON;
    BLANK_OFF;
}

