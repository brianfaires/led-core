#pragma once
#include "Arduino.h"
#include "FastLED.h"
#include "GammaManager.h"
#include "PaletteManager.h"

#include "ArduinoTrace.h"

#define NUM_DIM_PATTERNS 16
#define NUM_COLOR_PATTERNS 2

enum dim_pattern_names { COMET_F, COMET_R, TWO_SIDED, BARBELL, SLOPED_TOWERS_H, SLOPED_TOWERS_L, SLIDE_H, SLIDE_L, BOWTIES_F, BOWTIES_R, TOWERS, SNAKE, SNAKE2, THREE_COMETS_F, THREE_COMETS_R};

class PatternGenerator {
  public:
    uint8_t numColors, brightLength, transLength, dimPeriod, colorPeriod;
    
    PatternGenerator();
    void Init(PaletteManager* _pm, GammaManager* gm);
	void WriteDimPattern(uint8_t patternIndex, uint8_t* outputArray);
    void WriteColorPattern(uint8_t patternIndex, CRGB* outputArray);
	uint8_t GetTransFactor(uint8_t patternIndex);
	uint8_t GetBrightFactor(uint8_t patternIndex);

  private:
	PaletteManager* pm;
	GammaManager* Gamma;
	
    void WriteColorPattern_Gradient(CRGB* outputArray);
    void WriteColorPattern_Blocks(CRGB* outputArray);
    
    void WriteDimPattern_Comet(uint8_t* outputArray);
    void WriteDimPattern_ReverseComet(uint8_t* outputArray);
    void WriteDimPattern_TwoSided(uint8_t* outputArray);
    void WriteDimPattern_Barbell(uint8_t* outputArray);
    void WriteDimPattern_Snake(uint8_t* outputArray);
    void WriteDimPattern_Snake2(uint8_t* outputArray);
    void WriteDimPattern_ThreeEvenComets(uint8_t* outputArray);
    void WriteDimPattern_ThreeEvenReverseComets(uint8_t* outputArray);
    void WriteDimPattern_Towers(uint8_t* outputArray);
    void WriteDimPattern_Bowties(uint8_t* outputArray);
    void WriteDimPattern_ReverseBowties(uint8_t* outputArray);
    void WriteDimPattern_SlopedHighTowers(uint8_t* outputArray);
    void WriteDimPattern_SlopedLowTowers(uint8_t* outputArray);
    void WriteDimPattern_SlideHigh(uint8_t* outputArray);
    void WriteDimPattern_SlideLow(uint8_t* outputArray);
    void WriteDimPattern_NoDim(uint8_t* outputArray);
};
