/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define bool uint8_t
#define true 1
#define false 0

static volatile uint8_t intrCount;
static volatile bool intrFlag;

int main(void)
{
    intrCount = 0;
    intrFlag = false;

    cli(); 
    /* timer0 prescaler = clk/1042 -> 11059200Hz / 1024 = 10800Hz per increment */
    /* overflow every 256 ticks = 42.1875Hz = 23.7037ms an interrupt */
    TCCR0B |= _BV(CS00) | _BV(CS02);
    TCNT0 = 0;
    TIMSK0 |= _BV(TOIE0); 
 //   TCCR0A = 0x00;//_BV(WGM01);                //Set CTC Mode disabling the output
 //   TCCR0B = _BV(CS02) | _BV(CS00);     //1024 prescaler
 //   OCR0A  = 0xff;                      //essentially overflow
 //   TCNT0=0x00;
 //   TIMSK0 = _BV(TOIE0);               //enable OC interupt
////_BV(OCIE0A) | _BV(TOIE0);               //enable OC interupt

    DDRB  =  _BV(DDB1);
  //  DDRB &= ~DDB1;
  //  PORTB = _BV(PB1);

    sei();

   /* insert your hardware initialization here */
    for( ; ; ){
 
        if(intrFlag == true){
            intrFlag = false;
           // PORTB ^= _BV(PB1);
           //PORTB = _BV(PB1);
        }
    }
    return 0;   /* never reached */
}

ISR(TIMER0_OVF_vect)
{
    if (intrCount >= 2){
        intrFlag = true;
        intrCount = 0;
        PORTB ^= _BV(PB1);
    }
    else{
        intrCount = intrCount + 1;
    }
    reti();
}
