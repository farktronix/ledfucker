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

static volatile uint8_t intrCount;
static volatile bool intrFlag;

void setupPins (void)
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

    /* Set MOSI and SCK output, all others input */ 
    PORTB |= (1<<PB3)|(1<<PB5); 
    /* Enable SPI, Master, set clock rate fck/128 */ 
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0); 

    DDRD = DDD1 | DDRD;
    DDRB = DDB1 | DDRB;

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

void writeSPIByte (char byte)
{
    SPDR = byte; 
    /* Wait for transmission complete */ 
    while(!(SPSR & (1<<SPIF)));
}

void writeBrightnessToDriver (char *chans)
{
    int ii;
    for (ii = 0; ii < kNumChannels; ii++) {
        writeSPIByte(chans[ii]);
    }
    return;
}

int main(void)
{
    setupPins();

    CreateChanArray(happyChan);
    int ii;
    for (ii = 0; ii < kNumChannels; ii++) {
        setBrightnessForChannel(0x800, ii, happyChan);
    }
    writeBrightnessToDriver(happyChan);

    for ( ; ; ) {
        _delay_ms(100);
        PIND ^= 1 << PD7;        
        PINB ^= 1 << PB1;        
        _delay_ms(100);

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
