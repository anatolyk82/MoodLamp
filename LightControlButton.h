#ifndef ESP_LIGHT_BUTTON_H
#define ESP_LIGHT_BUTTON_H

#define DEBOUNCE_DELAY          50
#define REPEAT_DELAY            500
#define PRESS_AND_HOLD_DELAY    1000

#include <functional>
#include <Arduino.h>
#include "Config.h"

class LightControlButton
{
public:
  LightControlButton();
  ~LightControlButton();

  /*
   * Set a reference to the device state structure.
   * This must be called in setup() before sending/receiving any data.
   */
  void setDeviceStateReference(struct DeviceState *deviceState) {
    m_deviceState = deviceState;
  }


  /*
   * The function sets a callback function which will be called
   * whenever the button is pressed.
   */
  void onDeviceStateUpdate(std::function<void(void)> updateDeviceState) {
    m_updateDeviceState = updateDeviceState;
  }

  /* 
   * Init the button.
   * It must be called in setup()
   */
  void init(uint8_t);

  /* 
   * Run the button.
   * It must be called in loop()
   */
  void run();

private:
  void onClicked();
  void onDoubleClicked();
  void onTripleClicked();
  void onPressAndHold();

  const uint8_t m_defaultButtonStatus = 0;
  uint8_t m_buttonStatus;
  uint8_t m_pin;
  
  unsigned long m_eventStart;
  unsigned long m_eventLength;
  bool m_resetCount = true;
  unsigned char m_eventCount = 0;
  bool m_ready = false;

  unsigned long m_eventPressAndHoldStart = 0;
  
  std::function<void(void)> m_updateDeviceState;
  struct DeviceState *m_deviceState;
};

#endif
