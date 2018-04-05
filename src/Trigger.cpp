

#include "Trigger.h"

//TripValue is value which when exceeded, the pin changes polarity, OutputPin is the pin which is switched,
//NumVals are the number of average samples taken, Polarity is the value which should be present when triggered (1 for HIGH, 0 for LOW)
//
Trigger::Trigger(float _TripValue, int _OutputPin, int _NumVals, int _Polarity) { 
	TripValue = _TripValue;
	OutputPin = _OutputPin;
	NumVals = _NumVals;
	Polarity = _Polarity;
	TriggeredValue = Polarity;  //Set triggerd value based on polarity
	IdleValue = 1 - Polarity;  //Set idle value to opposite of triggerd value
}

void Trigger::begin() {
	initOutput();
}

bool Trigger::test(float Val) {
	// static LastStatus = false; //Keep state of the system, if the last value it receved was above or below the trip point
	static bool TripStatus = false; //Used to see if device has been tripped or not
	static int Count = 0;

	if(Val > TripValue && TripStatus == false) {  //trip value, device idle
		Count++;  //Increment count of trip values
	}

	else if(Val < TripValue && TripStatus == false) {  //idle value, device idle
		Count = 0;  //Clear count if you get a single bad value before you have been tripped
	}

	else if(Val < TripValue && TripStatus == true) {  //idle value, device tripped
		Count--; //Decrement count if you get a bad value but the device is already tripped
	}

	else if(Val > TripValue && TripStatus == true) {  //trip value, device tripped
		//Do nothing in this call
	}

	if(Count >= NumVals && TripStatus == false) {
		triggerOutput(true); //If device gets n concecutive trip vals, trip
		TripStatus = true;
	}
	if(Count == 0 && TripStatus == true) {
		triggerOutput(false); //If device gets n concecutive idle vals, idle
		TripStatus = false;
	}
	return TripStatus;
}

bool Trigger::test(int Val) {  //Overload to define test for integer values as well
	return test(float(Val));  //Inneficient coding, but simple to read, FIX later?? 
}

void Trigger::initOutput() {  //Moddify this function to setup whatever output is desired 
	pinMode(OutputPin, OUTPUT);
	digitalWrite(OutputPin, IdleValue); //Set pin to the "off" state
}

void Trigger::triggerOutput(bool Status) {  //Moddify this function to either set or clear output dependent of Status value

	if(Status) {  //Trigger output, place moddified code inside
		digitalWrite(OutputPin, TriggeredValue);
	}
	else {	//Idle output, place moddified code inside
		digitalWrite(OutputPin, IdleValue);
	}
}