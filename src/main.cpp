#include <Arduino.h>
#include <PubSubClient.h>

#include "ESPBASE.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);
const byte mqttDebug = 1;
//const int ESP_BUILTIN_LED = 1;
#define RELAY1_PIN 12
#define RELAY2_PIN 13
#define STATUS_LED 16
byte relay1state = 0;
byte relay2state = 0;
String sChipID;
long lastReconnectAttempt = 0;
int rbver = 0;
String RelayTopic;
String StatusTopic;

ESPBASE Esp;

void setup() {
  Serial.begin(115200);
  char cChipID[10];
  sprintf(cChipID,"%08X",ESP.getChipId());
  sChipID = String(cChipID);

  Esp.initialize();
  RelayTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/command";
  StatusTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/status";
  Esp.setupMQTTClient();
  customWatchdog = millis();

  pinMode(RELAY1_PIN,OUTPUT);
  pinMode(RELAY2_PIN,OUTPUT);
 
  Serial.println("Done with setup");
  Serial.println(config.ssid);
}

void loop() {
  Esp.loop();
}

String getSignalString()
{
  String signalstring = "";
  byte available_networks = WiFi.scanNetworks();
  signalstring = signalstring + sChipID + ":";
 
  for (int network = 0; network < available_networks; network++) {
    String sSSID = WiFi.SSID(network);
    if(network > 0)
      signalstring = signalstring + ",";
    signalstring = signalstring + WiFi.SSID(network) + "=" + String(WiFi.RSSI(network));
  }
  return signalstring;    
}

void sendStatus()
{
  String message = config.Relay1Name + ",1,";
  if(relay1state == 1)
    message = message + "on";
  else
    message = message + "off";
  message = message + ":" + config.Relay2Name + ",2,";
  if(relay2state == 1)
    message = message + "on";
  else
    message = message + "off";
  Esp.mqttSend(StatusTopic,"",message);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char c_payload[length];
  memcpy(c_payload, payload, length);
  c_payload[length] = '\0';
  
  String s_topic = String(topic);
  String s_payload = String(c_payload);

  if(s_topic == "SendStat")
  {
    sendStatus();
  }
  if (s_topic == RelayTopic || s_topic == "AllLights") {
    if(s_payload == "signal")
    {
      Esp.mqttSend(StatusTopic,sChipID," WiFi: " + getSignalString());
    }
    if(s_payload == "TOGGLE")
    {
      if(relay1state == 1)
      {
        s_payload = "OFF";
      }
      else
      {
        s_payload = "ON";
      }
    }
    if(s_payload == "TOGGLE_1")
    {
      if(relay1state == 1)
      {
        s_payload = "OFF_1";
      }
      else
      {
        s_payload = "ON_1";
      }
    }
    if(s_payload == "TOGGLE_2")
    {
      if(relay2state == 1)
      {
        s_payload = "OFF_2";
      }
      else
      {
        s_payload = "ON_2";
      }
    }
    if(s_payload == "ON_1" || s_payload == "ON")
    {
      relay1state = 1;
      digitalWrite(RELAY1_PIN,HIGH);
    }
    if(s_payload == "OFF_1" || s_payload == "OFF")
    {
      relay1state = 0;
      digitalWrite(RELAY1_PIN,LOW);
    }
    if(s_payload == "ON_2" || s_payload == "ON")
    {
      relay2state = 1;
      digitalWrite(RELAY2_PIN,HIGH);
    }
    if(s_payload == "OFF_2" || s_payload == "OFF")
    {
      relay2state = 0;
      digitalWrite(RELAY2_PIN,LOW);
    }
    sendStatus();
  }
}

void mqttSubscribe()
{
    if (Esp.mqttClient->connected()) {
      if (Esp.mqttClient->subscribe(RelayTopic.c_str())) {
        Serial.println("Subscribed to " + RelayTopic);
        Esp.mqttSend(StatusTopic,"","Subscribed to " + RelayTopic);
        Esp.mqttSend(StatusTopic,verstr,","+Esp.MyIP()+" start");
      }
      if (Esp.mqttClient->subscribe("SendStat"))
      {
        Serial.println("Subscribed to SendStat");
      }
      if (Esp.mqttClient->subscribe("AllLights"))
      {
        Serial.println("Subscribed to AllLights");
      }
    }
}


