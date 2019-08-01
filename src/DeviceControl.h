#ifndef ESP_LIGHT_DEVICE_CONTROL_H
#define ESP_LIGHT_DEVICE_CONTROL_H

#include <map>
#include <FastLED.h>
#include "Config.h"

#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

struct ChaosEffectHelper {
  uint8_t hue;
  uint8_t saturation;
  uint8_t brightness;
  uint8_t maxBrightness;
  bool up;
};

#define qsubd(x, b) ((x>b) ? b : 0)
#define qsuba(x, b) ((x>b) ? x-b : 0)

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
  void efAlarm();
  void efColorLoop();
  void efChaos();
  void efRandomPixels();
  void efFire();
  void efPlasmaLamp();

private:
  void transition();
  float m_current_brightness;
  bool m_inTransition;

  void setColor(byte red, byte green, byte blue, byte brightness);

  CRGB leds[NUM_LEDS];
  struct DeviceState *m_deviceState;

  /* Effects */
  uint8_t ef_hue = 0;
  bool ef_gReverseDirection = false;
  bool ef_police_switch = true;
  uint8_t ef_police_stage = 0;
  ChaosEffectHelper chaosEffectHelper[NUM_LEDS];
  int8_t ef_alarmDelta = 5;
  uint8_t ef_alarmValue = 0;

  CRGBPalette16 ef_PlasmaCurrentPalette;
  CRGBPalette16 ef_PlasmaTargetPalette;

  std::map< std::string, std::function<void(void)> > m_lightEffectsList = {
    {"Sparkles",       std::bind(&DeviceControl::efSparkles, this)},
    {"Rainbow",        std::bind(&DeviceControl::efRainbow, this)},
    {"Police",         std::bind(&DeviceControl::efPolice, this)},
    {"Alarm",          std::bind(&DeviceControl::efAlarm, this)},
    {"Colorloop",      std::bind(&DeviceControl::efColorLoop, this)},
    {"Chaos",          std::bind(&DeviceControl::efChaos, this)},
    {"RandomPixels",   std::bind(&DeviceControl::efRandomPixels, this)},
    {"Fire",           std::bind(&DeviceControl::efFire, this)},
    {"PlasmaLamp",     std::bind(&DeviceControl::efPlasmaLamp, this)},
    {"NextEffect",     std::bind(&DeviceControl::nextEffect, this)}
  };
  std::string m_lastActiveEffect;
};

#endif //ESP_LIGHT_DEVICE_CONTROL_H
