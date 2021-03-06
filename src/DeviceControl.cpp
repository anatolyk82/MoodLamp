#include "DeviceControl.h"
#include "Config.h"

DeviceControl::DeviceControl() :
  m_inTransition(false),
  m_deviceState(nullptr)
{
}


DeviceControl::~DeviceControl()
{
}


void DeviceControl::initDevice()
{
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
}

void DeviceControl::setColor(byte red, byte green, byte blue, byte brightness)
{
  byte realRed = map(red, 0, 255, 0, brightness);
  byte realGreen = map(green, 0, 255, 0, brightness);
  byte realBlue = map(blue, 0, 255, 0, brightness);
  fill_solid( leds, NUM_LEDS, CRGB(realRed, realGreen, realBlue) );
}

void DeviceControl::updateDeviceState()
{
  Serial.printf("DeviceControl: Update state. Transition: %d\n", m_deviceState->transition);

  if (m_deviceState->transition != 0) {
    m_current_brightness = m_deviceState->state ? 0 : m_deviceState->brightness;
    m_inTransition = true;
    return;
  }

  if (m_deviceState->state) {
    Serial.printf("DeviceControl: red:[%d], green:[%d], blue:[%d]\n", m_deviceState->red, m_deviceState->green, m_deviceState->blue);
    Serial.printf("DeviceControl: brightness:[%d]\n", m_deviceState->brightness);

    if ( (m_deviceState->effect != "") && (m_lightEffectsList.find(m_deviceState->effect) != m_lightEffectsList.end()) ) {
      FastLED.clear();
    } else {
      this->setColor(m_deviceState->red, m_deviceState->green, m_deviceState->blue, m_deviceState->brightness);
    }
  } else {
    Serial.println("DeviceControl: Switch the device off");
    m_deviceState->effect = "";
    FastLED.clear();
  }
  FastLED.show();
}


void DeviceControl::transition() {
  if (m_deviceState->state) { // Transition to ON
    m_current_brightness = m_current_brightness + (50.0/1000.0)*(float)m_deviceState->brightness/(float)m_deviceState->transition;
    if ( m_current_brightness >= m_deviceState->brightness) { // Stop transition
      m_deviceState->transition = 0;
      m_current_brightness = m_deviceState->brightness;
      m_inTransition = false;
    }
  } else { // Transition to OFF
    m_current_brightness = m_current_brightness - (50.0/1000.0)*(float)m_deviceState->brightness/(float)m_deviceState->transition;
     if ( m_current_brightness <= 0) { // Stop transition
      m_deviceState->transition = 0;
      m_current_brightness = 0;
      m_inTransition = false;
    }
  }
  this->setColor(m_deviceState->red, m_deviceState->green, m_deviceState->blue, m_current_brightness);
  FastLED.delay(50);
}


void DeviceControl::run()
{
  if ( m_inTransition ) {
    this->transition();
  } else {
    if (m_lightEffectsList.find(m_deviceState->effect) != m_lightEffectsList.end()) {
      m_lightEffectsList[ m_deviceState->effect ]();
    } else {
      m_deviceState->effect = "";
    }
  }

  FastLED.show();
}


void DeviceControl::nextEffect() {
  std::map< std::string, std::function<void(void)> >::iterator it;

  Serial.printf("NextEffect: Last active effect: %s\n", m_lastActiveEffect.c_str());
  if (m_lastActiveEffect != "") {
    /* Find the last avtive effect */
    for ( it = m_lightEffectsList.begin(); it != m_lightEffectsList.end(); it++ ) {
      if (it->first == m_lastActiveEffect) {
        if ((++it) != m_lightEffectsList.end()) {
          m_deviceState->effect = it->first;
        } else {
          m_deviceState->effect = m_lightEffectsList.begin()->first;
        }
        break;
      }
    }
  } else {
    m_deviceState->effect = m_lightEffectsList.begin()->first;
  }
  m_lastActiveEffect = m_deviceState->effect;
  Serial.printf("NextEffect: Apply effect: %s\n", m_deviceState->effect.c_str());
}

/* --------------------------------------------------------------- */

