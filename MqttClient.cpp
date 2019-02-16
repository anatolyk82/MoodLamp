#include "MqttClient.h"

DeviceMqttClient::DeviceMqttClient() : AsyncMqttClient()
  , m_updateDeviceState(nullptr)
  , m_deviceState(nullptr)
{
  onConnect( std::bind(&DeviceMqttClient::onMqttConnect, this, std::placeholders::_1) );
  onDisconnect( std::bind(&DeviceMqttClient::onMqttDisconnect, this, std::placeholders::_1) );
  onMessage( std::bind(&DeviceMqttClient::onMqttMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6) );
}

DeviceMqttClient::~DeviceMqttClient()
{
}

void DeviceMqttClient::onMqttConnect(bool sessionPresent)
{
  Serial.println("MQTT: Connected");
  Serial.printf("MQTT: Session present: %d\n", sessionPresent);

  Serial.printf("MQTT: Subscribing at QoS 0, topic: %s\n", MQTT_TOPIC_SET);
  subscribe(MQTT_TOPIC_SET, 0);

  Serial.printf("MQTT: Publish online status: %s\n", MQTT_TOPIC_STATUS);
  publish(MQTT_TOPIC_STATUS, 1, true, MQTT_STATUS_PAYLOAD_ON);

  publishDeviceState();
}

void DeviceMqttClient::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.println();
  Serial.print("MQTT: Disconnected: ");
  if (reason == AsyncMqttClientDisconnectReason::TCP_DISCONNECTED) {
    Serial.println("TCP disconnected");
  } else if (reason == AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION) {
    Serial.println("Unacceptable protocol version");
  } else if (reason == AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED) {
    Serial.println("Indentifier rejected");
  } else if (reason == AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE) {
    Serial.println("Server unavailable");
  } else if (reason == AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS) {
    Serial.println("Malformed credentials");
  } else if (reason == AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED) {
    Serial.println("Not authorized");
  } else {
    Serial.println("Unknown reason");
  }

  int8_t attemptToConnectCounter = 0;
  delay(3000);
  Serial.println("MQTT: Attemp to reconnect to the broker");
  while ( (connected() == false) && (attemptToConnectCounter < 100) ) {
    if (WiFi.isConnected()) {
      Serial.printf("MQTT: Attempt %d. Reconnecting to broker ... \n", attemptToConnectCounter);
      connect();
    } else {
      Serial.println("MQTT: WiFi is not connected. Wait.");
    }
    delay(3000);
    attemptToConnectCounter += 1;
  }
}

void DeviceMqttClient::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  Serial.println();
  Serial.println("MQTT: Message received.");
  Serial.printf("  topic: %s\n", topic);
  Serial.printf("  qos: %d\n", properties.qos);
  Serial.printf("  dup: %d\n", properties.dup);
  Serial.printf("  retain: %d\n", properties.retain);

  Serial.print("  payload: ");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(payload);
  json.printTo(Serial);
  Serial.println();

  Serial.print("MQTT: Parsing received JSON ... ");
  if (json.success()) {
    Serial.println("ok");
    if (json.containsKey("state")) {
      const char* state = json["state"];
      String stringState(state);
      m_deviceState->state = (stringState == "ON") ? 1 : 0;
        
      if (json.containsKey("brightness")) {
        m_deviceState->brightness = json["brightness"];
      }

      if (json.containsKey("color")) {
        m_deviceState->red = json["color"]["r"];
        m_deviceState->green = json["color"]["g"];
        m_deviceState->blue = json["color"]["b"];
      }

      if (json.containsKey("effect")) {
        const char* effect = json["effect"];
        m_deviceState->effect = String(effect);
      } else {
        m_deviceState->effect = "";
      }

      if (json.containsKey("transition")) {
        m_deviceState->transition = json["transition"];
      } else {
        m_deviceState->transition = 0;
      }

      /* Parameters of various effects */
      if (json.containsKey("effect_fire_cooling")) {
        m_deviceState->effect_fire_cooling = json["effect_fire_cooling"];
      }

      if (json.containsKey("effect_fire_sparking")) {
        m_deviceState->effect_fire_sparking = json["effect_fire_sparking"];
      }

      /* Update actual state of the device*/
      if (m_updateDeviceState) {
        m_updateDeviceState();
      }

      this->publishDeviceState();
    }
  } else {
    Serial.println("failed");
  }
}

void DeviceMqttClient::publishDeviceState() {
  const int BUFFER_SIZE = JSON_OBJECT_SIZE(20);
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  // Light state: state, color, brightness
  if (m_deviceState) {
    root["state"] = m_deviceState->state ? "ON" : "OFF";
    JsonObject& color = root.createNestedObject("color");
    color["r"] = m_deviceState->red;
    color["g"] = m_deviceState->green;
    color["b"] = m_deviceState->blue;
    root["brightness"] = m_deviceState->brightness;
  }

  if ( m_deviceState->effect != "" ) {
    root["effect"] = m_deviceState->effect;
  }/* else {
    root["effect"] = ""; //TODO: Is this really needed to send an empty string ?
  }*/

  if ( m_deviceState->transition != 0 ) {
    root["transition"] = m_deviceState->transition;
  }

  // Additional parameters: ip-address, mac-address, RSSI, uptime
  // IP-address
  char ip[16];
  memset(ip, 0, 18);
  sprintf(ip, "%s", WiFi.localIP().toString().c_str());
  root["ip"] = ip;

  // MAC address
  uint8_t macAddr[6];
  WiFi.macAddress(macAddr);
  char mac[18];
  memset(mac, 0, 18);
  sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
  root["mac"] = mac;

  // RSSI
  char rssi[8];
  sprintf(rssi, "%d", WiFi.RSSI());
  root["rssi"] = rssi;

  // Uptime
  root["uptime"] = uptime( millis() );

  // Firmware version
  root["version"] = FIRMWARE_VERSION;
  
  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  Serial.printf("\nMQTT: Publish state: %s\n", buffer);

  publish(MQTT_TOPIC_STATE, 0, true, buffer);
}
