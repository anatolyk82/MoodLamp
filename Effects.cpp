#include "Effects.h"
#include "DeviceControl.h"

namespace Effects {
  
  void sparkles(CRGB *leds, int numLEDs) {
    byte randomPixel = random(0, numLEDs);
    // With a probability 25% if a pixel is black, generate a new one.
    uint32_t pixelColor = (((uint32_t)leds[randomPixel].red << 16) | ((long)leds[randomPixel].green << 8 ) | (long)leds[randomPixel].blue);
    if ( (pixelColor == 0) && (random(0,3) == 0) ) {
      leds[randomPixel] = CHSV(random(0, 255), 255, random(150, 255) );
    }
    for (byte j=0; j<numLEDs; j++) {
      leds[j].fadeToBlackBy(1);
    }
    FastLED.delay(50);
  }

  struct chaosHelper {
    byte hue;
    byte saturation;
    byte brightness;
    byte maxBrightness;
    bool up;
  };

  void chaos(CRGB *leds, int numLEDs) {
    static struct chaosHelper helpBuffer[9]; //TODO:

    for (int i = 0; i < numLEDs; i++) {
      if ( (helpBuffer[i].up == false) && (helpBuffer[i].brightness == 0) ) {
        helpBuffer[i].hue = random(0, 255);
        helpBuffer[i].saturation = 255;
        helpBuffer[i].brightness = 0;
        helpBuffer[i].maxBrightness = random(100, 255);
        helpBuffer[i].up = true;
      } else if ((helpBuffer[i].up == true) && (helpBuffer[i].brightness >= 0)) {
        helpBuffer[i].brightness = helpBuffer[i].brightness + 1;
        helpBuffer[i].up = helpBuffer[i].brightness < helpBuffer[i].maxBrightness;
      } else if ((helpBuffer[i].up == false) && (helpBuffer[i].brightness > 0)) {
        helpBuffer[i].brightness = helpBuffer[i].brightness - 1;
      }
    }
    for (byte j=0; j<numLEDs; j++) {
      leds[j] = CHSV(helpBuffer[j].hue, helpBuffer[j].saturation, helpBuffer[j].brightness );
    }
    FastLED.delay(20);
  }


  void rainbow(CRGB *leds, int numLEDs) {
    static uint8_t hue=0;
    int deltaHue = 255 / numLEDs;
    fill_rainbow( leds, numLEDs, hue, deltaHue );
    hue += 5;
    FastLED.delay(50);
  }

  
  void runner(CRGB *leds, int numLEDs, byte initRed, byte initGreen, byte initBlue) {
    static byte index = 0;
    static bool goingUp = true;
    fill_solid( leds, numLEDs, CRGB( initRed/5, initGreen/5, initBlue/5) );
    
    leds[index] = CRGB( initRed, initGreen, initBlue);
    //leds[index-1] = CRGB( initRed/2, initGreen/2, initBlue/2);
    //leds[index+1] = CRGB( initRed/2, initGreen/2, initBlue/2);

    if (index == 0) {
      goingUp = true;
    } else if (index == (numLEDs-1)) {
      goingUp = false;
    }

    index = goingUp ? (index + 1) : (index - 1);

    FastLED.delay(100);
  }


  void alarm(CRGB *leds, int numLEDs) {
    fill_solid( leds, numLEDs, CHSV( 0, 255, 255) );
    FastLED.show();
    FastLED.delay(200);
    FastLED.clear();
    //FastLED.show();
    FastLED.delay(3000);
  }

  
  void police(CRGB *leds, int numLEDs) {
    static byte index = 0;
    static int half = floor(numLEDs/2);

    if (index % 2) {
      for (int i = 0; i < half; i++) {
        leds[i] = CHSV(0, 255, 255 - (255/(half + 1))*i); //hue = 0 -> Red
        leds[numLEDs-i-1] = CHSV(170, 255, 255 - (255/(half + 1))*i); //hue = 170 -> Blue
      }
    } else {
      for (int i = 0; i < half; i++) {
        leds[numLEDs-i-1] = CHSV(0, 255, 255 - (255/(half + 1))*i); //hue = 0 -> Red
        leds[i] = CHSV(170, 255, 255 - (255/(half + 1))*i); //hue = 170 -> Red
      }
    }

    index += 1;
    FastLED.delay(300);
    FastLED.clear();
  }


  void colorloop(CRGB *leds, int numLEDs, byte brightness) {
    static uint8_t hue=0;
    fill_solid( leds, numLEDs, CHSV( hue, 255, brightness) );
    hue += 1;
    FastLED.delay(50);
  }


  void randomPixels(CRGB *leds, int numLEDs) {
    static int countToCreateNewPixel = 0;
    static byte nToCreateNewPixel = 50;
  
    for (byte j=0; j<numLEDs; j++) {
      leds[j].fadeToBlackBy(1);
    }

    if (countToCreateNewPixel == 0) {
      byte i = 0;
      do {
        i = random(numLEDs);
      } while ( (((uint32_t)leds[i].red << 16) | ((long)leds[i].green << 8 ) | (long)leds[i].blue) != 0 );
      leds[i] = CHSV( random(255), 255, random(70,255));
      nToCreateNewPixel = random(10,70); // make the interval between two dots different 
    }

    countToCreateNewPixel = (countToCreateNewPixel + 1) % nToCreateNewPixel;
    FastLED.delay(150);
  }


