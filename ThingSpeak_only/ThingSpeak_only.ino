// ThingSpeak with multiple channels

/*
  WriteVoltage
  
  Reads an analog voltage from pin 0, and writes it to a channel on ThingSpeak every 20 seconds.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize and 
  analyze live data streams in the cloud.
  
  Copyright 2017, The MathWorks, Inc.
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the extras/documentation folder where the library was installed.
  See the accompaning licence file for licensing information.
*/

#include "ThingSpeak.h"

// ***********************************************************************************************************
// This example selects the correct library to use based on the board selected under the Tools menu in the IDE.
// Yun, Ethernet shield, WiFi101 shield, esp8266, and MXR1000 are all supported.
// With Yun, the default is that you're using the Ethernet connection.
// If you're using a wi-fi 101 or ethernet shield (http://www.arduino.cc/en/Main/ArduinoWiFiShield), uncomment the corresponding line below
// ***********************************************************************************************************

// Use wired ethernet shield
#include <SPI.h>
#include <Ethernet.h>

// Local Network Settings
byte mac[] = { 0xD4, 0x28, 0xB2, 0xFF, 0xA0, 0xA1 }; // Must be unique on local network
EthernetClient client;

// On Arduino:  0 - 1023 maps to 0 - 5 volts
#define VOLTAGE_MAX 5.0
#define VOLTAGE_MAXCOUNTS 1023.0

/*
  *****************************************************************************************
  **** Visit https://www.thingspeak.com to sign up for a free account and create
  **** a channel.  The video tutorial http://community.thingspeak.com/tutorials/thingspeak-channels/ 
  **** has more information. You need to change this to your channel, and your write API key
  **** IF YOU SHARE YOUR CODE WITH OTHERS, MAKE SURE YOU REMOVE YOUR WRITE API KEY!!
  *****************************************************************************************/

unsigned long myChannelNumber1 = 349627;
const char * myWriteAPIKey1 = "QVGE14HRYNR24LPV";

 unsigned long myChannelNumber2 = 349628;
 const char * myWriteAPIKey2 = "DOZEAV5W1AVD1GOA";

void setup() {
  Ethernet.begin(mac);
  ThingSpeak.begin(client);
}

void loop() {
  long timeRead = millis();
  // read the input on analog pin 0:
  int sensorValue1 = analogRead(A0);
   int sensorValue2 = analogRead(A1);

  // Convert the analog reading 
  // On UnoArduino:  0 - 1023 maps to 0 - 5 volts
  float voltage1 = sensorValue1 * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS);
  float voltage2 = sensorValue2 * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS);

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.

  ThingSpeak.setField(1, voltage1);
  // ThingSpeak.setField(2, sensor2Voltage);
  // ThingSpeak.setField(3, sensor3Meaning);
  ThingSpeak.setField(4, timeRead);
  ThingSpeak.setCreatedAt("2017-01-07T13:56:28");
  ThingSpeak.writeFields(myChannelNumber1, myWriteAPIKey1);

  ThingSpeak.setField(1, voltage2);
  // ThingSpeak.setField(2, sensor2Voltage);
  // ThingSpeak.setField(3, sensor3Meaning);
  ThingSpeak.setField(4, timeRead);
  ThingSpeak.setCreatedAt("2017-01-08T13:56:28");
  ThingSpeak.writeFields(myChannelNumber2, myWriteAPIKey2);
  // ThingSpeak.writeField(myChannelNumber1, 1, voltage1, myWriteAPIKey1);
  // ThingSpeak.writeField(myChannelNumber2, 1, voltage2, myWriteAPIKey2);
  delay(20000); // ThingSpeak will only accept updates every 15 seconds.
}