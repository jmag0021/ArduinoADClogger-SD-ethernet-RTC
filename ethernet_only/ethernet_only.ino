// Ethernet Shield HanRun HR911105A
#include <SPI.h>
#include <Ethernet.h>

// Local Network Settings
byte mac[] = { 0xD4, 0x28, 0xB2, 0xFF, 0xA0, 0xA1 }; // Must be unique on local network

// Initialize Arduino Ethernet Client
EthernetClient client;

void setup(void)
{
  Serial.begin(9600);
  Serial.println();
  // Start Ethernet on Arduino
  startEthernet();
}

void startEthernet()
{
  client.stop();

  Serial.println("Connecting Arduino to network...");
  Serial.println();  

  delay(1000);
  
  // Connect to network amd obtain an IP address using DHCP
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("DHCP Failed, reset Arduino to try again");
    Serial.println();
  }
  else
  {
    Serial.println("Arduino connected to network using DHCP");
    // print your local IP address:
    Serial.println(Ethernet.localIP());
    Serial.println();
  }
  delay(1000);
}

void loop(void) {
}