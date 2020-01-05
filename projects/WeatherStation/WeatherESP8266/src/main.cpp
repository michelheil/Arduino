#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-ubuntu-16-04
// https://techtutorialsx.com/2017/04/24/esp32-subscribing-to-mqtt-topic/
// https://www.arduino.cc/reference/en/language/functions/communication/serial/write/

const char* ssid = "FRITZ!Box 7412";
const char* stringNumber = "31469059394759135757";
const char* mqtt_server = "192.168.178.55";
const int mqtt_port = 1883;

const char* pubTopicLog = "/log";

const char* pubTopicToPC = "/arbeitszimmer/temperatur";
const int maxExpecArdInLen = 50;
const char* subTopicFromPC = "/arbeitszimmer/temperatur/ergebnis";
const int expectedMqttInput = 400;

const char* subTopicSteerArduino = "/steerArduino";
const int expectedSteerArduinoInput = 7;

const int keyWordLength = 3;
const char* pubTemperature = "/temperature";
char keyTemperature[keyWordLength] = "TP";



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





// Setup
void setup() {
  Serial.begin(1000000); // Initialize UART given the baud rate
  WiFiInit(); // establish WiFi connection
  client.setServer(mqtt_server, mqtt_port); // set MQTT broker address and port
  client.setCallback(callback); // provide callback function that is executed when subscribung to a topic
  mqttConnect(); // connect to MQTT broker
}

void loop() {
  
  // Arduino to ESP via UART
  if (Serial.available() > 0) {                 // read from Rx from atmega328p
    uint8_t usartStrCompleteFlag = 0;
    char    usartStr[maxExpecArdInLen] = "";      // buffer for RX data 
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
    // grid Values (and everything else)
    else {
      client.publish(pubTopicToPC, usartStr);
    }
  }

  // This should be called regularly to allow the client to process
  // incoming messages and maintain its connection to the server.
  if(!client.loop()) {mqttConnect();}
}