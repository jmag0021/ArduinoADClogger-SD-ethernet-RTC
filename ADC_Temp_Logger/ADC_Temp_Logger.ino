
#include "ThingSpeak.h"

// Use wired ethernet shield
#include <Ethernet.h>

// Dallas Temperature Sensor
#include <OneWire.h>
#include <DallasTemperature.h>

// RTC time module DS3231
// SCL -> 21
// SDA -> 20
#include <RTClib.h>

// Initialize Arduino RTC
RTC_DS3231 RTC;

// Local Network Settings
byte mac[] = { 0xD4, 0x28, 0xB2, 0xFF, 0xA0, 0xA1 }; // Must be unique on local network
EthernetClient client;

// Dallas Temperature Sensor Settings
#define ONE_WIRE_BUS 2 // Pin location
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
#define TEMPERATURE_PRECISION 9
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature. 
DeviceAddress Thermometer1, Thermometer2, Thermometer3, Thermometer4, Thermometer5, Thermometer6, Thermometer7; // arrays to hold device addresses

// ThingSpeak Settings
unsigned long myChannelNumber1 = 349627;
const char * myWriteAPIKey1 = "QVGE14HRYNR24LPV";

unsigned long myChannelNumber2 = 349628;
const char * myWriteAPIKey2 = "DOZEAV5W1AVD1GOA";

// On Arduino:  0 - 1023 maps to 0 - 5 volts
#define VOLTAGE_MAX 5.0
#define VOLTAGE_MAXCOUNTS 1023.0
#define resValue 1

void setup() {
	RTC.begin();
	Ethernet.begin(mac);
	ThingSpeak.begin(client);
  	sensors.begin();
	Serial.begin(9600);

// start temp sensor

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

 // Assign address manually. The addresses below will beed to be changed
  // to valid device addresses on your bus. Device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  //insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
  //outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then 
  // use those addresses and manually assign them (see above) once you know 
  // the devices on your bus (and assuming they don't change).
  // 
  // method 1: by index
  if (!sensors.getAddress(Thermometer1, 0)) Serial.println("Unable to find address for Device 0"); 
  if (!sensors.getAddress(Thermometer2, 1)) Serial.println("Unable to find address for Device 1"); 

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices, 
  // or you have already retrieved all of them. It might be a good idea to 
  // check the CRC to make sure you didn't get garbage. The order is 
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");
  // assigns the seconds address found to outsideThermometer
  //if (!oneWire.search(outsideThermometer)) Serial.println("Unable to find address for outsideThermometer");

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(Thermometer1);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(Thermometer2);
  Serial.println();

  // set the resolution to 9 bit per device
  sensors.setResolution(Thermometer1, TEMPERATURE_PRECISION);
  sensors.setResolution(Thermometer2, TEMPERATURE_PRECISION);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(Thermometer1), DEC); 
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(Thermometer2), DEC); 
  Serial.println();

// end temp sensor
	//RTC.adjust(DateTime(__DATE__, __TIME__));
}

void loop() {

  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  // print the device information
  // printData(Thermometer1);
  // printData(Thermometer2);

	// fetch the time	
	DateTime now = RTC.now();
	char datetime [30] = "";
  	sprintf(datetime, "%04d-%02d-%02dT%02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour() - 2, now.minute(), now.second());

	// log time
	Serial.print('"');
	Serial.print(now.year(), DEC);
	Serial.print("/");
	Serial.print(now.month(), DEC);
	Serial.print("/");
	Serial.print(now.day(), DEC);
	Serial.print(" ");
	Serial.print(now.hour(), DEC);
	Serial.print(":");
	Serial.print(now.minute(), DEC);
	Serial.print(":");
	Serial.print(now.second(), DEC);
	Serial.print('"');


  long timeRead = millis();
  // read the input on analog pin 0:
  int sensorValue0 = analogRead(A0);
  int sensorValue1 = analogRead(A1);
  int sensorValue2 = analogRead(A2);
  int sensorValue3 = analogRead(A3);
  int sensorValue4 = analogRead(A4);
  int sensorValue5 = analogRead(A5);

  Serial.println(sensorValue0);
  // Convert the analog reading 
  // On UnoArduino:  0 - 1023 maps to 0 - 5 volts
  float power0 = sensorValue0 * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS) * resValue;
  float power1 = sensorValue1 * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS) * resValue;
  float power2 = sensorValue2 * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS) * resValue;
  float power3 = sensorValue3 * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS) * resValue;
  float power4 = sensorValue4 * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS) * resValue;
  float power5 = sensorValue5 * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS) * resValue;

Serial.println(power0);

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.

  // ThingSpeak Power Channel
  ThingSpeak.setField(1, power0);
  ThingSpeak.setField(2, power1);
  ThingSpeak.setField(3, power2);
  ThingSpeak.setField(4, power3);
  ThingSpeak.setField(5, power4);
  ThingSpeak.setField(6, power5);
  ThingSpeak.setField(7, timeRead);
  ThingSpeak.setCreatedAt(datetime);
  ThingSpeak.writeFields(myChannelNumber1, myWriteAPIKey1);
  	Serial.print('new');
  	Serial.print('"');
	Serial.print(datetime);
	Serial.print('"');

  // ThingSpeak Temp Channel
  // ThingSpeak.setField(1, temp0);
  // ThingSpeak.setField(2, temp1);
  // ThingSpeak.setField(3, temp2);
  // ThingSpeak.setField(4, temp3);
  // ThingSpeak.setField(5, temp4);
  // ThingSpeak.setField(6, temp5);
  // ThingSpeak.setField(7, temp6);
  // ThingSpeak.setField(8, timeRead);
  // ThingSpeak.setCreatedAt(date&time);
  // ThingSpeak.writeFields(myChannelNumber2, myWriteAPIKey2);

  delay(20000); // ThingSpeak will only accept updates every 15 seconds.
}


// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();    
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}