#ifndef __PULSE_COLOR_H
#define __PULSE_COLOR_H

#include "LEDCommon.h"

// The actual brightness isn't yet working. Pass zero for off, non-zero for on
void PulseColorStart (uint8_t redBrightness, uint8_t greenBrightness, uint8_t blueBrightness, int rate);
void PulseColorStep ();

#endif //__PULSE_COLOR_H
