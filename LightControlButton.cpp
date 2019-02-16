#include "LightControlButton.h"

LightControlButton::LightControlButton()
{
}


LightControlButton::~LightControlButton()
{
}


void LightControlButton::init(uint8_t pin)
{
  m_pin = pin;
  pinMode(pin, INPUT);
  m_buttonStatus = m_defaultButtonStatus;
}

void LightControlButton::run()
{
  if (digitalRead(m_pin) != m_buttonStatus) {
    // Debounce
    unsigned long start = millis();
    while ((millis() - start) < DEBOUNCE_DELAY) delay(1);

    if (digitalRead(m_pin) != m_buttonStatus) {
      m_buttonStatus = !m_buttonStatus;

      // released
      if (m_buttonStatus == m_defaultButtonStatus) {
        m_eventLength = millis() - m_eventStart;
        m_ready = true;
        m_eventPressAndHoldStart = 0;
      } else { // pressed
        m_eventStart = millis();
        m_eventPressAndHoldStart = m_eventStart;
        m_eventLength = 0;
        if (m_resetCount) {
          m_eventCount = 1;
          m_resetCount = false;
        } else {
          m_eventCount++;
        }
        m_ready = false;
      }
    }
  }

  /* Detect the press-and-hold event */
  if ( ((millis() - m_eventPressAndHoldStart) > PRESS_AND_HOLD_DELAY) && (!m_ready) && (m_eventPressAndHoldStart > 0) ) {
    Serial.printf("LightControlButton: Press And Hold event\n");
    m_eventPressAndHoldStart = millis();
    this->onPressAndHold();
  }

  /* A click is done */
  if (m_ready && ((millis() - m_eventStart) > REPEAT_DELAY)) {
    m_ready = false;
    m_resetCount = true;

    Serial.printf("LightControlButton: Click duration: %d; Event count: %d\n", m_eventLength, m_eventCount);

    if (m_eventCount == 1) {
      if (m_eventLength > 150) {
        this->onClicked();
      }
    } else if (m_eventCount == 2) {
      this->onDoubleClicked();
    } else if (m_eventCount == 3) {
      this->onTripleClicked();
    }
  }
}



void LightControlButton::onClicked()
{
  if (m_deviceState) {
    m_deviceState->state = !m_deviceState->state;

    /* Update actual state of the device*/
    if (m_updateDeviceState) {
      m_updateDeviceState();
    }
  }
}

void LightControlButton::onDoubleClicked()
{
  // Double click: change effect
}

void LightControlButton::onTripleClicked()
{
  // Triple click: change color
}

void LightControlButton::onPressAndHold()
{
  //TODO: Change brightness
}
