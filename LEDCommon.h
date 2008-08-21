#ifndef __LED_COMMON_H
#define __LED_COMMON_H

#include <avr/io.h>

#include "LEDDriver.h"

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

#define bool uint8_t
#define true 1
#define false 0

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

typedef enum {
    LEDColorRed = 0,
    LEDColorGreen,
    LEDColorBlue
} LEDColor;

typedef enum {
    PatternStateTransition,
    PatternStateTest,
    PatternStatePulseColor
} PatternState;

#define kNumLEDs 10

// The leds are in RGB order
#define setLEDColor(ledNum, color, brightness, allChans) \
    setBrightnessForChannel((brightness), (((ledNum) * 3) + (color)), (allChans))
#define incrementLEDColor(ledNum, color, amount, allChans) \
    incrementBrightnessForChannel((amount), (((ledNum) * 3) + (color)), (allChans))

#endif //__LED_COMMON_H
