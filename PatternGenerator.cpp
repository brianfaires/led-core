#include "PatternGenerator.h"

// Macros for shorthand pattern definition
#define TEST_DIM_PERIOD(x) if(x > dimPeriod) { THROW(Out of bounds dimPeriod) DUMP(x) }
#define GET_FADE_STEP_SIZE(x) 255.0f / (x+1)
#define SETUP_DIM_PATTERN()  uint8_t i = 0; uint8_t limit = 0; 
#define SETUP_FADE(x)  float fadeStepSize = GET_FADE_STEP_SIZE(x); uint8_t lastLimitMinusOne;
#define SETUP_FADE2(x) float fadeStepSize2 = GET_FADE_STEP_SIZE(x);

#define SINGLE_SPACE() outputPattern[i] = 0; i++; limit++;
#define SINGLE_BRIGHT() outputPattern[i] = 255; i++; limit++;
#define FILL_SPACING() while(i < limit) { outputPattern[i] = 0; i++; }
#define DRAW_SPACING(x) for(limit += x; i < limit; i++) { outputPattern[i] = 0; }

#define DRAW_BRIGHT(x) for(limit += x; i < limit; i++) { outputPattern[i] = 255; }
#define DRAW_DIM(x) for(limit += x; i < limit; i++) { outputPattern[i] = 32; }
#define FADE_DOWN(x)  for(limit += x; i < limit; i++) { outputPattern[i] = fadeStepSize * (limit - i); }
#define FADE_DOWN2(x) for(limit += x; i < limit; i++) { outputPattern[i] = fadeStepSize2 * (limit - i); }
#define FADE_UP(x)  lastLimitMinusOne = limit - 1; for(limit += x; i < limit; i++) { outputPattern[i] = fadeStepSize  * (uint8_t)(i - lastLimitMinusOne); }
#define FADE_UP2(x) lastLimitMinusOne = limit - 1; for(limit += x; i < limit; i++) { outputPattern[i] = fadeStepSize2 * (uint8_t)(i - lastLimitMinusOne); }


PatternGenerator::PatternGenerator() {
  numColors = 0;
  brightLength = 0;
  transLength = 0;
  dimPeriod = 0;
  colorPeriod = 0;
}

void PatternGenerator::Init(PaletteManager* _pm, GammaManager* gm) {
	pm = _pm;
	Gamma = gm;
}

void PatternGenerator::WriteDimPattern(uint8_t targetDimPatternIndex, uint8_t* outputArray) {
  switch(targetDimPatternIndex) {
    case 0:   WriteDimPattern_Comet(outputArray); break;
    case 1:   WriteDimPattern_ReverseComet(outputArray); break;
    case 2:   WriteDimPattern_TwoSided(outputArray); break;
    case 3:   WriteDimPattern_Barbell(outputArray); break;
    case 4:   WriteDimPattern_SlopedHighTowers(outputArray); break;
    case 5:   WriteDimPattern_SlopedLowTowers(outputArray); break;
    case 6:   WriteDimPattern_SlideHigh(outputArray); break;
    case 7:   WriteDimPattern_SlideLow(outputArray); break;
    case 8:   WriteDimPattern_Bowties(outputArray); break;
    case 9:   WriteDimPattern_ReverseBowties(outputArray); break;
    case 10:  WriteDimPattern_Towers(outputArray); break;
    case 11:  WriteDimPattern_Snake(outputArray); break;
    case 12:  WriteDimPattern_Snake2(outputArray); break;
    case 13:  WriteDimPattern_ThreeEvenComets(outputArray); break;
    case 14:  WriteDimPattern_ThreeEvenReverseComets(outputArray); break;
    default:  WriteDimPattern_NoDim(outputArray); DUMP(targetDimPatternIndex); break;
  }

  //for(uint8_t i =0; i < dimPeriod; i++) { Serial.println(String(i) + ": " + outputArray[i]);}
}

