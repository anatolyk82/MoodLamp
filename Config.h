#ifndef ESP_LIGHT_CONFIG_H
#define ESP_LIGHT_CONFIG_H

#define FIRMWARE_VERSION "0.1.0"

#define INTERVAL_PUBLISH_STATE 600000 // Interval to send statistics to the mqtt broker

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
