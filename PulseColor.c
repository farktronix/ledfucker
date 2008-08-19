#include "PulseColor.h"

#define MAX_BRIGHT 0x1f

static uint8_t sPCRedRatio = 0;
static uint8_t sPCGreenRatio = 0;
static uint8_t sPCBlueRatio = 0;

static uint8_t sPCRedBrightness = 0;
static uint8_t sPCGreenBrightness = 0;
static uint8_t sPCBlueBrightness = 0;

static uint8_t sPCRate = 40;
static uint8_t sPCCurCount = 0;
static uint8_t sPCCurBrightness = 0;
static int sPCDirection = 1;
static CreateChanArray(sAllChans);

void PulseColorStart (uint8_t redBrightness, uint8_t greenBrightness, uint8_t blueBrightness, int rate)
{
    sPCRedRatio = redBrightness;
    sPCGreenRatio = greenBrightness;
    sPCBlueRatio = blueBrightness;

    sPCRedBrightness = 0;
    sPCGreenBrightness = 0;
    sPCBlueBrightness = 0;

    sPCRate = rate; 
    sPCCurCount = 0;
    sPCCurBrightness = 0;
}

void PulseColorStep ()
{
    sPCCurCount++;
    if (sPCCurCount == sPCRate) {
        sPCCurCount = 0;
        int ii;
        for (ii = 0; ii < kNumLEDs; ii++) {
            setLEDColor(ii, LEDColorRed, sPCRedBrightness, sAllChans);
            setLEDColor(ii, LEDColorGreen, sPCGreenBrightness, sAllChans);
            setLEDColor(ii, LEDColorBlue, sPCBlueBrightness, sAllChans);
        }
        writeBrightnessToDriver(sAllChans);
        sPCCurBrightness += sPCDirection;
        if (sPCRedRatio) sPCRedBrightness += sPCDirection;
        if (sPCGreenRatio) sPCGreenBrightness += sPCDirection;
        if (sPCBlueRatio) sPCBlueBrightness += sPCDirection;

        if (sPCCurBrightness == MAX_BRIGHT) sPCDirection = -1;
        else if (sPCCurBrightness == 0x00) sPCDirection = 1;
    }
}
