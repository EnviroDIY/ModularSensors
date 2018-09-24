/*****************************************************************************
PaleoTerraRedox_AddressSearch.ino

This scans through all possible addresses for the Paleo Terra Redox sensors 
and prints out their locations if found on the I2C bus

If no devices are detected, make sure they are properly connected, and all other 
I2C devices are disconected (this is only an issue if their is an address clash)
*****************************************************************************/

#include <Wire.h>

const int PwrPin = 22;  // The pin sending power to the sensors

void setup() {

	pinMode(PwrPin, OUTPUT);  //Turn switched power on to ensure I2C devices are active
    digitalWrite(PwrPin, HIGH);

	Wire.begin(); //Begin I2C comunication (Talk to Paleoterra devices)
	Serial.begin(57600); //Begin serial comunication (Talk to user in serial monitor)
}

void loop() {
	int N = 0; //Counter for number of devices found
	Serial.println("PaleoTerra Search Begin...\n"); //Print begin statment 
	for(int i = 0x69; i <= 0x6F; i++) {
		Wire.beginTransmission(i);
		Wire.write(0x00);
		if(Wire.endTransmission() == 0) {  //If device exists at address, print statment 
			Serial.print("Found Sensor "); 
			Serial.print(N);
			Serial.print(" at address 0x");
			Serial.println(i, HEX);
			N++; //Increment N if device has been found
		}
	}

	Serial.println("\n...Search completed!");

	if(N == 0) {  //If no sensors were found, give warning message
		Serial.println("No sensors found, ensure devices are connected correctly");
	}

	else if(N > 0) {  //If sensors are found, print how many were found
		Serial.print("Found ");
		Serial.print(N); 
		Serial.println(" Sensors");
	}
	
	Serial.println("Press reset to repeat search\n\n");

	while(1);
}