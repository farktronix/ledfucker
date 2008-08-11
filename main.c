/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    /* insert your hardware initialization here */
    DDRB = DDB1 | DDRB;
    for( ; ; ){
        /* insert your main loop code here */
        PINB |= 1 << PB1;
//		PORTB = 0xFF;
        _delay_ms(100);
		PINB = 0x00;
//        PORTB = PB1 | PORTB;
//        _delay_ms(1000);
    }
    return 0;   /* never reached */
}
