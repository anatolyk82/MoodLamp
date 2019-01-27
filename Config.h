#ifndef ESP_LIGHT_CONFIG_H
#define ESP_LIGHT_CONFIG_H

#define FIRMWARE_VERSION "0.1.1"

#define INTERVAL_PUBLISH_STATE 600000 // Interval to send statistics to the mqtt broker

/* MQTT Settings */
#define MQTT_TOPIC_STATE "light/moodlamp"
#define MQTT_TOPIC_SET "light/moodlamp/set"

#define MQTT_TOPIC_STATUS "light/moodlamp/status"
#define MQTT_STATUS_PAYLOAD_ON "online"
#define MQTT_STATUS_PAYLOAD_OFF "offline"

#define MQTT_KEEP_ALIVE_SECONDS 30


/* WiFi Manager settings */
#define WIFI_AP_NAME "MoodLamp"
#define WIFI_AP_PASS "123456789"


/* Device settings */
#define DATA_PIN    D1
#define NUM_LEDS    9

struct DeviceState {
  bool state = false;
  
  byte red = 255;
  byte green = 255;
  byte blue = 255;
  byte brightness = 128;

  int transition;

  String effect;
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
