/******************************************************************************
Tally_I2C_Read.ino
Demo sketch for demonstrating the use of the Tall_I2C library
For use with Tally > v1.0
Bobby Schulz @ Northern Widget LLC
6/26/2019
https://github.com/NorthernWidget-Skunkworks/Tally_Library/examples

"On two occasions I have been asked, 'Pray, Mr. Babbage, if you put into the machine wrong figures, will the right answers come out?'
I am not able rightly to apprehend the kind of confusion of ideas that could provoke such a question."
-Charles Babbage

Distributed as-is; no warranty is given.
******************************************************************************/

#include <Tally_I2C.h>

Tally_I2C Counter;  //Instantiate counter device

unsigned long Period = 5000; //Wait 5 seconds between samples

void setup() {
	uint8_t Stat = false; //Used to test for connectivity to device
	Serial.begin(115200); //Initialize serial communication
	Serial.println("Welcome to the Counting Machine...");  //Obligatory welcome
	Serial.print("Period = "); Serial.print(Period); Serial.println(" ms"); //Display preset period
	Serial.print("Status = ");  //Prints status of device, if device is detected, prints PASS, otherwise prints ERROR
	Stat = Counter.begin(); //Initalize counter
	Counter.Sleep();  //Engage sleep mode
	if(Stat == 0) Serial.println("PASS");
	else Serial.println("ERROR");
	Counter.Clear(); //Clear device count on startup to ensure first reading is valid
}

void loop() {
	static unsigned long Time = millis(); //Initialize time to begin counting period
	uint16_t Data = 0; //Value to temporarily store the count data from the Tally device

	while((millis() - Time) < Period); //Wait for rollover
	Data = Counter.Peek(); //Read data from counter without clearing
	Counter.Clear(); //Clear count value
	Time = millis(); //Capture time

	float Frequency = Data/(Period/1000.0);  // average event frequency in Hz
	float WindSpeed = Frequency * 2.5 * 1.60934;  // in km/h, from 2.5 mph/Hz & 1.60934 kmph/mph
	// 2.5 mph/Hz conversion factor from https://www.store.inspeed.com/Inspeed-Version-II-Reed-Switch-Anemometer-Sensor-Only-WS2R.htm

	Serial.print("Count = ");
	Serial.print(Data);		//Print number of events in period
	Serial.println(" Events");

	Serial.print("Freq = ");
	Serial.print(Frequency);  //Print average event frequency in Hz
	Serial.println(" Hz");

	Serial.print("Wind Speed = ");
	Serial.print(WindSpeed);  //Print average wind speed in km/h
	Serial.println(" km/h");
}