void DeviceControl::efSparkles()
{
  byte randomPixel = random(0, NUM_LEDS);
  // With a probability 25% if a pixel is black, generate a new one.
  uint32_t pixelColor = (((uint32_t)leds[randomPixel].red << 16) | ((long)leds[randomPixel].green << 8 ) | (long)leds[randomPixel].blue);
  if ( (pixelColor == 0) && (random(0,3) == 0) ) {
    leds[randomPixel] = CHSV(random(0, 255), 255, random(150, 255) );
  }
  for (byte j=0; j<NUM_LEDS; j++) {
    leds[j].fadeToBlackBy(1);
  }
  FastLED.delay(50);
}

void DeviceControl::efRainbow()
{
  //static uint8_t hue=0;
  int deltaHue = 255 / NUM_LEDS_ONE_SIDE;
  for (uint8_t l=0; l < 4; l++) {
    fill_rainbow( leds + l*NUM_LEDS_ONE_SIDE, NUM_LEDS_ONE_SIDE, ef_hue, deltaHue );
  }
  ef_hue += 5;
  FastLED.delay(150);
}

void DeviceControl::efPolice()
{
  CRGB color = ef_police_switch ? CRGB(255, 0, 0) : CRGB(0, 0, 255);
  uint8_t shift1 = ef_police_switch ? 0 : 1;
  uint8_t shift2 = ef_police_switch ? 2 : 3;

  fill_solid( leds, NUM_LEDS, CRGB(0, 0, 0) );
  if (ef_police_stage == 0) {
    fill_solid( leds + shift1*NUM_LEDS_ONE_SIDE, NUM_LEDS_ONE_SIDE, color );
    fill_solid( leds + shift2*NUM_LEDS_ONE_SIDE, NUM_LEDS_ONE_SIDE, color );
    FastLED.delay(30);
  } else if (ef_police_stage == 1) {
    FastLED.delay(30);
  } else if (ef_police_stage == 2) {
    fill_solid( leds + shift1*NUM_LEDS_ONE_SIDE, NUM_LEDS_ONE_SIDE, color );
    fill_solid( leds + shift2*NUM_LEDS_ONE_SIDE, NUM_LEDS_ONE_SIDE, color );
    FastLED.delay(100);
  } else {
    FastLED.delay(400);
  }

  ef_police_stage = ef_police_stage == 3 ? 0 : ef_police_stage + 1;
  if (!ef_police_stage) {
    ef_police_switch = !ef_police_switch;
  }
}


void DeviceControl::efAlarm()
{
  CHSV clr1 = CHSV(0, 255, ef_alarmValue);
  CHSV clr2 = CHSV(160, 255, 255 - ef_alarmValue);

  fill_solid( leds, NUM_LEDS_ONE_SIDE, clr1 );
  fill_solid( leds + 2*NUM_LEDS_ONE_SIDE, NUM_LEDS_ONE_SIDE, clr1 );

  fill_solid( leds + 1*NUM_LEDS_ONE_SIDE, NUM_LEDS_ONE_SIDE, clr2 );
  fill_solid( leds + 3*NUM_LEDS_ONE_SIDE, NUM_LEDS_ONE_SIDE, clr2 );

  ef_alarmValue = ef_alarmValue + ef_alarmDelta;

  if (ef_alarmValue == 0) {
    ef_alarmDelta = 5;
  } else if (ef_alarmValue == 255) {
    ef_alarmDelta = -5;
  }

  FastLED.delay(10);
}


void DeviceControl::efColorLoop()
{
  fill_solid( leds, NUM_LEDS, CHSV( ef_hue, 255, m_deviceState->brightness) );
  ef_hue += 1;
  FastLED.delay(500); //50
}

