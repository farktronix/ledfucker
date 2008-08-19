#include "Chaser.h"

static uint8_t curLED = 0;
static uint8_t sRate = 200;
static uint8_t rateCnt = 0;
static uint8_t fadeCnt = 1;

static CreateChanArray(sAllChans);

void ChaserStart (uint8_t redBrightness, uint8_t greenBrightness, uint8_t blueBrightness, uint8_t rate)
{
    curLED = 0;
    sRate = rate;
    fadeCnt = rate / 255;
}

void ChaserStep (void)
{
    rateCnt++;
    if (rateCnt == sRate) {
        rateCnt = 0;
        setLEDColor(curLED, LEDColorRed, 0xff, sAllChans);
        curLED++;
        if (curLED == 6) curLED = 0;
    }

    if (rateCnt % 65 == 0) {
        int ii;
        for (ii = 0; ii < sizeof(sAllChans); ii++) {
            uint8_t nibH = (sAllChans[ii] & 0xf0) >> 4;
            uint8_t nibL = sAllChans[ii] & 0x0f;
            if (nibH > 0) nibH--;
            if (nibL > 0) nibL--;
            sAllChans[ii] = (nibH << 4) | nibL;
        }
    }
    writeBrightnessToDriver(sAllChans);
}
