#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-ubuntu-16-04
// Sketch mainly copied out of "Das ESP8266 Praxisbuch - Hack 19", Erik Bartmann.
// https://techtutorialsx.com/2017/04/24/esp32-subscribing-to-mqtt-topic/
// https://www.arduino.cc/reference/en/language/functions/communication/serial/write/

const char* ssid = "FRITZ!Box nnnn";
const char* stringNumber = "xxx";
const char* mqtt_server = "xxx.xxx.xxx.xxx";
const int mqtt_port = 1883;

const char* pubTopicLog = "/log";

const char* pubTopicToPC = "/arbeitszimmer/temperatur";
const int maxExpecArdInLen = 50;
const char* subTopicFromPC = "/arbeitszimmer/temperatur/ergebnis";
const int expectedMqttInput = 400;

const char* subTopicSteerArduino = "/steerArduino";
const int expectedSteerArduinoInput = 7;
const char* subTopicWaterPlant = "/waterPlant";
const int expectedWaterPlantInput = 3;

const int keyWordLength = 3;
const char* pubPlant1 = "/plant/1";
char keyPlant1[keyWordLength] = "P1";
const char* pubPlant2 = "/plant/2";
char keyPlant2[keyWordLength] = "P2";
const char* pubPlant3 = "/plant/3";
char keyPlant3[keyWordLength] = "P3";
const char* pubPlant4 = "/plant/4";
char keyPlant4[keyWordLength] = "P4";
const char* pubTemperature = "/temperature";
char keyTemperature[keyWordLength] = "TP";
const char* pubDateTime = "/dateTime";
char keyDateTime[keyWordLength] = "DT";



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

  // Output from Spark Streaming
  if(strcmp(topic, subTopicFromPC) == 0) {
    char txData[expectedMqttInput] = "";
    for(int i = 0; i < (int) length; i++) {
      txData[i] = (char) payload[i];
    }
    Serial.write(txData); Serial.println();
  }

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

  // Command to water plants
  if(strcmp(topic, subTopicWaterPlant) == 0) {

    // string is transported to Arduino with no modifications
    // (just adding a carriage return and line feed at the end)
    char txData[expectedWaterPlantInput] = "";
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
      client.subscribe(subTopicFromPC);
      client.subscribe(subTopicSteerArduino);
      client.subscribe(subTopicWaterPlant);
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

    // Plant 1
    if(strcmp(currentKeyWord, keyPlant1) == 0) {
      char currentValue[4] = "";
      substring(usartStr, currentValue, 3, 3);
      client.publish(pubPlant1, currentValue);
    } 

    // Plant 2
    else if(strcmp(currentKeyWord, keyPlant2) == 0) {
      char currentValue[4] = "";
      substring(usartStr, currentValue, 3, 3);
      client.publish(pubPlant2, currentValue);
    } 

    // Plant 3
    else if(strcmp(currentKeyWord, keyPlant3) == 0) {
      char currentValue[4] = "";
      substring(usartStr, currentValue, 3, 3);
      client.publish(pubPlant3, currentValue);
    } 

    // Plant 4
    else if(strcmp(currentKeyWord, keyPlant4) == 0) {
      char currentValue[4] = "";
      substring(usartStr, currentValue, 3, 3);
      client.publish(pubPlant4, currentValue);
    }

    // Temperature
    else if(strcmp(currentKeyWord, keyTemperature) == 0) {
      char currentValue[6] = "";
      substring(usartStr, currentValue, 3, 5);
      client.publish(pubTemperature, currentValue);
    }

    // Date & Time
    else if(strcmp(currentKeyWord, keyDateTime) == 0) {
      char currentValue[20] = "";
      substring(usartStr, currentValue, 3, 19);
      client.publish(pubDateTime, currentValue);
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
