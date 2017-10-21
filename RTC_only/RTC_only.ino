// RTC time module DS3231

// SCL -> 21
// SDA -> 20

#include <RTClib.h>

// Initialize Arduino RTC
RTC_DS3231 RTC; // define the Real Time Clock object

void setup() {
	RTC.begin();
	Serial.begin(9600);
	Serial.println();
	// used to set date and time
	// RTC.adjust(DateTime(2017, 10, 21, 11, 50, 0));
}

void loop(){
	// fetch the time	
	DateTime now = RTC.now();
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

	delay(1000);                                    //One data per second
}