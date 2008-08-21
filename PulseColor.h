#ifndef __PULSE_COLOR_H
#define __PULSE_COLOR_H

#include "LEDCommon.h"

void PulseColorStart (uint8_t redBrightness, uint8_t greenBrightness, uint8_t blueBrightness, int rate);
PatternState PulseColorStep ();

#endif //__PULSE_COLOR_H
