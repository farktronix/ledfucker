/* Name: main.c
 * Author: Some asshos
 * Copyright 2008
 * License: If you get the circuit wired up the code is as good as your own
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define bool uint8_t
#define true 1
#define false 0

volatile uint8_t intrCount;
volatile bool intrFlag;

#define LED_ON  (PORTB |= _BV(PB1))
#define LED_OFF (PORTB &= ~_BV(PB1))
#define LED_TOGGLE (PORTB ^= _BV(PB1))

#define SS_ON (PORTB |= _BV(PB2))
#define SS_OFF (PORTB &= ~_BV(PB2))
#define SS_TOGGLE (PORTB ^= _BV(PB2))

#define BLANK_ON (PORTD |= _BV(PD5))
#define BLANK_OFF (PORTD &= ~_BV(PD5))
#define BLANK_TOGGLE (PORTD ^= _BV(PD5))

#define VPRG_ON (PORTD |= _BV(PD6))
#define VPRG_OFF (PORTD &= ~_BV(PD6))
#define VPRG_TOGGLE (PORTD ^= _BV(PD6))

#define GSCLOCK_ON (PORTD |= _BV(PD7))
#define GSCLOCK_OFF (PORTD &= ~_BV(PD7))
#define GSCLOCK_TOGGLE (PORTD ^= _BV(PD7))

#define DCPRG_ON (PORTB |= _BV(PB7))
#define DCPRG_OFF (PORTB &= ~_BV(PB7))
#define DCPRG_TOGGLE (PORTB ^= _BV(PB7))

void setupPins (void)
{
    intrCount = 0;
    intrFlag = false;

    cli(); 
    CLKPR = 0x80;
    CLKPR = 0x00;

    /* timer0 prescaler = clk/1042 -> 11059200Hz / 1024 = 10800Hz per increment */
    /* overflow every 256 ticks = 42.1875Hz = 23.7037ms an interrupt */
    //TCCR0B |= _BV(CS00) | _BV(CS01) | _BV(CS02);
    //TCNT0   = 0;
    //TIMSK0 |= _BV(TOIE0); 
   
    /* Fast-PWM mode, BOTTOM set/CLEAR on compare*/
    TCCR2A  = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    /* timer1 prescaler = clk/128 */
    TCCR2B |= _BV(CS22);//_BV(CS20) | _BV(CS22);
    TCNT2   = 0;
    /* 50% duty cycle */
    OCR2B   = 127;
    TIMSK2 |= _BV(TOIE2); 

    // set status LED as output
    // set DCPRG as output
    DDRB |= _BV(PB1) | _BV(PB7);
    // set BLANK as output
    // set GSCLK as output
    // set VPRG as output
    // set PWM LED as output
    DDRD |= _BV(PD3) | _BV(PD5) | _BV(PD6) | _BV(PD7);
    
    /* Set MOSI and SCK output, all others input */ 
    DDRB |= _BV(PB2) | _BV(PB3) | _BV(PB5); 
    /* Enable SPI, Master, set clock rate fck/4 */ 
    SPCR = _BV(SPE) | _BV(MSTR);

    SS_ON;
    VPRG_ON;
    BLANK_ON;

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

#define kNumChannels 16
#define kBitsPerChannel 12
// stupid preprocessor tricks
#define CreateChanArray(name) char name[(kNumChannels * kBitsPerChannel) / 8] = {0}

// Brightness is the duty cycle determined by a number between 0 (0%) and 4095 (100%)
void setBrightnessForChannel (unsigned int brightness, int channel, char *allChans)
{
    // We get a 3 nibble value like so:
    // 0xA12
    // nibH: 0xA
    // nibM: 0x1
    // nibL: 0x2
    char nibH = (brightness >> 8) & 0x0F;
    char nibM = (brightness & 0xF0) >> 4;
    char nibL = brightness & 0xF;

    int bitnum = channel * kBitsPerChannel;
    int startByte = bitnum / 8;
    if (bitnum - (startByte * 8) == 0) {
        allChans[startByte] = (nibH << 4) | nibM;
        allChans[startByte + 1] = (allChans[startByte + 1] & 0xF) | (nibL << 4);
    } else {
        allChans[startByte] = (allChans[startByte] & 0xF0) | nibH;
        allChans[startByte + 1] = (nibM << 4) | nibL;
    }
}

void writeSPIByte (unsigned char byte)
{
    SPDR = byte;
    /* Wait for transmission complete */ 
    while(!(SPSR & _BV(SPIF)));
}

void writeBrightnessToDriver (char *chans)
{
    SS_OFF;
    VPRG_OFF;
    int ii;
    for (ii = 0; ii < (kNumChannels * kBitsPerChannel) / 8; ii++) {
        writeSPIByte(chans[ii]);
    }
    SS_ON;
    _delay_ms(10);
    SS_OFF;
    return;
}

void writeDCToDriver (void)
{
    
    int ii;
    SS_OFF;
    VPRG_ON;
    DCPRG_OFF;
    for (ii = 0; ii < 12; ii++) {
        writeSPIByte(0xFF);
    }
    SS_ON;
    _delay_ms(10);
    SS_OFF;
    return;
}

int main(void)
{
    setupPins();

    CreateChanArray(happyChan);
    int ii;
    for (ii = 0; ii <= kNumChannels; ii++) {
        setBrightnessForChannel(0x06f, ii, happyChan);
    }
    writeDCToDriver(); 
    writeBrightnessToDriver(happyChan);
    BLANK_OFF;

    for ( ; ; ) {
    }

    return 0;   /* never reached */
}

static volatile char clkCntL = 0;
static volatile char clkCntH = 0;
ISR(TIMER2_OVF_vect)
{
//    clkCntL++;
//    GSCLOCK_TOGGLE;
//    GSCLOCK_TOGGLE;
//    BLANK_ON;
//    BLANK_OFF;
//    if (clkCntL == 0xFF) {
//        clkCntH++;
//        clkCntL = 0;
//    }
//    if (clkCntH == 0x0F) {
//        clkCntL = 0;
//        clkCntH = 0;
//        LED_ON;
//        BLANK_ON;
//        BLANK_OFF;
//    }

//    static int16_t bob = 0;
//    static int dir = 1;
//    if(OCR2B == 255)
//        dir = -1;
//    if(OCR2B == 0)
//        dir = 1;
//    OCR2B += dir;
//
//    bob++;
//    if(bob == 0xff0) {
//        bob = 0;
//        BLANK_ON;
//        BLANK_OFF;
//        LED_ON;
//    }
}

ISR(TIMER0_OVF_vect)
{

//    clkCnt++;
//    if (clkCnt >= 0xFFF) {
//        BLANK_OFF;
//        _delay_us(100);
//        BLANK_ON;
//        LED_TOGGLE;
//    }
//    if (intrCount >= 2){
//        intrFlag = true;
//        intrCount = 0;
//    //    PORTB ^= _BV(PB1);
//    }
//    else{
//        intrCount = intrCount + 1;
//    }
}
