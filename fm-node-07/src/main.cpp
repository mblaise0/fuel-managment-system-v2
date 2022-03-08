/*
*  335KVAGen01 Fuel-Management-Control
*/

#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_STATUS_REQUEST
#include <TaskScheduler.h>

#define GEN_01_335KVA_VALVE 36
#define GEN_01_335KVA_FLOWMETER 37

const char* ssid = "iomariTPLink"; // Wifi SSID
const char* password = "#InspireD#"; // Wifi Password

const char* mqtt_server = "192.168.2.44";
IPAddress local_IP(192, 168, 2, 66);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 2);
 
IPAddress subnet(255, 255, 252, 0);

WiFiClient espClient10;
PubSubClient client(espClient10);

const char* clientName = "FM-Node-07";

Scheduler userScheduler; 

long lastReconnectAttempt = 0;

#include "./dcl.h"
#include "./GeneratorFuelTankControl335Kva.h"

void callback(char* topic, byte* payload, unsigned int length) {
  String inValue;
  String topicStr = topic;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  char payloadString[length+1];
  memcpy(payloadString, payload, length);
  payloadString[length] = '\0';
  inValue = payloadString;
  Serial.println();  
  parseCommand(inValue, topicStr);
}


boolean reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientName)) {
      Serial.println("connected");
      client.publish("outTopic","FM-Node07 connected");
      client.subscribe("DTNK", 1);
      client.subscribe("ADTKR", 1);
      client.subscribe("DTKR", 1);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, 11883);
  client.setCallback(callback);

  pinMode(GEN_01_335KVA_VALVE, OUTPUT);
  pinMode(GEN_01_335KVA_FLOWMETER, INPUT);

  userScheduler.addTask(Replenish335KvaGen01);
  userScheduler.addTask(StartProcessToReplenish335KvaGen01);
  userScheduler.addTask(Stop335kvaGen01ReplenishmentProcess);

  generator01335kva.currentMillis = 0;
  generator01335kva.previousMillis = 0;
  generator01335kva.flowInterval = 1000;
  generator01335kva.calibrationFactor = 4.5;
  generator01335kva.pulse1Sec = 0;
  generator01335kva.pulseCount = 0;
  generator01335kva.flowRate = 0.0;
  generator01335kva.flowMilliLitres = 0;
  generator01335kva.totalMilliLitres = 0;
  generator01335kva.previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(GEN_01_335KVA_FLOWMETER), gen01335pulseCounter, FALLING);

  generator01335kva.flowSwitchState = 0;
  generator01335kva.previousFlowSwitchState = 0;

  generator01335kva.aSignalCount = 0;
  generator01335kva.dSignalCount = 0;

  lastReconnectAttempt = 0;
}

void loop() {
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
    reconnect();
  } else {
      client.loop();
      userScheduler.execute();
  }
}