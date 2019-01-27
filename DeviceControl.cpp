#include "DeviceControl.h"
#include "Config.h"
#include "Effects.h"

DeviceControl::DeviceControl() :
  m_deviceState(nullptr),
  m_inTransition(false),
  m_currentEffect("")
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

    if ((m_deviceState->effect != "") && (m_deviceState->effect != EFFECT_STOP)) {
      //Serial.printf("DeviceControl: Effect:[%s]\n", m_deviceState->effect.toCharArray());
      FastLED.clear();
      m_currentEffect = m_deviceState->effect;
    } else {
      m_currentEffect = "";
      this->setColor(m_deviceState->red, m_deviceState->green, m_deviceState->blue, m_deviceState->brightness);
    }
  } else {
    Serial.println("DeviceControl: Switch the device off");
    m_deviceState->effect != "";
    m_currentEffect = "";
    FastLED.clear();
  }
  FastLED.show();
}


void DeviceControl::transition() {
  EVERY_N_MILLISECONDS(50) {
    if (m_deviceState->state) { // Transition to ON
      m_current_brightness = m_current_brightness + 50*(float)m_deviceState->brightness/(float)m_deviceState->transition;

      if ( m_current_brightness >= m_deviceState->brightness) { // Stop transition
        m_deviceState->transition = 0;
        m_current_brightness = m_deviceState->brightness;
        m_inTransition = false;
      }
    } else { // Transition to OFF
      m_current_brightness = m_current_brightness - 50*(float)m_deviceState->brightness/(float)m_deviceState->transition;

      if ( m_current_brightness <= 0) { // Stop transition
        m_deviceState->transition = 0;
        m_current_brightness = 0;
        m_inTransition = false;
      }
    }
    this->setColor(m_deviceState->red, m_deviceState->green, m_deviceState->blue, m_current_brightness);
  }
}


void DeviceControl::run()
{
  if ( m_inTransition ) {
    this->transition();
  } else {
    if (m_currentEffect == EFFECT_SPARKLES) {
      Effects::sparkles(leds, NUM_LEDS);
    } else if (m_currentEffect == EFFECT_RAINBOW) {
      Effects::rainbow(leds, NUM_LEDS);
    } else if (m_currentEffect == EFFECT_RUNNER) {
      Effects::runner(leds, NUM_LEDS, m_deviceState->red, m_deviceState->green, m_deviceState->blue);
    } else if (m_currentEffect == EFFECT_POLICE) {
      Effects::police(leds, NUM_LEDS);
    } else if (m_currentEffect == EFFECT_COLORLOOP) {
      Effects::colorloop(leds, NUM_LEDS, m_deviceState->brightness);
    } else if (m_currentEffect == EFFECT_ALARM) {
      Effects::alarm(leds, NUM_LEDS);
    } else if (m_currentEffect == EFFECT_CHAOS) {
      Effects::chaos(leds, NUM_LEDS);
    } else if (m_currentEffect == EFFECT_RANDOMPIXELS) {
      Effects::randomPixels(leds, NUM_LEDS);
    } else if (m_currentEffect == EFFECT_FIRE) {
      Effects::fire(leds, NUM_LEDS);
    } else if (m_currentEffect == EFFECT_JUGGLE) {
      Effects::juggle(leds, NUM_LEDS);
    } else if (m_currentEffect == EFFECT_SINELON) {
      Effects::sinelon(leds, NUM_LEDS);
    } else if (m_currentEffect == EFFECT_BREATHE) {
      Effects::breathe(leds, NUM_LEDS);
    } else if (m_currentEffect == EFFECT_TOWER) {
      Effects::tower(leds, NUM_LEDS);
    } else if (m_currentEffect == EFFECT_DROP) {
      Effects::drop(leds, NUM_LEDS);
    }
  }

  FastLED.show();
}
