#ifndef ESP_LIGHT_EFFECTS_H
#define ESP_LIGHT_EFFECTS_H

#include <FastLED.h>

#define EFFECT_SPARKLES "Sparkles"
#define EFFECT_RAINBOW "Rainbow"
#define EFFECT_ALARM "Alarm"
#define EFFECT_POLICE "Police"
#define EFFECT_COLORLOOP "Colorloop"
#define EFFECT_CHAOS "Chaos"
#define EFFECT_RUNNER "Runner"
#define EFFECT_RANDOMPIXELS "RandomPixels"
#define EFFECT_FIRE "Fire"
#define EFFECT_JUGGLE "Juggle"
#define EFFECT_SINELON "Sinelon"
#define EFFECT_BREATHE "Breathe"
#define EFFECT_TOWER "Tower"
#define EFFECT_DROP "Drop"
#define EFFECT_STOP "Stop"

namespace Effects {
  void sparkles(CRGB *leds, int numLEDs);
  void chaos(CRGB *leds, int numLEDs);
  void rainbow(CRGB *leds, int numLEDs);
  void runner(CRGB *leds, int numLEDs, byte initRed, byte initGreen, byte initBlue);
  void alarm(CRGB *leds, int numLEDs);
  void police(CRGB *leds, int numLEDs);
  void colorloop(CRGB *leds, int numLEDs, byte brightness);
  void randomPixels(CRGB *leds, int numLEDs);
  void fire(CRGB *leds, int numLEDs);
  void juggle(CRGB *leds, int numLEDs);
  void sinelon(CRGB *leds, int numLEDs);
  void breathe(CRGB *leds, int numLEDs);
  void tower(CRGB *leds, int numLEDs);
  void drop(CRGB *leds, int numLEDs);
};

#endif //ESP_LIGHT_EFFECTS_H
