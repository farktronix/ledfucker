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

#define LED_ON  (PORTC |= _BV(PC5))
#define LED_OFF (PORTC &= ~_BV(PC5))
#define LED_TOGGLE (PORTC ^= _BV(PC5))

#define SS_ON (PORTB |= _BV(PB2))
#define SS_OFF (PORTB &= ~_BV(PB2))
#define SS_TOGGLE (PORTB ^= _BV(PB2))

#define BLANK_ON (PORTB |= _BV(PB1))
#define BLANK_OFF (PORTB &= ~_BV(PB1))
#define BLANK_TOGGLE (PORTB ^= _BV(PB1))

#define VPRG_ON (PORTD |= _BV(PD7))
#define VPRG_OFF (PORTD &= ~_BV(PD7))
#define VPRG_TOGGLE (PORTD ^= _BV(PD7))

#define GSCLOCK_ON (PORTD |= _BV(PD6))
#define GSCLOCK_OFF (PORTD &= ~_BV(PD6))
#define GSCLOCK_TOGGLE (PORTD ^= _BV(PD6))

#define DCPRG_ON (PORTB |= _BV(PB0))
#define DCPRG_OFF (PORTB &= ~_BV(PB0))
#define DCPRG_TOGGLE (PORTB ^= _BV(PB0))

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
    TCCR0A  = _BV(COM0A0) | _BV(WGM01);//_BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    TCCR0B |= _BV(CS01);
    TCNT0   = 0;
    OCR0A   = 1;
    //TCCR2A  = _BV(COM2B0) | _BV(WGM21);//_BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    //TCCR2B |= _BV(CS21);
    //TCNT2   = 0;
    //OCR2B   = 1;
    //TIMSK2 |= _BV(TOIE2); 

    /* set  LED */
    DDRC |= _BV(PC5);
    /* Set  GSCLK      VPRG       ????GSClck timer?*/
    DDRD |= _BV(PD3) | _BV(PD7) | _BV(PD6);// | _BV(PD5);
    /* Set  !SS        MOSI       SCK        BLANK      DCPRG */ 
    DDRB |= _BV(PB2) | _BV(PB3) | _BV(PB5) | _BV(PB1) | _BV(PB0); 
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

void setBrightnessForAllChannels (int bright, char *allChans)
{
    int ii;
    for (ii = 0; ii <= kNumChannels; ii++) {
        setBrightnessForChannel(bright, ii, allChans);
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
    for (ii = (kNumChannels * kBitsPerChannel) / 8; ii >= 0; ii--) {
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

volatile bool perFlag = false;

int main(void)
{
    setupPins();
    uint8_t up = 0, down = 0xff, count = 0;
    CreateChanArray(happyChan);
    unsigned char brightness = 0x00;
    int direction = -1;
    setBrightnessForAllChannels(brightness, happyChan);
    writeDCToDriver(); 
    writeBrightnessToDriver(happyChan);
    BLANK_OFF;
    //PORTC |= _BV(PC5);
    for ( ; ; ) {
    // chaser
//        setBrightnessForAllChannels(0x000, happyChan);
        //setBrightnessForChannel(0xFFF, brightness, happyChan);
        //writeSPIByte(0xFF); 
        //SS_ON;
        //_delay_ms(10);
        //SS_OFF;

        if(perFlag == true){
            // Impulse Blank every 4096 cycles
            direction = direction;
            
            // Clear flag 
            perFlag = false;
            //BLANK_OFF;
            
            // Fade and Glow... eventually
            #ifdef true
            //setBrightnessForAllChannels(brightness, happyChan);
            LED_ON;
            writeSPIByte(0x00); 
            writeSPIByte(0x00); //-0
            writeSPIByte(~brightness); //ff    for square green LED 
            writeSPIByte(0x00); 
            writeSPIByte(0x00); 
            writeSPIByte(0x00); 
            writeSPIByte(0x00); //0f
            writeSPIByte(0xf0); //f-    for white LED
            writeSPIByte(0x00); 
            writeSPIByte(0x00); 
            writeSPIByte(0x00); 
            writeSPIByte(0x00); 
            writeSPIByte(0x00); 
            writeSPIByte(0x00); 
            writeSPIByte(0x00); 
            writeSPIByte(brightness>>4); //-f
            writeSPIByte(brightness<<4); //f0    for round green LED
            writeSPIByte(0x00); 
            writeSPIByte(0x00); //-0
            writeSPIByte(up); //ff    for yellow LED
            writeSPIByte(0x00); 
            writeSPIByte(0x00); 
            writeSPIByte(0x00); 
            writeSPIByte(0x00); 
            SS_ON;
            SS_OFF;
            //writeBrightnessToDriver(happyChan);
            LED_OFF;
            count++;
            if(count == 2){
                count = 0;
                brightness += direction;
                if (brightness == 0xFF) direction = -1;
                else if (brightness == 0x00) direction = 1;
                up++;
                down--;
            }
            #endif
            

            //LED_TOGGLE;
        }
    }

    return 0;   /* never reached */
}

ISR(TIMER1_COMPA_vect) {
    perFlag = true;
    BLANK_ON;
    BLANK_OFF;
}

