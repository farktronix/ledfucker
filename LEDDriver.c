#include "LEDCommon.h"
#include "LEDDriver.h"

// Brightness is the duty cycle determined by a number between 0 (0%) and 4095 (100%)
void setBrightnessForChannel (uint8_t brightness, int channel, uint8_t *allChans)
{
    // We get a 3 nibble value like so:
    // 0xA12
    // nibH: 0xA
    // nibM: 0x1
    // nibL: 0x2
    uint8_t nibH = (brightness >> 8) & 0x0F;
    uint8_t nibM = ((brightness & 0xF0) >> 4) & 0xF;
    uint8_t nibL = brightness & 0xF;

    int bitnum = channel * kBitsPerChannel;
    int startByte = bitnum / 8;
    if ((bitnum % 8) == 0) {
        allChans[startByte] = (nibM << 4) | nibL;
        allChans[startByte + 1] = (allChans[startByte + 1] & 0xF0) | nibH;
    } else {
        allChans[startByte] = (allChans[startByte] & 0xF) | (nibL << 4);
        allChans[startByte + 1] = (nibH << 4) | nibM;
    }
}

void setBrightnessForAllChannels (int bright, char *allChans)
{
    int ii;
    for (ii = 0; ii <= kNumChannels; ii++) {
        setBrightnessForChannel(bright, ii, allChans);
    }
}

static void _writeSPIByte (unsigned char byte)
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
        _writeSPIByte(chans[ii]);
    }
    SS_ON;
    SS_OFF;
    return;
}

void writeDCToDriver (void)
{
    int ii;
    SS_OFF;
    VPRG_ON;
    DCPRG_OFF;
    for (ii = 0; ii < (kNumChannels * 8); ii++) {
        _writeSPIByte(0xff);
    }
    SS_ON;
    SS_OFF;
    return;
}
