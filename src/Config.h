#ifndef ESP_LIGHT_CONFIG_H
#define ESP_LIGHT_CONFIG_H

#include <string>

#define FIRMWARE_VERSION "0.3.2" /* Firmware version */

#define INTERVAL_PUBLISH_STATE 600000 /* Interval to send statistics to the mqtt broker */

/* MQTT Settings */
#define MQTT_TOPIC_STATE "light/moodlamp"           /* state report MQTT topic */
#define MQTT_TOPIC_SET "light/moodlamp/set"         /* command MQTT topic */

#define MQTT_TOPIC_STATUS "light/moodlamp/status"   /* status MQTT topic: online/offline */
#define MQTT_STATUS_PAYLOAD_ON "online"
#define MQTT_STATUS_PAYLOAD_OFF "offline"

#define MQTT_KEEP_ALIVE_SECONDS 30

//Uncomment to enable button control
//#define LAMP_CONTROL_BUTTON_ENABLED 1

/* WiFi Manager settings */
#define WIFI_AP_NAME "MoodLamp"
#define WIFI_AP_PASS "123456789"

/* Device settings */
#define DATA_PIN    D1
#define NUM_LEDS    88
#define NUM_LEDS_ONE_SIDE 22
#define NUM_LED_SIDES 4


/* Button pin */
#define BUTTON_PIN D0

/* The structure holds the current state of the device */
struct DeviceState {
  bool state = false;

  uint8_t red = 255;
  uint8_t green = 255;
  uint8_t blue = 255;
  uint8_t brightness = 128;

  int transition;

  std::string effect;

  uint8_t effect_fire_cooling = 15;
  uint8_t effect_fire_sparking = 80;
};


/* Calculates uptime for the device */
inline char *uptime(unsigned long milli) {
  static char _return[32];
  unsigned long secs=milli/1000, mins=secs/60;
  unsigned int hours=mins/60, days=hours/24;
  milli-=secs*1000;
  secs-=mins*60;
  mins-=hours*60;
  hours-=days*24;
  sprintf(_return,"%dT%2.2d:%2.2d:%2.2d.%3.3d", (byte)days, (byte)hours, (byte)mins, (byte)secs, (int)milli);
  return _return;
}

#endif //ESP_LIGHT_CONFIG_H