  //TODO: make 3 random colors
  void juggle(CRGB *leds, int numLEDs)
  {
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy( leds, numLEDs, 20 );
    byte dothue = 0;
    for( int i = 0; i < 3; i++) {
      leds[beatsin16( i+3, 0, numLEDs-1 )] |= CHSV(dothue, 255, 255);
      dothue += 80;
    }
  }

  void sinelon(CRGB *leds, int numLEDs)
  {
    static uint8_t gHue = 0;
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy( leds, numLEDs, 20);
    int pos = beatsin16( 13, 0, numLEDs-1 );
    leds[pos] += CHSV( gHue, 255, 192);
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  }



  void breathe(CRGB *leds, int numLEDs)
  {
    const float pulseSpeed = 0.4;  // Larger value gives faster pulse.
    const byte hueA = 66;          // Start hue at valueMin.
    const byte satA = 200;         // Start saturation at valueMin.
    const float valueMin = 120.0;  // Pulse minimum value (Should be less then valueMax).

    const byte hueB = 83;          // End hue at valueMax.
    const byte satB = 255;         // End saturation at valueMax.
    const float valueMax = 200.0;  // Pulse maximum value (Should be larger then valueMin).

    static uint8_t hue = hueA;     // Do Not Edit
    static uint8_t sat = satA;     // Do Not Edit
    static float val = valueMin;   // Do Not Edit
    float delta = (valueMax - valueMin) / 2.35040238;  // Do Not Edit

    float dV = ((exp(sin(pulseSpeed * millis()/2000.0*PI)) -0.36787944) * delta);
    val = valueMin + dV;
    hue = map(val, valueMin, valueMax, hueA, hueB);  // Map hue based on current val
    sat = map(val, valueMin, valueMax, satA, satB);  // Map sat based on current val

    for (int i = 0; i < numLEDs; i++) {
      leds[i] = CHSV(hue, sat, val);
    }
    FastLED.delay(20);
  }


  void drop(CRGB *leds, int numLEDs)
  {
    static uint16_t dropIndex = 0;
    static CHSV dropColor = CHSV(random(255), 255, 255);
    
    EVERY_N_MILLISECONDS(250) {
      leds[numLEDs - dropIndex - 1] = dropColor;
      dropIndex = (dropIndex + 1) % numLEDs;
      if (dropIndex == 0) {
        dropColor = CHSV(random(255), 255, 255);
      }
    }
    fadeToBlackBy( leds, numLEDs, 1 );
    FastLED.delay(10);
  }


  void tower(CRGB *leds, int numLEDs)
  {
    static bool generatePixel = true;
    static int16_t topPixelIndex = -1;
    static int16_t newPixelIndex = 0;
    static uint16_t filledCounter = 10001;

    if (filledCounter < 280) {
      fadeToBlackBy( leds, numLEDs, 1 );
      FastLED.delay(100);
      filledCounter = filledCounter + 1;
      return;
    } else {
      if (generatePixel) {
        newPixelIndex = numLEDs-1;
        leds[newPixelIndex] = CHSV(random(255), 255, 255);
        generatePixel = false;
        FastLED.delay(400);
      } else {
        EVERY_N_MILLISECONDS( 300 ) {
          if ( newPixelIndex == (topPixelIndex + 1) ) {
            topPixelIndex = topPixelIndex + 1;
            generatePixel = true;
          } else {
            leds[newPixelIndex-1] = leds[newPixelIndex];
            newPixelIndex = newPixelIndex - 1;
          }
        }
      }

      if (topPixelIndex == (numLEDs-1)) {
        filledCounter = 0;
        generatePixel = true;
        topPixelIndex = -1;
      }

      for (byte i = newPixelIndex+1; i < numLEDs; i++) {
        leds[i].fadeToBlackBy(2);
      }
    }
  }
  

//--------------------------------------------


#define FIRE_COOLING  15
#define FIRE_SPARKING 80
bool gReverseDirection = false;

  void fire(CRGB *leds, int numLEDs)
  {
    // Add entropy to random number generator; we use a lot of it.
    //random16_add_entropy( random() );

    // run simulation frame
    // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];

    // Step 1.  Cool down every cell a little
    for( int i = 0; i < numLEDs; i++) {
      heat[i] = qsub8( heat[i],  (random8(0, ((FIRE_COOLING * 10) / numLEDs) + 2) +i*15) );
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= numLEDs - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < FIRE_SPARKING ) {
      int y = random8(3);
      heat[y] = qadd8( heat[y], random8(100,200) );
    }

     // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < numLEDs; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Yellow ), colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (numLEDs-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
  
    FastLED.show(); // display this frame
    FastLED.delay(50);
  }
}
