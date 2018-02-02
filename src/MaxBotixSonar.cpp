/*
 *MaxBotixSonar.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the MaxBotix Sonar Library
 *
 * The output from the HRXL-MaxSonar-WRL sonar is the range in mm.
 *
 * Warm up time to completion of header:  160ms
 */

#include "MaxBotixSonar.h"


MaxBotixSonar::MaxBotixSonar(Stream* stream, int powerPin, int triggerPin, int readingsToAverage)
    : Sensor(F("MaxBotixMaxSonar"), HRXL_NUM_VARIABLES,
             HRXL_WARM_UP, HRXL_STABILITY, HRXL_RESAMPLE,
             powerPin, -1, readingsToAverage)
{
    _triggerPin = triggerPin;
    _stream = stream;
}
MaxBotixSonar::MaxBotixSonar(Stream& stream, int powerPin, int triggerPin, int readingsToAverage)
    : Sensor(F("MaxBotixMaxSonar"), HRXL_NUM_VARIABLES,
             HRXL_WARM_UP, HRXL_STABILITY, HRXL_RESAMPLE,
             powerPin, -1, readingsToAverage)
{
    _triggerPin = triggerPin;
    _stream = &stream;
}


// unfortunately, we really cannot know where the stream is attached.
String MaxBotixSonar::getSensorLocation(void){return F("sonarStream");}


SENSOR_STATUS MaxBotixSonar::setup(void)
{
    // Set the stream timeout;
    // Even the slowest sensors should respond at a rate of 6Hz (166ms).
    _stream->setTimeout(180);
    // Set up the trigger, if applicable
    if(_triggerPin != -1)
    {
        pinMode(_triggerPin, OUTPUT);
        digitalWrite(_triggerPin, LOW);
    }
    return Sensor::setup();
}


// Parsing and tossing the header lines in the wake-up
bool MaxBotixSonar::wake(void)
{
    bool isAwake = Sensor::wake();

    // NOTE: After the power is turned on to the MaxBotix, it sends several lines
    // of header to the serial port, beginning at ~65ms and finising at ~160ms.
    // Although we are waiting for them to complete in the "waitForWarmUp"
    // function, the values will still be in the serial buffer and need
    // to be read to be cleared out
    // For an HRXL without temperature compensation, the headers are:
    // HRXL-MaxSonar-WRL
    // PN:MB7386
    // Copyright 2011-2013
    // MaxBotix Inc.
    // RoHS 1.8b090  0713
    // TempI

    MS_DBG(F("Parsing Header Lines\n"));
    for(int i = 0; i < 6; i++)
    {
        String headerLine = _stream->readStringUntil('\r');
        MS_DBG(i, F(" - "), headerLine, F("\n"));
    }
    return isAwake;
}


// nothing needs to happen to start an individual measurement
bool MaxBotixSonar::startSingleMeasurement(void)
{
    _lastMeasurementRequested = millis();
    return true;
}


bool MaxBotixSonar::addSingleMeasurementResult(void)
{
    bool stringComplete = false;
    int rangeAttempts = 0;
    int result = 0;

    MS_DBG(F("Beginning detection for Sonar\n"));
    while (stringComplete == false && rangeAttempts < 50)
    {
        if(_triggerPin != -1)
        {
            MS_DBG(F("Triggering Sonar\n"));
            digitalWrite(_triggerPin, HIGH);
            delay(1);
            digitalWrite(_triggerPin, LOW);
            delay(160);  // Published return time is 158ms
        }

        result = _stream->parseInt();
        _stream->read();  // To throw away the carriage return
        MS_DBG(result, F("\n"));
        rangeAttempts++;

        // If it cannot obtain a result , the sonar is supposed to send a value
        // just above it's max range.  For 10m models, this is 9999, for 5m models
        // it's 4999.  The sonar might also send readings of 300 or 500 (the
        //  blanking distance) if there are too many acoustic echos.
        // If the result becomes garbled or the sonar is disconnected, the parseInt function returns 0.
        if (result <= 300 || result == 500 || result == 4999 || result == 9999)
        {
            MS_DBG(F("Bad or Suspicious Result, Retry Attempt #"), rangeAttempts, F("\n"));
        }
        else
        {
            MS_DBG(F("Good result found\n"));
            stringComplete = true;  // Set completion of read to true
        }
    }

    sensorValues[HRXL_VAR_NUM] += result;
    MS_DBG(F("Sonar Range: "), result, F("\n"));

    // Return true when finished
    return true;
}
