#ifndef __LED_DRIVER_H
#define __LED_DRIVER_H

#define kNumChannels 32 //16
#define kBitsPerChannel 12
// stupid preprocessor tricks
#define CreateChanArray(name) uint8_t name[(kNumChannels * kBitsPerChannel) / 8] = {0}

// Brightness is the duty cycle determined by a number between 0 (0%) and 4095 (100%)
void setBrightnessForChannel (uint8_t brightness, int channel, uint8_t *allChans);
void setBrightnessForAllChannels (int bright, uint8_t *allChans);
void writeBrightnessToDriver (uint8_t *chans);

// writes all 0xff
void writeDCToDriver (void);

#endif //__LED_DRIVER_H
