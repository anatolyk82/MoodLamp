# MoodLamp

![Lamp](doc/lamp.jpg)

The lamp is controlled via mqtt protocol. It has 88 WS2811 LEDs located on a square aluminium profile on every side with 22 LEDs ona side. The LEDs are controlled by ESP8266 (Wemos D1 mini board) All device settings (the lamp) as well as mqtt topics can be configured in `Config.h`.

### Home Assistant configuration
```
- platform: mqtt
  name: 'Mood Lamp'
  schema: json
  rgb: true
  brightness: true
  command_topic: "light/moodlamp/set"
  state_topic: "light/moodlamp"
  availability_topic: "light/moodlamp/status"
  payload_available: "online"
  payload_not_available: "offline"
  optimistic: false
  qos: 0
  effect: true
  effect_list:
  - Sparkles
  - Rainbow
  - Colorloop
  - Police
  - Chaos
  - RandomPixels
  - Fire
  - Stop
```
The lamp supports  several visual effect which are specified in the configuration. Code of all effects are in the class `DeviceControl` in files `DeviceControl.h` `DeviceControl.cpp`. The mqtt topics is configured in `Config.h`. As a payload of mqtt messages the lamp sends JSON documents. The structure of the document which the lamp sends every `INTERVAL_PUBLISH_STATE` milliseconds is:
```
{
  "state": "ON",
  "effect": "EffectName",
  "brightness": 255,
  "color": { "r": 255, "g": 255, "b": 255 },
  "transition": 10,
  "ip": "192.168.1.110",
  "mac": "11:22:33:44:55:66",
  "rssi": "-67",
  "uptime": "12:23:33"
}
```

The command JSON document is similar to the one which is shown above. For example, if it's required to turn on the lamp at the red color with 50% brightness, the JSON document will look like:
```
{
  "state": "ON",
  "brightness": 128,
  "color": { "r": 255, "g": 0, "b": 0 }
}
```
To turn on the fire effect, for example:
```
{
  "state": "ON",
  "effect": "Fire",
}
```
The state of the lamp must be sent in every command message. Ti turn off the lamp, JSON will be like:
```
{
  "state": "OFF"
}
```

### Button
 The lamp has a touch button to switch effects. All logic of the button is described in `LightControllButton.h` / `LightControllButton.cpp`