void DeviceControl::efChaos()
{
  for (int i = 0; i < NUM_LEDS; i++) {
    if ( (chaosEffectHelper[i].up == false) && (chaosEffectHelper[i].brightness == 0) ) {
      chaosEffectHelper[i].hue = random(0, 255);
      chaosEffectHelper[i].saturation = 255;
      chaosEffectHelper[i].brightness = 0;
      chaosEffectHelper[i].maxBrightness = random(100, 255);
      chaosEffectHelper[i].up = true;
    } else if ((chaosEffectHelper[i].up == true) && (chaosEffectHelper[i].brightness >= 0)) {
      chaosEffectHelper[i].brightness = chaosEffectHelper[i].brightness + 1;
      chaosEffectHelper[i].up = chaosEffectHelper[i].brightness < chaosEffectHelper[i].maxBrightness;
    } else if ((chaosEffectHelper[i].up == false) && (chaosEffectHelper[i].brightness > 0)) {
      chaosEffectHelper[i].brightness = chaosEffectHelper[i].brightness - 1;
    }
  }
  for (byte j=0; j<NUM_LEDS; j++) {
    leds[j] = CHSV(chaosEffectHelper[j].hue, chaosEffectHelper[j].saturation, chaosEffectHelper[j].brightness );
  }
  FastLED.delay(20);
}

void DeviceControl::efRandomPixels()
{
  static int countToCreateNewPixel = 0;
  static byte nToCreateNewPixel = 30;

  for (byte j=0; j<NUM_LEDS; j++) {
    leds[j].fadeToBlackBy(1);
  }

  if (countToCreateNewPixel == 0) {
    byte i = 0;
    do {
      i = random(NUM_LEDS);
    } while ( (((uint32_t)leds[i].red << 16) | ((long)leds[i].green << 8 ) | (long)leds[i].blue) != 0 );
    leds[i] = CHSV( random(255), 255, random(70,255));
    nToCreateNewPixel = random(10,70); // make the interval between two dots different
  }

  countToCreateNewPixel = (countToCreateNewPixel + 1) % nToCreateNewPixel;
  FastLED.delay(150);
}

void DeviceControl::efFire()
{
  // Add entropy to random number generator; we use a lot of it.
  //random16_add_entropy( random() );

  // run simulation frame
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS_ONE_SIDE];

  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS_ONE_SIDE; i++) {
    heat[i] = qsub8( heat[i],  (random8(0, ((m_deviceState->effect_fire_cooling * 10) / NUM_LEDS_ONE_SIDE) + 2) +i*15) );
  }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS_ONE_SIDE - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < m_deviceState->effect_fire_sparking ) {
      int y = random8(3);
      heat[y] = qadd8( heat[y], random8(100,200) );
    }

     // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS_ONE_SIDE; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Yellow ), colorindex);
      int pixelnumber;
      if( ef_gReverseDirection ) {
        pixelnumber = (NUM_LEDS_ONE_SIDE-1) - j;
      } else {
        pixelnumber = j;
      }
      for (uint8_t l=0; l < 4; l++) {
        leds[pixelnumber + l*NUM_LEDS_ONE_SIDE] = color;
      }
    }

    FastLED.delay(50);
}

// This effect is based on: https://github.com/atuline/FastLED-Demos/tree/master/plasma
void DeviceControl::efPlasmaLamp()
{
  EVERY_N_MILLISECONDS(50) {
    int thisPhase = beatsin8(6,-64,64);
    int thatPhase = beatsin8(7,-64,64);

    for (int k=0; k<NUM_LEDS_ONE_SIDE; k++) {
      int colorIndex = cubicwave8((k*23)+thisPhase)/2 + cos8((k*15)+thatPhase)/2;
      int thisBright = qsuba(colorIndex, beatsin8(7,0,96));
      for (int s = 0; s < NUM_LED_SIDES; s++) {
        leds[k + s*NUM_LEDS_ONE_SIDE] = ColorFromPalette(ef_PlasmaCurrentPalette, colorIndex, thisBright, LINEARBLEND);
      }
    }
  }

  EVERY_N_MILLISECONDS(100) {
    uint8_t maxChanges = 24;
    nblendPaletteTowardPalette(ef_PlasmaCurrentPalette, ef_PlasmaTargetPalette, maxChanges);
  }

  EVERY_N_SECONDS(5) {
    uint8_t baseC = random8();
    ef_PlasmaTargetPalette = CRGBPalette16(CHSV(baseC+random8(32), 192, random8(128,255)), CHSV(baseC+random8(32), 255, random8(128,255)), CHSV(baseC+random8(32), 192, random8(128,255)), CHSV(baseC+random8(32), 255, random8(128,255)));
  }
}
