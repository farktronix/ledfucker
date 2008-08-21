#include "TestPattern.h"

static LEDColor sCurColor = LEDColorRed;
static uint16_t sLoopCount = 0;
static CreateChanArray(sAllChans);

#define kHoldTime 500

void TestPatternStart (void)
{
    sCurColor = LEDColorRed; 
    sLoopCount = kHoldTime - 1;

    setBrightnessForAllChannels(0x00, sAllChans);
}

static void _setConstantColor (uint8_t red, uint8_t green, uint8_t blue, uint8_t *chans)
{
    int ii = 0;
    for (ii = 0; ii < kNumLEDs; ii++) {
        setLEDColor(ii, LEDColorRed, red, chans);
        setLEDColor(ii, LEDColorGreen, green, chans);
        setLEDColor(ii, LEDColorBlue, blue, chans);
    }
    writeBrightnessToDriver(chans);
}

PatternState TestPatternStep (void)
{
    PatternState nextState = PatternStateTest;
    sLoopCount++;
    if (sLoopCount == kHoldTime) {
        sLoopCount = 0;
        switch (sCurColor) {
            case LEDColorRed:
                _setConstantColor(0xFF, 0, 0, sAllChans);
                sCurColor = LEDColorGreen;
                break;
            case LEDColorGreen:
                _setConstantColor(0, 0xFF, 0, sAllChans);
                sCurColor = LEDColorBlue;
                break;
            case LEDColorBlue:
                _setConstantColor(0, 0, 0xFF, sAllChans);
                sCurColor = 999;
                break;
            default:
                nextState = PatternStateTransition;
                break;
        }
    }
    return nextState;
}
