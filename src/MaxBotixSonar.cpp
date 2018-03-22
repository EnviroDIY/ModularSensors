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


MaxBotixSonar::MaxBotixSonar(Stream* stream, int8_t powerPin, int8_t triggerPin, uint8_t measurementsToAverage)
    : Sensor(F("MaxBotixMaxSonar"), HRXL_NUM_VARIABLES,
             HRXL_WARM_UP_TIME_MS, HRXL_STABILIZATION_TIME_MS, HRXL_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage)
{
    _triggerPin = triggerPin;
    _stream = stream;
}
MaxBotixSonar::MaxBotixSonar(Stream& stream, int8_t powerPin, int8_t triggerPin, uint8_t measurementsToAverage)
    : Sensor(F("MaxBotixMaxSonar"), HRXL_NUM_VARIABLES,
             HRXL_WARM_UP_TIME_MS, HRXL_STABILIZATION_TIME_MS, HRXL_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage)
{
    _triggerPin = triggerPin;
    _stream = &stream;
}


// unfortunately, we really cannot know where the stream is attached.
String MaxBotixSonar::getSensorLocation(void)
{
    // attach the trigger pin to the stream number
    String loc = "sonarStream_trigger" + String(_triggerPin);
    return loc;
}


bool MaxBotixSonar::setup(void)
{
    // Set up the trigger, if applicable
    if(_triggerPin != -1)
    {
        pinMode(_triggerPin, OUTPUT);
        digitalWrite(_triggerPin, LOW);
    }

    // Set the stream timeout;
    // Even the slowest sensors should respond at a rate of 6Hz (166ms).
    _stream->setTimeout(180);

    return Sensor::setup();  // this will set timestamp and status bit
}


// Parsing and tossing the header lines in the wake-up
bool MaxBotixSonar::wake(void)
{
    bool isAwake = Sensor::wake();  // takes care of timing stamps and status bits

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


bool MaxBotixSonar::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a new reading to be available
    waitForMeasurementCompletion();

    bool success = false;
    int rangeAttempts = 0;
    int result = -9999;

    MS_DBG(F("Beginning detection for Sonar\n"));
    while (success == false && rangeAttempts < 50)
    {
        if(_triggerPin != -1)
        {
            MS_DBG(F("Triggering Sonar\n"));
            digitalWrite(_triggerPin, HIGH);
            delayMicroseconds(30);  // Trigger must be held low for >20 µs
            digitalWrite(_triggerPin, LOW);
        }

        result = _stream->parseInt();
        _stream->read();  // To throw away the carriage return
        MS_DBG(F("Sonar Range: "), result, F("\n"));
        rangeAttempts++;

        // If it cannot obtain a result , the sonar is supposed to send a value
        // just above it's max range.  For 10m models, this is 9999, for 5m models
        // it's 4999.  The sonar might also send readings of 300 or 500 (the
        // blanking distance) if there are too many acoustic echos.
        // If the result becomes garbled or the sonar is disconnected, the parseInt function returns 0.
        if (result <= 300 || result == 500 || result == 4999 || result == 9999 || result == 0)
        {
            MS_DBG(F("Bad or Suspicious Result, Retry Attempt #"), rangeAttempts, F("\n"));
            result = -9999;
        }
        else
        {
            MS_DBG(F("Good result found\n"));
            success = true;
        }
    }

    verifyAndAddMeasurementResult(HRXL_VAR_NUM, result);

    // Unset the time stamp for the beginning of this measurements
    _millisMeasurementRequested = 0;

    // Return true when finished
    return true;
}
