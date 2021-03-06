#include "ThingSpeak.h"
// Use SDcard & ethernet shield
#include <Ethernet.h>
#include <SD.h>
// Dallas Temperature Sensor
#include <OneWire.h>
#include <DallasTemperature.h>
// RTC time module DS3231 SCL>21 SDA>20
#include <RTClib.h>
#include "Time.h"
#include "TimeAlarms.h"
//
// Initialize Arduino RTC
RTC_DS3231 RTC;
//
uint32_t syncProvider() { //function which sets up the RTC as the source of external time
	return RTC.now().unixtime();
}
//
// Local Network Settings
byte mac[] = { 0xD4, 0x28, 0xB2, 0xFF, 0xA0, 0xA1 };
EthernetClient client;
//
// Dallas Temperature Sensor Settings
OneWire ds18x20[] = { 3 }; // Pin location
const int noTempVal = sizeof(ds18x20)/sizeof(OneWire);
DallasTemperature sensor[noTempVal];
float TempCount[noTempVal];
//
// ThingSpeak Settings
unsigned long myChannelNumber[] = { 349627, 349628 };
const char * myWriteAPIKey[] = { "QVGE14HRYNR24LPV", "DOZEAV5W1AVD1GOA" };
const int noChannels = sizeof(myChannelNumber);
//
// Analog sensor readout
byte ADCsensors[] = {A0, A1, A2, A3, A4, A5};
const int noPowerVal = sizeof(ADCsensors)/sizeof(ADCsensors[0]);
float voltage[noPowerVal];
float PowerCount[noPowerVal];
//
// On Arduino:  0 - 1023 maps to 0 - 5 volts
#define VOLTAGE_MAX 5.0
#define VOLTAGE_MAXCOUNTS 1023.0
#define resValue 1   // shunt value
#define SampleTime 30 // value in seconds
//
File Logfile;
DateTime current;
//
// End of Declerations
//
void setup() {
	// Start Serial
	Serial.begin(9600);
	// Get current time from RTC
	RTC.begin();
	setSyncProvider(syncProvider);   // the function to get the time from the RTC
	Alarm.timerRepeat(SampleTime, Repeats);            // timer for every 15 seconds    
	Serial.println(" Program Started 7 ");
	Serial.println("No of Temperature Sensors: ");
	Serial.println(noTempVal);
	Serial.println("No of Power Sensors: ");
	Serial.println(noPowerVal);
	// Initialize Ethernet
	startEthernet();
	// Initialize SDcard
	startSDcard();
	// Initialize DallasTemperature sensor
	startDallasTemp();
	// Initialize ThingSpeak API
	ThingSpeak.begin(client);
}
//
// Main loop of program
//
void  loop(){  
	Serial.println(" main loop ");
	Alarm.delay(1000); // wait one second between clock display
}
//
// Alarm loop of program
//
void Repeats(){
	Serial.println("!!!! MAIN LOOP STARTED ");
	current = RTC.now();
	readADC();
	readDallasTemp();
	StoreToSD();
	SendThings();
	Serial.println("!!!! MAIN LOOP ENDED ");     
}
//
// ThingSpeak Function
//
void SendThings() {
	Serial.println("Starting ThingSpeak...");
	long timeRead = millis();
	char datetime [35] = "";
	sprintf(datetime, "%04d-%02d-%02dT%02d:%02d:%02d+02", current.year(), current.month(), current.day(), current.hour(), current.minute(), current.second());

	Serial.println("Sending data to ThingSpeak...");
	Serial.println();  

	for (int thisPower = 0;  thisPower < noPowerVal;  thisPower++) {
		ThingSpeak.setField(thisPower+1, PowerCount[thisPower]);
		Serial.println((thisPower+1));
		if (thisPower+1 == noPowerVal) {
			ThingSpeak.setField((thisPower), timeRead);
			ThingSpeak.setCreatedAt(datetime);
			ThingSpeak.writeFields(myChannelNumber[0], myWriteAPIKey[0]);
			Serial.println("power fields done");
		}
	}
	for (int thisTemp = 0;  thisTemp < noTempVal;  thisTemp++) {
		ThingSpeak.setField(thisTemp+1, TempCount[thisTemp]);
		Serial.println((thisTemp+1));
		if (thisTemp+1 == noTempVal) {
			ThingSpeak.setField((thisTemp), timeRead);
			ThingSpeak.setCreatedAt(datetime);
			ThingSpeak.writeFields(myChannelNumber[1], myWriteAPIKey[1]);
			Serial.println("temp fields done");
		}
	}
	Serial.println("ThingSpeak done");
	Serial.println();  
}
//
// Ethernet Connection Function
//
void startEthernet() {
	client.stop();
	Serial.println("Connecting Arduino to network...");
	Serial.println();  
	delay(1000);
	// Connect to network amd obtain an IP address using DHCP
	if (Ethernet.begin(mac) == 0) {
		Serial.println("DHCP Failed, reset Arduino to try again");
		Serial.println();
	} else {
		Serial.println("Arduino connected to network using DHCP");
		// print your local IP address:
		Serial.println(Ethernet.localIP());
		Serial.println();
	}
	delay(1000);
}
//
// SDcard Functions
//
void startSDcard() {
	Serial.println("Initializing SDcard...");
	Serial.println();

	if (!SD.begin(4)) {
		Serial.println("SD initialization failed!");
		return;
	} else {
		Serial.println("SD initialization done.");
	}
}
//
char * getFileName() {
	char * filename = (char *) malloc (666);
	sprintf(filename, "%02d%02d%02d.csv", current.year(), current.month(), current.day());
return filename;
}
//
void StoreToSD() {
	Serial.println("Storing to SD ");
	Logfile=SD.open(getFileName(),FILE_WRITE);       //Will open and will write date
	// if the file opened okay, write to it:
	if(Logfile) {
		Logfile.print(current.year(), DEC);       
		Logfile.print("/");
		Logfile.print(current.month(), DEC);
		Logfile.print("/");
		Logfile.print(current.day(), DEC);
		Logfile.print(",");

		Logfile.print(current.hour(), DEC);
		Logfile.print(":");
		Logfile.print(current.minute(), DEC);
		Logfile.print(":");
		Logfile.print(current.second(), DEC);
		Logfile.print(",");

		for (int thisPower = 0;  thisPower < noPowerVal;  thisPower++) {
			Logfile.print(PowerCount[thisPower], DEC);
			Logfile.print(",");
		  }
		for (int thisTemp = 0;  thisTemp < noTempVal;  thisTemp++) {
			Logfile.print(TempCount[thisTemp], DEC);
			Logfile.print(",");
		  }

	} else {
		// if the file didn't open, print an error:
	    Serial.println("error opening ");
	    Serial.print(getFileName());
	}
	Logfile.print("\n");
	Logfile.close();
	Serial.println("Storing to SD DONE !!! ");
}
//
// DallasTemperature Functions
//
void startDallasTemp() {
// Start up the library on all defined bus-wires
Serial.println("Initializing DallasTemperature...");
  DeviceAddress deviceAddress;
  for (int i = 0; i < noTempVal; i++) {;
    sensor[i].setOneWire(&ds18x20[i]);
    sensor[i].begin();
    if (sensor[i].getAddress(deviceAddress, 0)) sensor[i].setResolution(deviceAddress, 12);
  }
  Serial.println("DallasTemperature DONE !!!");
}
//
void readDallasTemp() {
	Serial.println("Requesting temperatures...");
	for (int i = 0; i < noTempVal; i++) {
		sensor[i].requestTemperatures();
	}
	Serial.println("DONE");
	delay(100);
	for (int i = 0; i < noTempVal; i++) {
		TempCount[i] = sensor[i].getTempCByIndex(0);
		Serial.print("Temperature for the sensor ");
		Serial.print(i);
		Serial.print(" is ");
		Serial.println(TempCount[i]);
	}
	Serial.println();
	Serial.println("DONE reading temperatures");
	Serial.println();
} 
//
// ADC reading Function
//
void readADC() {
	Serial.print("Requesting Power Values...");
	for (int i = 0; i < noPowerVal; i++) {
		voltage[i] = analogRead(ADCsensors[i]);
	}
	Serial.println("DONE");
	delay(100);
	for (int i = 0; i < noPowerVal; i++) {
		PowerCount[i] = voltage[i] * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS) * resValue;
		Serial.print("Power for the sensor ");
		Serial.print(i);
		Serial.print(" is ");
		Serial.println(PowerCount[i]);
	}
	Serial.println();
	Serial.println("DONE reading power");
	Serial.println();
}
