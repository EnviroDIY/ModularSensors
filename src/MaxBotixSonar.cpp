/*
 *MaxBotixSonar.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the MaxBotix Sonar Library
 *It is dependent on Software Serial.
 *
 * The output from the HRXL-MaxSonar-WRL sonar is the range in mm.
 */

#include "MaxBotixSonar.h"
#include <SoftwareSerial_PCINT12.h>

MaxBotixSonar::MaxBotixSonar(int powerPin, int dataPin, int triggerPin)
: Sensor(powerPin, dataPin, F("MaxBotixMaxSonar"), HRXL_NUM_MEASUREMENTS)
{_triggerPin = triggerPin;}

SENSOR_STATUS MaxBotixSonar::setup(void)
{
    pinMode(_powerPin, OUTPUT);
    pinMode(_dataPin, INPUT);
    digitalWrite(_powerPin, LOW);
    if(_triggerPin != -1)
    {
        pinMode(_triggerPin, OUTPUT);
        digitalWrite(_triggerPin, LOW);
    }

    // Serial.print(F("Set up "));  // for debugging
    // Serial.print(getSensorName());  // for debugging
    // Serial.print(F(" attached at "));  // for debugging
    // Serial.print(getSensorLocation());  // for debugging
    // Serial.print(F(" which can return up to "));  // for debugging
    // Serial.print(_numReturnedVars);  // for debugging
    // Serial.println(F(" variable[s]."));  // for debugging
    
    return SENSOR_READY;
}

bool MaxBotixSonar::update(void)
{
    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}  // powerUp function includes a 500ms delay
    else{delay(160);}  // See note below

    // Clear values before starting loop
    // clearValues();

    // NOTE: After the power is turned on to the MaxBotix, it sends several lines
    // of header to the serial pin, beginning at ~65ms and finising at ~160ms.
    // By not opening SoftwareSerial to begin listening until after this header
    // information is sent, we can completely eliminate the header.
    // For an HRXL without temperature compensation, the headers are:
    // HRXL-MaxSonar-WRL
    // PN:MB7386
    // Copyright 2011-2013
    // MaxBotix Inc.
    // RoHS 1.8b090  0713
    // TempI

    // define serial port for receiving data
    SoftwareSerial sonarSerial(_dataPin, -1);
    sonarSerial.begin(9600);
    // Even the slowest sensors should respond at a rate of 6Hz (166ms).
    sonarSerial.setTimeout(180);

    // Note:  if the power is on for >160ms before SoftwareSerial starts
    // the header lines will already have been sent and lost
    // Serial.println(F("Parsing Header Lines"));  // For debugging
    // for(int i=0; i < 6; i++)  // For debugging
    // {  // For debugging
    //     Serial.println(sonarSerial.readStringUntil('\r'));  // For debugging
    // }  // For debugging

    bool stringComplete = false;
    int rangeAttempts = 0;
    int result = 0;

    // Serial.println(F("Beginning detection for Sonar"));  // For debugging
    while (stringComplete == false && rangeAttempts < 50)
    {
        if(_triggerPin != -1)
        {
            // Serial.println(F("Triggering Sonar"));  // For debugging
            digitalWrite(_triggerPin, HIGH);
            delay(1);
            digitalWrite(_triggerPin, LOW);
            delay(160);  // Published return time is 158ms
        }

        result = sonarSerial.parseInt();
        sonarSerial.read();  // To throw away the carriage return
        // Serial.println(result);  // For debugging
        rangeAttempts++;

        // If it cannot obtain a result , the sonar is supposed to send a value
        // just above it's max range.  For 10m models, this is 9999, for 5m models
        // it's 4999.  The sonar might also send readings of 300 or 500 (the
        //  blanking distance) if there are too many acoustic echos.
        // If the result becomes garbled or the sonar is disconnected, the parseInt function returns 0.
        if (result <= 300 || result == 500 || result == 4999 || result == 9999)
        {
            // Serial.print(F("Bad or Suspicious Result, Retry Attempt #"));  // For debugging
            // Serial.println(rangeAttempts);  // For debugging
        }
        else
        {
            // Serial.println(F("Good result found"));  // For debugging
            stringComplete = true;  // Set completion of read to true
        }
    }

    sensorValues[HRXL_VAR_NUM] = result;
    // Serial.println(sensorValues[HRXL_VAR_NUM]);  // For debugging

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Update the registered variables with the new values
    notifyVariables();

    // Return true when finished
    return true;
}
