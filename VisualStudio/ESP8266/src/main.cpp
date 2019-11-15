#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-ubuntu-16-04
// Sketch mainly copied out of "Das ESP8266 Praxisbuch - Hack 19", Erik Bartmann.
// https://techtutorialsx.com/2017/04/24/esp32-subscribing-to-mqtt-topic/
// https://www.arduino.cc/reference/en/language/functions/communication/serial/write/

const char* ssid = "FRITZ!Box 7412";
const char* stringNumber = "31469059394759135757";
const char* mqtt_server = "192.168.178.49";
const int mqtt_port = 1883;
const char* topicToPC = "/arbeitszimmer/temperatur";
const char* topicFromPC = "/arbeitszimmer/temperatur/ergebnis";
const int expectedMqttInput = 20;
const int RATE = 5000; // Sendefrequenz alle 5000 Milli-Sekunden (fuer Debugging)

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



// callback function wenn eine Nachricht in einem Topic landet
void callback(char* topicFromPC, byte* payload, unsigned int length){
  char txData[expectedMqttInput] = "";
  for(int i = 0; i < (int) length; i++) {
    txData[i] = (char) payload[i];
  }
  Serial.write(txData); Serial.println();
}


// reconnect to MQTT-Broker
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      client.publish(topicToPC, "Ich lebe!");
      client.subscribe(topicFromPC);
    } else {
      delay(5000);
    }
  }
}



// globale Variablen verwendet fuer den Timer, um alle 5 Sekunden eine Hello World Nachricht zu versenden
long lastMsg = 0;
int counter = 0;
const int maxExpecArdInLen = 50;                         // expectedArduinoInputLength (maximum length is (64 bytes * (5 + 1)) + 1) = 

void setup() {
  Serial.begin(250000);
  WiFiInit();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()) {reconnect();}               // Reconnect falls Verbindung abgebrochen
  client.loop();

  // Arduino to ESP via serial RX

  if (Serial.available() > 0) {                                        // read from Rx from atmega328p

    uint8_t usartStrCompleteFlag = 0;
    char    usartStr[maxExpecArdInLen] = "";                       // buffer for RX data 
    uint8_t usartStrCount = 0;

    while (usartStrCompleteFlag == 0)
    {
      delay(15);
      byte nextByte = Serial.read();
      
      if ( (nextByte >= 0x20) && (usartStrCount < maxExpecArdInLen) )
      {
        usartStr[usartStrCount] = nextByte;                // start reading serially and save to variable
        usartStrCount++;
        
      } else {
        usartStr[usartStrCount] = '\0';
        usartStrCompleteFlag = 1;
      }

      if (Serial.peek() == -1)
      {
        usartStr[usartStrCount] = '\0';
        usartStrCompleteFlag = 1;
      }
    }

    // ESP to MQTT/PC via WiFi  
    client.publish(topicToPC, usartStr);
  }
}