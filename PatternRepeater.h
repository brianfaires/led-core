#pragma once
#include "PatternCommon.h"


class PatternRepeater {
  public:
    PatternRepeater();
    void Init();
    void SetDimPattern(uint8_t* newPattern, uint16_t newDimPeriod);
    void SetColorPattern(CRGB* newPattern, uint16_t newColorPeriod);
    void ScrollDimPattern(bool scrollForward);
    void ScrollColorPattern(bool scrollForward);
    void SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs);
	uint16_t GetDimIndexFirst();
	uint16_t GetColorIndexFirst();

    uint8_t myBrightness;
    
  private:
    CRGB colorPattern[MAX_PERIOD];
    uint8_t colorPeriod;
    uint8_t colorIndexFirst;
    
    uint8_t dimPattern[MAX_PERIOD];
    uint8_t dimPeriod;
    uint8_t dimIndexFirst;
};
