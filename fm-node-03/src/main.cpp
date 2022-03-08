/*
*  Day-tank Fuel-Management-Control 
*  Framework: ESP32
*/

#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_STATUS_REQUEST
#include <TaskScheduler.h>

#define FLOWMETER_DISCHARGE_PUMP01 33
#define FLOWMETER_DISCHARGE_PUMP02 34
#define TEMPERATURE_SENSOR 35
#define INPUT_TO_DAY_TANKFLOW_METER 32
#define OUTPUT_FROM_DAY_TANK_FLOWMETER 27 
#define OUTPUT_FROM_DAY_TANK_FLOW_SWITCH 26
#define ELECTRIC_PUMP 25

OneWire oneWire(TEMPERATURE_SENSOR);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature flowTemperature(&oneWire);

const char* ssid = "iomariTPLink"; // Wifi SSID
const char* password = "#InspireD#"; // Wifi Password

const char* mqtt_server = "192.168.2.44";
IPAddress local_IP(192, 168, 2, 61);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 2);
 
IPAddress subnet(255, 255, 252, 0);

WiFiClient espClient10;
PubSubClient client(espClient10);

const char* clientName = "FM-Node-03";

Scheduler userScheduler; 

long lastReconnectAttempt = 0;

#include "./dcl.h"
#include "./DayTankFuelAnalysis.h"

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
      client.publish("outTopic","FM-Node03 connected");
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

  int actuators[] = {FLOWMETER_DISCHARGE_PUMP01, FLOWMETER_DISCHARGE_PUMP02, ELECTRIC_PUMP};
  for (int i = 0; i < 3; i++){
    pinMode(actuators[i], OUTPUT);
  }
    
  int interrupts[] = {TEMPERATURE_SENSOR, INPUT_TO_DAY_TANKFLOW_METER, OUTPUT_FROM_DAY_TANK_FLOWMETER, OUTPUT_FROM_DAY_TANK_FLOW_SWITCH};
  for (int i = 0; i < 4; i++){
    pinMode(interrupts[i], INPUT);
  } 
  
  userScheduler.addTask(DetectFuelFlowFromDayTank);
  DetectFuelFlowFromDayTank.enable();

  userScheduler.addTask(StopInputFlowComputationToDayTank);
  userScheduler.addTask(StartInputFlowComputationToDayTank);
  userScheduler.addTask(ProcessInputToDayTank);
  userScheduler.addTask(ProcessOutputFromDayTank);
  userScheduler.addTask(ProcessPump01FlowRate);
  userScheduler.addTask(ProcessPump02FlowRate);

  inputFlowMeterControl.currentMillis = 0;
  inputFlowMeterControl.previousMillis = 0;
  inputFlowMeterControl.flowInterval = 1000;
  inputFlowMeterControl.calibrationFactor = 4.5;
  inputFlowMeterControl.pulse1Sec = 0;
  inputFlowMeterControl.pulseCount = 0;
  inputFlowMeterControl.flowRate = 0.0;
  inputFlowMeterControl.flowMilliLitres = 0;
  inputFlowMeterControl.totalMilliLitres = 0;
  inputFlowMeterControl.previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(INPUT_TO_DAY_TANKFLOW_METER), inputFlowMeterPulseCounter, FALLING);

  outputFlowMeterControl.currentMillis = 0;
  outputFlowMeterControl.previousMillis = 0;
  outputFlowMeterControl.flowInterval = 1000;
  outputFlowMeterControl.calibrationFactor = 4.5;
  outputFlowMeterControl.pulse1Sec = 0;
  outputFlowMeterControl.pulseCount = 0;
  outputFlowMeterControl.flowRate = 0.0;
  outputFlowMeterControl.flowMilliLitres = 0;
  outputFlowMeterControl.totalMilliLitres = 0;
  outputFlowMeterControl.previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(OUTPUT_FROM_DAY_TANK_FLOWMETER), outputFlowMeterPulseCounter, FALLING);


  pump01FlowMeterControl.currentMillis = 0;
  pump01FlowMeterControl.previousMillis = 0;
  pump01FlowMeterControl.flowInterval = 1000;
  pump01FlowMeterControl.calibrationFactor = 4.5;
  pump01FlowMeterControl.pulse1Sec = 0;
  pump01FlowMeterControl.pulseCount = 0;
  pump01FlowMeterControl.flowRate = 0.0;
  pump01FlowMeterControl.flowMilliLitres = 0;
  pump01FlowMeterControl.totalMilliLitres = 0;
  pump01FlowMeterControl.previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(FLOWMETER_DISCHARGE_PUMP01), pump01FlowMeterPulseCounter, FALLING);

  pump02FlowMeterControl.currentMillis = 0;
  pump02FlowMeterControl.previousMillis = 0;
  pump02FlowMeterControl.flowInterval = 1000;
  pump02FlowMeterControl.calibrationFactor = 4.5;
  pump02FlowMeterControl.pulse1Sec = 0;
  pump02FlowMeterControl.pulseCount = 0;
  pump02FlowMeterControl.flowRate = 0.0;
  pump02FlowMeterControl.flowMilliLitres = 0;
  pump02FlowMeterControl.totalMilliLitres = 0;
  pump02FlowMeterControl.previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(FLOWMETER_DISCHARGE_PUMP02), pump02FlowMeterPulseCounter, FALLING);

  dayTankFuelTank.flowSwitchState = 0;
  dayTankFuelTank.previousFlowSwitchState = 0;

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