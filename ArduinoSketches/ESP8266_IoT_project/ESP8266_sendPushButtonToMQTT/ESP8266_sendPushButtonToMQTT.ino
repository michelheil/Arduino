#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-ubuntu-16-04
// Sketch mainly copied out of "Das ESP8266 Praxisbuch - Hack 19", Erik Bartmann.

const char* ssid = "FRITZ!Box";
const char* stringNumber = "zzz";
const char* mqtt_server = "192.168.178.49";
const int mqtt_port = 1883;
const char* topic = "/arbeitszimmer/temperatur";
const int RATE = 5000; // Sendefrequenz alle 5 Sekunden

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0; // wird fuer Timer verwendet
int counter = 0;


void WiFiInit() {
  delay(10);
  Serial.print("\n\nVerbindungsaufnahme zu: ");
  Serial.print(ssid);
  WiFi.begin(ssid, stringNumber);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nMitWiFi verbunden");
  Serial.print("IP Adresse: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Nachricht eingegangen [");
  Serial.print(topic);
  Serial.print("] ");
  for(int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Starte MQTT-Verbindung...");
    if (client.connect("ESP8266Client")) {
      Serial.println("verbunden");
      client.publish(topic, "Ich lebe!");
      client.subscribe(topic);
    } else {
      Serial.print("Fehler, rc=");
      Serial.print(client.state());
      Serial.println("versuche es in 5 Sekunden erneut");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  WiFiInit();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()) { reconnect(); }
  client.loop();
  long now = millis();
  char msg[100]; // wird fuer Nachricht verwendet
  // Nachricht alle 5 Sekunden uebertragen
  if(now - lastMsg > RATE) {
    lastMsg = now;
    ++counter;
    snprintf(msg, 80, "Hello world #%ld", counter);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(topic, msg);
  }


  char inChar = 0;          
  char inData[6] = "";        // data length of 6 characters
  String variable = "";
  String variable1 = "";
  int index1 = 0;
 
  if (Serial.available() > 0 ) {                      // Read from Rx from atmega16
    while (Serial.available() > 0 && index1 < 6)     // read till 6th character
    {
      delay(100);
      inChar = Serial.read();      // start reading serilly and save to variable
      inData[index1] = inChar;
      index1++;
      inData[index1] = '\0';         // Add a null at the end // Hier nicht ganz richtig!! die \0 wird nur ganz am Ende benoetigt
    }
    variable.toUpperCase();       // convert to uppercase
    for (byte  i = 0 ; i < 6 ; i++) {
      variable.concat(String(inData[i]));    // concat strings
    }
    Serial.print("Variable = "); Serial.println(variable);  // debug and print incoming data
    delay(20);


    String  string = String(variable);  // string used to compare
  
    client.publish(topic, inData);
    Serial.println("");
  }
  
  


}
