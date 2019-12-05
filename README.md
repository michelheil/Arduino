# Content
This repository contains the source code of all my Arduino projects. The Arduino has a ATmega328P micro controller.


New Structure:
util
sensor
projects
Tipps&Tricks


util
- myGlobalArduinoDefines (globalDefines, myHelperFunctions, myVariableTypes)
- myADC
- myLOG
- myTWI
- myUSART

README
- motivation
- dependency/interaction with other utils
- usage

sensor
- myAMG8833
- myBME680
- myDS3231
- myLCD1602 (myLCD)
- myILI9341 (TFTLCD)

README
- picture of sensor
- link to sensor data sheet (upload to GitHub!)
- picture of wiring
- required Util (there should be no dependency to another sensor)
- Init function
- remaining functions (API (no internal/helper))


projects
- Arduino StarterKit
- LoopingLouie
- First IoT-ESP8266
- ESP8266 (connectWiFi, scanWiFi, sendToMqtt)

README
- overview & motivation
- picture wiring
- dependencies to utils and sensors (including links to up-to-date libraries)
- ToDos & next steps
