
#include <SimpleTimer.h>          // https://github.com/schinken/SimpleTimer

#include "Config.h"
#include "UiManager.h"
#include "MqttClient.h"
#include "DeviceControl.h"
#include "LightControlButton.h"

/* Create a UI manager */
UiManager uiManager;

/* MQTT Client instance */
DeviceMqttClient mqttClient;

/* The state of this device */
DeviceState deviceState;

/* Device control object */
DeviceControl device;

/* Timer to publish the current state */
SimpleTimer timer;

#ifdef LAMP_CONTROL_BUTTON_ENABLED
/* Light control button */
LightControlButton button;
#endif

void publishDeviceStateTimer() {
  mqttClient.publishDeviceState();
}

void setup() {

  /* Init serial port */
  Serial.begin(115200);
  Serial.println();

  //clean FS, for testing
  //SPIFFS.format();

  /* Initialize the device */
  device.setDeviceStateReference( &deviceState );
  device.initDevice();

  /* Create UI and connect to WiFi */
  uiManager.initUIManager(false);

  /* Configure MQTT */
  mqttClient.setDeviceStateReference( &deviceState );
  int p = atoi( uiManager.mqttPort() );
  mqttClient.setServer( uiManager.mqttServer(), p );
  mqttClient.setCredentials( uiManager.mqttLogin(), uiManager.mqttPassword() );
  mqttClient.setKeepAlive( MQTT_KEEP_ALIVE_SECONDS );
  mqttClient.setWill( MQTT_TOPIC_STATUS, 1, true, MQTT_STATUS_PAYLOAD_OFF ); //topic, QoS, retain, payload
  String string_client_id( uiManager.mqttClientId() );
  string_client_id.trim();
  if (string_client_id != String("")) {
    mqttClient.setClientId( uiManager.mqttClientId() );
  }

  /* Set a callback to update the actual state of the device when an mqtt command is received */
  mqttClient.onMessageReveived( std::bind(&DeviceControl::updateDeviceState, &device) );

  /* Connect the MQTT client to the broker */
  int8_t attemptToConnectToMQTT = 0;
  Serial.println("MQTT: Connect to the broker");
  while ( (mqttClient.connected() == false) && (attemptToConnectToMQTT < 5) ) {
    if (WiFi.isConnected()) {
      Serial.printf("MQTT: Attempt %d. Connecting to broker [%s:%d]: login: [%s] password: [%s] ... \n", attemptToConnectToMQTT, uiManager.mqttServer(), p, uiManager.mqttLogin(), uiManager.mqttPassword() );
      mqttClient.connect();
    } else {
      //attemptToConnectToMQTT = 0;
      Serial.println("MQTT: WiFi is not connected. Try to reconnect WiFi.");
      WiFi.reconnect();
    }
    delay(3000);
    attemptToConnectToMQTT += 1;
  }

  /* If there is still no connection here, restart the device */
  if (!WiFi.isConnected()) {
    Serial.println("setup(): WiFi is not connected. Reset the device to initiate connection again.");
    ESP.restart();
  }

  if (!mqttClient.connected()) {
    Serial.println("setup(): The device is not connected to MQTT. Reset the device to initiate connection again.");
    ESP.restart();
  }

#ifdef LAMP_CONTROL_BUTTON_ENABLED
  /* Initialize the button */
  button.init(BUTTON_PIN);
  button.onClicked( std::bind(&DeviceMqttClient::sendSwitchStateCommand, &mqttClient) );
  button.onDoubleClicked( std::bind(&DeviceMqttClient::sendSwitchEffectCommand, &mqttClient) );
  button.onPressAndHold( std::bind(&DeviceMqttClient::sendChangeBrightnessCommand, &mqttClient) );
#endif

  /* Publish device state periodicly */
  timer.setInterval(INTERVAL_PUBLISH_STATE, publishDeviceStateTimer);
}


void loop() {
  timer.run();
  device.run();

#ifdef LAMP_CONTROL_BUTTON_ENABLED
  button.run();
#endif

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("loop(): WiFi is not connected. Reset the device to initiate connection again.");
    ESP.restart();
  }
}
