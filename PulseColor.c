#include "PulseColor.h"
#include <util/delay.h>

#define MAX_BRIGHT 0xff

static uint8_t sPCRedRatio = 0;
static uint8_t sPCGreenRatio = 0;
static uint8_t sPCBlueRatio = 0;

static uint8_t sPCRate = 40;
static uint8_t sPCCurCount = 0;
static uint8_t sPCCurBrightness = 0;
static char sPCDirection = 1;
static CreateChanArray(sAllChans);

void PulseColorStart (uint8_t redBrightness, uint8_t greenBrightness, uint8_t blueBrightness, int rate)
{
    sPCRedRatio = (redBrightness == 0 ? 0 : (255 / redBrightness));
    sPCGreenRatio = (greenBrightness == 0 ? 0 : (255 / greenBrightness));
    sPCBlueRatio = (blueBrightness == 0 ? 0 : (255 / blueBrightness));

    sPCRate = rate; 
    sPCCurCount = 0;
    sPCCurBrightness = 1;
    sPCDirection = 1;

    setBrightnessForAllChannels(0xff, sAllChans);
    writeBrightnessToDriver(sAllChans);
    _delay_ms(20);
    setBrightnessForAllChannels(0x00, sAllChans);
    writeBrightnessToDriver(sAllChans);
}

PatternState PulseColorStep ()
{
    sPCCurCount++;
    if (sPCCurCount >= sPCRate) {
        sPCCurCount = 0;
        uint8_t ii;

        // XXX: none of this is working right now. I have no idea why.
        bool redUp =   (sPCCurBrightness % sPCRedRatio == 0);
        bool greenUp = (sPCCurBrightness % sPCGreenRatio == 0);
        bool blueUp =  (sPCCurBrightness % sPCBlueRatio == 0);
        int8_t direction = sPCDirection;
        for (ii = 0; ii < kNumLEDs; ii++) {
            if (redUp) incrementLEDColor(ii, LEDColorRed, direction, sAllChans);
            if (greenUp) incrementLEDColor(ii, LEDColorGreen, direction, sAllChans);
            if (blueUp) incrementLEDColor(ii, LEDColorBlue, direction, sAllChans);
        }
        writeBrightnessToDriver(sAllChans);
        sPCCurBrightness += sPCDirection;

        if (sPCCurBrightness == MAX_BRIGHT) sPCDirection = -1;
        else if (sPCCurBrightness == 0x01) sPCDirection = 1;
    }
    return PatternStatePulseColor;
}
