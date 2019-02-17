#ifndef ESP_LIGHT_DEVICE_CONTROL_H
#define ESP_LIGHT_DEVICE_CONTROL_H

#include <map>
#include <FastLED.h>
#include "Config.h"

#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

#define EFFECT_SPARKLES "Sparkles"
#define EFFECT_RAINBOW "Rainbow"
#define EFFECT_POLICE "Police"
#define EFFECT_COLORLOOP "Colorloop"
#define EFFECT_CHAOS "Chaos"
#define EFFECT_RANDOMPIXELS "RandomPixels"
#define EFFECT_FIRE "Fire"
#define EFFECT_STOP "Stop"

struct ChaosEffectHelper {
  uint8_t hue;
  uint8_t saturation;
  uint8_t brightness;
  uint8_t maxBrightness;
  bool up;
};

/*
 * This class may differ from one device to another 
 * depending on how the device is controled.
 */
class DeviceControl
{
public:
  DeviceControl();
  ~DeviceControl();

  /* 
   * Initialize the device. 
   * The function must be called once in setup() 
   */
  void initDevice();

  /*
   * Set a reference to the device state structure.
   * This must be called in setup() before sending/receiving any data.
   */
  void setDeviceStateReference(struct DeviceState *deviceState) {
    m_deviceState = deviceState;
  }

  /*
   * The function applies all changes from 
   * from the device state structure
   */
  void updateDeviceState();

  /*
   * The function switches the current effect.
   * If there was no any running effect, it starts from the beginning.
   */
  void nextEffect();

  /* 
   * Run the device.
   * It must be called in loop()
   */
  void run();

  /* Effects */
  void efSparkles();
  void efRainbow();
  void efPolice();
  void efColorLoop();
  void efChaos();
  void efRandomPixels();
  void efFire();

private:
  void transition();
  float m_current_brightness;
  bool m_inTransition;

  void setColor(byte red, byte green, byte blue, byte brightness);
  
  CRGB leds[NUM_LEDS];
  struct DeviceState *m_deviceState;
  std::string m_currentEffect;

  /* Effects */
  uint8_t ef_hue = 0;
  bool ef_gReverseDirection = false;
  bool ef_police_switch = true;
  ChaosEffectHelper chaosEffectHelper[NUM_LEDS];

  std::map< std::string, std::function<void(void)> > m_lightEffectsList = {
    {"Sparkles",       std::bind(&DeviceControl::efSparkles, this)},
    {"Rainbow",        std::bind(&DeviceControl::efRainbow, this)},
    {"Police",         std::bind(&DeviceControl::efPolice, this)},
    {"Colorloop",      std::bind(&DeviceControl::efColorLoop, this)},
    {"Chaos",          std::bind(&DeviceControl::efChaos, this)},
    {"RandomPixels",   std::bind(&DeviceControl::efRandomPixels, this)},
    {"Fire",           std::bind(&DeviceControl::efFire, this)},
  };
  uint8_t m_currentEffectIndex = 0;
};

#endif //ESP_LIGHT_DEVICE_CONTROL_H
