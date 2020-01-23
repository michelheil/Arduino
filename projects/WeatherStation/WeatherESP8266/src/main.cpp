/**
 * @file main.cpp
 * @author Michael Heil
 * @brief Weather Station
 * @version 0.2 - ESP8266
 * @date 2020-01-05
 * 
 * @copyright Copyright (c) 2020
 * 
 * @section Features
 * Sending information from BME680 to MQTT.
 * 
 * @section Open Points
 * @li Power saving modes of ESP8266
 * @li Battery power supply for ESP8266
 * @li Verwende "Verteilerbox" und nicht Eier-Pappe
 * 
 * @section Obeservations
 * @li .
 * @li .
 * @li .
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const char* ssid = "FRITZ!Box";
const char* stringNumber = "xxx";
const char* mqtt_server = "192.168.178.57";
const int mqtt_port = 1883;
const char* pubTopicLog = "/log";

// input from MQTT
const char* subTopicSteerArduino = "/steerArduino";
const int expectedSteerArduinoInput = 7;

// input from Arduino
const int maxExpecArdInLen = 10;
const int keyWordLength = 3;

// output to MQTT
const char* pubTemperature = "/temperature";
char keyTemperature[keyWordLength] = "TP";
const char* pubHumidity = "/humidity";
char keyHumidity[keyWordLength] = "HU";
const char* pubPressure = "/pressure";
char keyPressure[keyWordLength] = "PR";
const char* pubAltitude = "/altitude";
char keyAltitude[keyWordLength] = "AL";


WiFiClient espClient;
PubSubClient client(espClient);


// Initialize WiFi connection
void WiFiInit() {
  delay(10);
  WiFi.begin(ssid, stringNumber);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}


// Callback functions handles incoming data from subscribed mqtt topics
// ESP to Arduino via UART
ICACHE_RAM_ATTR void callback(char* topic, byte* payload, unsigned int length){

  // Command to steer Arduino
  if(strcmp(topic, subTopicSteerArduino) == 0) {

    // string is transported to Arduino with no modifications
    // (just adding a carriage return and line feed at the end)
    char txData[expectedSteerArduinoInput] = "";
    for(int i = 0; i < (int) length; i++) {
      txData[i] = (char) payload[i];
    }
    Serial.write(txData); Serial.println();
  }
}


// connect to MQTT-Broker and subscribe to relevant topics
void mqttConnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      client.publish(pubTopicLog, "Ich lebe!");
      client.subscribe(subTopicSteerArduino);
    } else {
      delay(5000);
    }
  }
}


// C substring function definition
// to reference to first position of str use 1 not 0
void substring(char str[], char subStr[], int position, int length) {
   int currentIndex = 0;
   
   while (currentIndex < length) {
      subStr[currentIndex] = str[position-1+currentIndex];
      currentIndex++;
   }
   subStr[currentIndex] = '\0';
}

void setup() 
{
  Serial.begin(1000000); // Initialize UART given the baud rate
  WiFiInit(); // establish WiFi connection
  client.setServer(mqtt_server, mqtt_port); // set MQTT broker address and port
  client.setCallback(callback); // provide callback function that is executed when subscribung to a topic
  mqttConnect(); // connect to MQTT broker
}

void loop()
{
  // Arduino to ESP via UART
  if (Serial.available() > 0) // read from Rx from atmega328p
  {
    uint8_t usartStrCompleteFlag = 0;
    char    usartStr[maxExpecArdInLen] = ""; // buffer for RX data 
    uint8_t usartStrCount = 0;

    delayMicroseconds(400); // required to wait until all data (8 temperature values) have been transferred

    while (usartStrCompleteFlag == 0)
    {
      byte nextByte = Serial.read();
      
      if ( (nextByte >= 0x20) && (usartStrCount < maxExpecArdInLen) )
      {
        usartStr[usartStrCount] = nextByte;     // start reading serially and save to buffer
        usartStrCount++;
        
      } else {
        usartStr[usartStrCount] = '\0';
        usartStrCompleteFlag = 1;
      }
    }

    // handle key words
    // extract key word
    char currentKeyWord[keyWordLength] = "";
    substring(usartStr, currentKeyWord, 1, 2);
    
    // Temperature
    if(strcmp(currentKeyWord, keyTemperature) == 0) {
      char currentValue[6] = "";
      substring(usartStr, currentValue, 3, 5);
      client.publish(pubTemperature, currentValue);
    } 
    
    // Humidity
    else if(strcmp(currentKeyWord, keyHumidity) == 0) {
      char currentValue[6] = "";
      substring(usartStr, currentValue, 3, 5);
      client.publish(pubHumidity, currentValue);
    }

    // Pressure
    else if(strcmp(currentKeyWord, keyPressure) == 0) {
      char currentValue[8] = "";
      substring(usartStr, currentValue, 3, 7);
      client.publish(pubPressure, currentValue);
    }

    // Altidude
    else if(strcmp(currentKeyWord, keyAltitude) == 0) {
      char currentValue[8] = "";
      substring(usartStr, currentValue, 3, 7);
      client.publish(pubAltitude, currentValue);
    }

    // everything else
    else {
      client.publish(pubTopicLog, usartStr);
    }
  }

  // This should be called regularly to allow the client to process
  // incoming messages and maintain its connection to the server.
  if(!client.loop()) {mqttConnect();}
}