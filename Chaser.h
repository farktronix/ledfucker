#ifndef __CHASER_H
#define __CHASER_H

#include "LEDCommon.h"

void ChaserStart (uint8_t redBrightness, uint8_t greenBrightness, uint8_t blueBrightness, uint8_t rate);
void ChaserStep (void);

#endif //__CHASER_H