void PatternGenerator::WriteColorPattern(uint8_t targetColorPatternIndex, CRGB* outputArray) {
  switch(targetColorPatternIndex) {
    case 0:  WriteColorPattern_Gradient(outputArray); break;
    default: WriteColorPattern_Blocks(outputArray); DUMP(targetColorPatternIndex); break;
  }
  
  //for(uint8_t i =0; i < colorPeriod; i++) { Serial.println(String(i) + ": (" + outputArray[i].r + ", " + outputArray[i].g + ", " + outputArray[i].b + ")"); }
}


// Color patterns
void PatternGenerator::WriteColorPattern_Gradient(CRGB* outputArray) {
  uint8_t segLength = colorPeriod / numColors;
  uint8_t extraPixels = 0;

  for(uint8_t i = 0; i < numColors; i++) {
	uint8_t adjLength = segLength;
	if(segLength*numColors + extraPixels < colorPeriod) {
		adjLength++;
	}
	
	CRGB first = pm->palette[i];
	CRGB second = i == numColors-1 ? pm->palette[0] : pm->palette[i+1];
	Gamma->Inverse(first);
	Gamma->Inverse(second);
	
    for(uint8_t j = 0; j < adjLength; j++) {
      uint8_t blendAmount = j * 255 / adjLength;
	  uint16_t idx = segLength*i + j + extraPixels;
      outputArray[idx] = blend(first, second, blendAmount);
	  Gamma->Correct(outputArray[idx]);
    }
	
	if(adjLength > segLength) { extraPixels++; }
  }
}

void PatternGenerator::WriteColorPattern_Blocks(CRGB* outputArray) {
  uint16_t colorLengths[numColors];
  uint16_t minLength = colorPeriod / numColors;

  for(uint8_t i = 0; i < numColors; i++) { colorLengths[i] = minLength; }
  
  // Assign extra pixels, inside first
  uint8_t extra = colorPeriod - minLength * numColors;
  if(extra % 2 == 1 && numColors % 2 == 1) {
    colorLengths[numColors/2]++;
    extra--;
  }    
  if(extra > 0) {
    uint8_t assigned = 0;
    for(uint8_t i = 0; i < numColors/2; i++) {
      colorLengths[numColors/2 - 1 - i]++;
      assigned++;
      if(assigned == extra) { break; }
      
      colorLengths[(numColors+1)/2 + i]++;
      assigned++;
      if(assigned == extra) { break; }
    }
  }

  // Write pattern
  uint16_t pixel = 0;
  for(uint8_t col = 0; col < numColors; col++) {
    for(uint16_t i = 0; i < colorLengths[col]; i++) {
      outputArray[pixel++] = pm->palette[col];
    }
  }
}


