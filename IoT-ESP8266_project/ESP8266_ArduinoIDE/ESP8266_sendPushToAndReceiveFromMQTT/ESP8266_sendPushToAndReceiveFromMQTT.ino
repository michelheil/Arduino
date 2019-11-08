#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-ubuntu-16-04
// Sketch mainly copied out of "Das ESP8266 Praxisbuch - Hack 19", Erik Bartmann.
// https://techtutorialsx.com/2017/04/24/esp32-subscribing-to-mqtt-topic/
// https://www.arduino.cc/reference/en/language/functions/communication/serial/write/

const char* ssid = "FRITZ!Box";
const char* stringNumber = "xxx";
const char* mqtt_server = "192.168.178.49";
const int mqtt_port = 1883;
const char* topicToPC = "/arbeitszimmer/temperatur";
const char* topicFromPC = "/arbeitszimmer/temperatur/ergebnis";
const int RATE = 5000; // Sendefrequenz alle 5000 Milli-Sekunden

WiFiClient espClient;
PubSubClient client(espClient);

// Initialize WiFi connection
void WiFiInit() {
  delay(10);
  //DEBUG Serial.print("\n\nVerbindungsaufnahme zu: ");
  //DEBUG Serial.print(ssid);
  WiFi.begin(ssid, stringNumber);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    //DEBUG Serial.print(".");
  }
  //DEBUG Serial.println("\nMitWiFi verbunden");
  //DEBUG Serial.print("IP Adresse: ");
  //DEBUG Serial.println(WiFi.localIP());
}

// callback function wenn eine Nachricht in einem Topic landet
void callback(char* topicFromPC, byte* payload, unsigned int length){
  char txData[20] = "";
  //DEBUG Serial.print("Nachricht eingegangen [");
  //DEBUG Serial.print(topicFromPC);
  //DEBUG Serial.print("] ");
  for(int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    txData[i] = (char) payload[i];
  }
  Serial.write(txData); Serial.println();
  //DEBUG Serial.println();
}

// reconnect to MQTT-Broker
void reconnect() {
  while (!client.connected()) {
    //DEBUG Serial.print("Starte MQTT-Verbindung...");
    if (client.connect("ESP8266Client")) {
      //DEBUG Serial.println("verbunden");
      client.publish(topicToPC, "Ich lebe!");
      client.subscribe(topicFromPC);
    } else {
      //DEBUG Serial.print("Fehler, rc=");
      //DEBUG Serial.print(client.state());
      //DEBUG Serial.println("versuche es in 5 Sekunden erneut");
      delay(5000);
    }
  }
}

// globale Variablen verwendet fuer den Timer, um alle 5 Sekunden eine Hello World Nachricht zu versenden
long lastMsg = 0;
int counter = 0;


void setup() {
  Serial.begin(9600);
  WiFiInit();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()) {reconnect();}               // Reconnect falls Verbindung abgebrochen
  client.loop();
  
// Begin //////////////////////////////////////////
  // Heartbeat (ESP to MQTT)
//  long now = millis();                                 // Referenz des Timers
//  char msg[100];                                       // wird fuer Hello World Nachricht verwendet
//
//  if(now - lastMsg > RATE) {                           // Nachricht alle 5 Sekunden uebertragen
//    lastMsg = now;
//    ++counter;
//    snprintf(msg, 80, "Hello world #%ld", counter);
//    //DEBUG Serial.print("Publish message: ");
//    //DEBUG Serial.println(msg);
//    client.publish(topicToPC, msg);
//  }
// End ////////////////////////////////////////////  

// Begin //////////////////////////////////////////
// Arduino to ESP via serial RX
  const int expecArdInLen = 64;                                       // expectedArduinoInputLength (maximum length is 64 bytes)
  char rxData[expecArdInLen] = "";                                    // buffer for RX data                             
  //String variable = "";
  int indexArduinoInput = 0;
 
  if (Serial.available() > 0 ) {                                        // Read from Rx from atmega328p
    while (Serial.available() > 0 && indexArduinoInput < expecArdInLen) // read till 6th character
    {
      delay(15);
      rxData[indexArduinoInput] = Serial.read();                // start reading serially and save to variable
      indexArduinoInput++;
      rxData[indexArduinoInput] = '\0';
    }
   // variable.toUpperCase();                                       // convert to uppercase
    //for (byte  i = 0 ; i < expecArdInLen ; i++) {
    //  variable.concat(String(rxData[i]));                       // concat strings of RX string
   // }
   // Serial.print("Variable = "); Serial.println(variable);        // debug and print incoming data
   // delay(20);


    //String  string = String(variable);                            // string used to compare
// End ////////////////////////////////////////////

// Begin //////////////////////////////////////////
// ESP to MQTT/PC via WiFi  
    client.publish(topicToPC, rxData);
// End ////////////////////////////////////////////

    
    //DEBUG Serial.println("");
  }
  
  


}
