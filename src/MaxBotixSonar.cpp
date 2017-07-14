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
 *
 * Warm up time to completion of header:  160ms
 */

#include "MaxBotixSonar.h"

MaxBotixSonar::MaxBotixSonar(int powerPin, Stream* stream, int triggerPin)
: Sensor(powerPin, -1, F("MaxBotixMaxSonar"), HRXL_NUM_MEASUREMENTS, HRXL_WARM_UP)
{
    _triggerPin = triggerPin;
    _stream = stream;
}
MaxBotixSonar::MaxBotixSonar(int powerPin, Stream& stream, int triggerPin)
: Sensor(powerPin, -1, F("MaxBotixMaxSonar"), HRXL_NUM_MEASUREMENTS, HRXL_WARM_UP)
{
    _triggerPin = triggerPin;
    _stream = &stream;
}

// unfortunately, we really cannot know where the stream is attached.
String MaxBotixSonar::getSensorLocation(void){return F("sonarStream");}

SENSOR_STATUS MaxBotixSonar::setup(void)
{
    pinMode(_powerPin, OUTPUT);

    if(_triggerPin != -1)
    {
        pinMode(_triggerPin, OUTPUT);
        digitalWrite(_triggerPin, LOW);
    }

    DBGM(F("Set up "), getSensorName(), F(" attached at "), getSensorLocation());
    DBGM(F(" which can return up to "), _numReturnedVars, F(" variable[s].\n"));

    return SENSOR_READY;
}

bool MaxBotixSonar::update(void)
{
    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    // Wait until the sensor is warmed up
    waitForWarmUp();

    // Set the stream timeout;
    // Even the slowest sensors should respond at a rate of 6Hz (166ms).
    _stream->setTimeout(180);

    // Clear values before starting loop
    clearValues();

    // NOTE: After the power is turned on to the MaxBotix, it sends several lines
    // of header to the serial pin, beginning at ~65ms and finising at ~160ms.
    // For an HRXL without temperature compensation, the headers are:
    // HRXL-MaxSonar-WRL
    // PN:MB7386
    // Copyright 2011-2013
    // MaxBotix Inc.
    // RoHS 1.8b090  0713
    // TempI

    DBGM(F("Parsing Header Lines\n"));
    for(int i=0; i < 6; i++);
    {
        String headerLine = _stream->readStringUntil('\r');
        DBGM(headerLine, F("\n"));
    }

    bool stringComplete = false;
    int rangeAttempts = 0;
    int result = 0;

    DBGM(F("Beginning detection for Sonar\n"));
    while (stringComplete == false && rangeAttempts < 50)
    {
        if(_triggerPin != -1)
        {
            DBGM(F("Triggering Sonar\n"));
            digitalWrite(_triggerPin, HIGH);
            delay(1);
            digitalWrite(_triggerPin, LOW);
            delay(160);  // Published return time is 158ms
        }

        result = _stream->parseInt();
        _stream->read();  // To throw away the carriage return
        DBGM(result, F("\n"));
        rangeAttempts++;

        // If it cannot obtain a result , the sonar is supposed to send a value
        // just above it's max range.  For 10m models, this is 9999, for 5m models
        // it's 4999.  The sonar might also send readings of 300 or 500 (the
        //  blanking distance) if there are too many acoustic echos.
        // If the result becomes garbled or the sonar is disconnected, the parseInt function returns 0.
        if (result <= 300 || result == 500 || result == 4999 || result == 9999)
        {
            DBGM(F("Bad or Suspicious Result, Retry Attempt #"), rangeAttempts, F("\n"));
        }
        else
        {
            DBGM(F("Good result found\n"));
            stringComplete = true;  // Set completion of read to true
        }
    }

    sensorValues[HRXL_VAR_NUM] = result;
    DBGM(sensorValues[HRXL_VAR_NUM], F("\n"));

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Update the registered variables with the new values
    notifyVariables();

    // Return true when finished
    return true;
}
