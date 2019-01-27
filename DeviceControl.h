#ifndef ESP_LIGHT_DEVICE_CONTROL_H
#define ESP_LIGHT_DEVICE_CONTROL_H

#include <FastLED.h>

#define DATA_PIN    D1
#define NUM_LEDS    9
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

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
   * Run the device.
   * It must be called in loop()
   */
  void run();

private:
  void transition();
  float m_current_brightness;
  bool m_inTransition;

  void setColor(byte red, byte green, byte blue, byte brightness);
  
  CRGB leds[NUM_LEDS];
  struct DeviceState *m_deviceState;
  String m_currentEffect;
};

#endif //ESP_LIGHT_DEVICE_CONTROL_H