// Dim patterns
void PatternGenerator::WriteDimPattern_Comet(uint8_t* outputPattern) {
  TEST_DIM_PERIOD(brightLength + transLength)
  SETUP_DIM_PATTERN()
  SETUP_FADE(transLength)
  
  FADE_UP(transLength)
  DRAW_BRIGHT(brightLength)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_ReverseComet(uint8_t* outputPattern) {
  TEST_DIM_PERIOD(brightLength + transLength)
  SETUP_DIM_PATTERN()
  SETUP_FADE(transLength)
  
  DRAW_BRIGHT(brightLength)
  FADE_DOWN(transLength)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_TwoSided(uint8_t* outputPattern) {
  uint8_t halfTrans = transLength/2;
  uint8_t adjBright = (2*halfTrans == transLength) ? brightLength : brightLength+1;
  
  TEST_DIM_PERIOD(adjBright + 2*halfTrans)
  SETUP_DIM_PATTERN()
  SETUP_FADE(halfTrans);
  
  FADE_UP(halfTrans)
  DRAW_BRIGHT(adjBright)
  FADE_DOWN(halfTrans)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_Barbell(uint8_t* outputPattern) {
  uint8_t halfTrans = transLength/2;
  uint8_t adjBright = (2*halfTrans == transLength) ? brightLength : brightLength+1;
  uint8_t halfBright = adjBright/2;
  
  TEST_DIM_PERIOD(2*halfBright + 2*halfTrans)
  SETUP_DIM_PATTERN()
  SETUP_FADE(halfTrans);
  
  DRAW_BRIGHT(halfBright)
  FADE_DOWN(halfTrans)
  FADE_UP(halfTrans)
  DRAW_BRIGHT(halfBright)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_Snake(uint8_t* outputPattern) {
  uint8_t adjBright = brightLength + transLength;
  TEST_DIM_PERIOD(adjBright)
  SETUP_DIM_PATTERN()
  
  DRAW_BRIGHT(adjBright)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_Snake2(uint8_t* outputPattern) {
  uint8_t halfBright = brightLength / 2;
  uint8_t halfSpacing = (dimPeriod - 2*halfBright) / 2;

  TEST_DIM_PERIOD(2*halfBright + 2*halfSpacing)
  SETUP_DIM_PATTERN()
  
  DRAW_BRIGHT(halfBright)
  DRAW_SPACING(halfSpacing)
  DRAW_BRIGHT(halfBright)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_ThreeEvenComets(uint8_t* outputPattern) {
  uint8_t adjLength = (transLength + brightLength) / 3;
  uint8_t remainder = (transLength + brightLength) - 3*adjLength;
  
  TEST_DIM_PERIOD(3*adjLength + remainder + 3)
  SETUP_DIM_PATTERN()
  SETUP_FADE(adjLength-1)
  
  FADE_UP(adjLength-1)
  SINGLE_BRIGHT()
  if(remainder == 2) { SINGLE_BRIGHT() }
  SINGLE_SPACE()
  
  FADE_UP(adjLength-1)
  SINGLE_BRIGHT()
  if(remainder == 1) { SINGLE_BRIGHT() }
  SINGLE_SPACE()
  
  FADE_UP(adjLength-1)
  SINGLE_BRIGHT()
  if(remainder == 2) { SINGLE_BRIGHT() }
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_ThreeEvenReverseComets(uint8_t* outputPattern) {
  uint8_t adjLength = (transLength + brightLength) / 3;
  uint8_t remainder = (transLength + brightLength) - 3*adjLength;
  
  TEST_DIM_PERIOD(3*adjLength + remainder + 3)
  SETUP_DIM_PATTERN()
  SETUP_FADE(adjLength-1)
  
  SINGLE_BRIGHT()
  if(remainder == 2) { SINGLE_BRIGHT() }
  FADE_DOWN(adjLength-1)
  SINGLE_SPACE()
  
  SINGLE_BRIGHT()
  if(remainder == 1) { SINGLE_BRIGHT() }
  FADE_DOWN(adjLength-1)
  SINGLE_SPACE()
  
  SINGLE_BRIGHT()
  if(remainder == 2) { SINGLE_BRIGHT() }
  FADE_DOWN(adjLength-1)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_Towers(uint8_t* outputPattern) {
  uint8_t halfTrans = transLength / 2;
  uint8_t adjBright = brightLength + (2*halfTrans != transLength ? 1 : 0);
  
  TEST_DIM_PERIOD(adjBright + 2*halfTrans + 2)
  SETUP_DIM_PATTERN()
  SETUP_FADE(transLength+1)
  
  DRAW_DIM(halfTrans)
  SINGLE_SPACE()
  DRAW_BRIGHT(adjBright)
  SINGLE_SPACE()
  DRAW_DIM(halfTrans)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_Bowties(uint8_t* outputPattern) {
  uint8_t quartTrans = transLength / 4;
  uint8_t transInner = quartTrans;
  uint8_t transOuter = quartTrans;
  uint8_t adjBright = brightLength;
  uint8_t remainder = transLength - 4*quartTrans;
  if(remainder >= 2) { transOuter++; }
  if(remainder == 1 || remainder == 3) { adjBright++; }

  TEST_DIM_PERIOD(2*transOuter + 2*transInner + adjBright)
  SETUP_DIM_PATTERN()
  SETUP_FADE(transOuter)
  SETUP_FADE2(transInner)

  FADE_DOWN(transOuter)
  FADE_UP2(transInner)
  DRAW_BRIGHT(adjBright)
  FADE_DOWN2(transInner)
  FADE_UP(transOuter)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_ReverseBowties(uint8_t* outputPattern) {
  uint8_t quartTrans = transLength / 4;
  uint8_t transInner = quartTrans;
  uint8_t transOuter = quartTrans;
  uint8_t adjBright = brightLength;
  uint8_t remainder = transLength - 4*quartTrans;
  if(remainder >= 2) { transOuter++; }
  if(remainder == 1 || remainder == 3) { adjBright++; }

  TEST_DIM_PERIOD(2*transOuter + 2*transInner + adjBright + 2)
  SETUP_DIM_PATTERN()
  SETUP_FADE(transOuter)
  SETUP_FADE2(transInner)

  FADE_UP(transOuter)
  FADE_DOWN2(transInner)
  SINGLE_SPACE()
  DRAW_BRIGHT(adjBright)
  SINGLE_SPACE()
  FADE_UP2(transInner)
  FADE_DOWN(transOuter)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_SlopedHighTowers(uint8_t* outputPattern) {
  uint8_t adjBright = brightLength;
  uint8_t halfTrans = transLength / 2;
  if(transLength != 2*halfTrans) { adjBright++; }
  
  TEST_DIM_PERIOD(adjBright + 2*halfTrans + 2)
  SETUP_DIM_PATTERN()
  SETUP_FADE(halfTrans)
  
  FADE_DOWN(transLength)
  SINGLE_SPACE()
  DRAW_BRIGHT(brightLength)
  SINGLE_SPACE()
  FADE_UP(transLength)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_SlopedLowTowers(uint8_t* outputPattern) {
  uint8_t adjBright = brightLength;
  uint8_t halfTrans = transLength / 2;
  if(transLength != 2*halfTrans) { adjBright++; }
  
  TEST_DIM_PERIOD(adjBright + 2*halfTrans + 2)
  SETUP_DIM_PATTERN()
  SETUP_FADE(halfTrans)
  
  FADE_DOWN(transLength)
  SINGLE_SPACE()
  DRAW_DIM(brightLength)
  SINGLE_SPACE()
  FADE_UP(transLength)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_SlideHigh(uint8_t* outputPattern) {
  uint8_t adjBright = brightLength;
  uint8_t halfTrans = transLength / 2;
  if(transLength != 2*halfTrans) { adjBright++; }
  
  TEST_DIM_PERIOD(adjBright + 2*halfTrans + 2)
  SETUP_DIM_PATTERN()
  SETUP_FADE(halfTrans)
  
  FADE_UP(transLength)
  SINGLE_SPACE()
  DRAW_BRIGHT(brightLength)
  SINGLE_SPACE()
  FADE_DOWN(transLength)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_SlideLow(uint8_t* outputPattern) {
  uint8_t adjBright = brightLength;
  uint8_t halfTrans = transLength / 2;
  if(transLength != 2*halfTrans) { adjBright++; }
  
  TEST_DIM_PERIOD(adjBright + 2*halfTrans + 2)
  SETUP_DIM_PATTERN()
  SETUP_FADE(halfTrans)
  
  FADE_UP(transLength)
  SINGLE_SPACE()
  DRAW_DIM(brightLength)
  SINGLE_SPACE()
  FADE_DOWN(transLength)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_NoDim(uint8_t* outputPattern) {
  SETUP_DIM_PATTERN()
  DRAW_BRIGHT(dimPeriod)
}

// Retired
/*


void PatternGenerator::WriteDimPattern_StepBarbell(uint8_t* outputPattern) {
  uint8_t spacing = dimPeriod - 2*transLength - brightLength - 2;
  uint8_t adjSpacing = spacing;
  uint8_t adjBrightLength = brightLength / 2;
  uint8_t adjTransLength = transLength;
  if(brightLength % 2 == 0) { adjTransLength++; }
  else { adjBrightLength++; adjSpacing++; }

  uint8_t i = 0;
  uint8_t limit;
  for(limit = adjSpacing; i < limit; i++) {
    outputPattern[i] = 0;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    outputPattern[i] = 255;
  }
  
  for(limit += 2*adjTransLength; i < limit; i++) {
    outputPattern[i] = 32;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    outputPattern[i] = 255;
  }
}
*/

