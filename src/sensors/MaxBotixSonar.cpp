/**
 * @file MaxBotixSonar.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the MaxBotixSonar class.
 */

#include "MaxBotixSonar.h"


MaxBotixSonar::MaxBotixSonar(Stream* stream, int8_t powerPin, int8_t triggerPin,
                             int16_t maxRange, uint8_t measurementsToAverage,
                             bool convertCm)
    : Sensor("MaxBotixMaxSonar", HRXL_NUM_VARIABLES, HRXL_WARM_UP_TIME_MS,
             HRXL_STABILIZATION_TIME_MS, HRXL_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage, HRXL_INC_CALC_VARIABLES),
      _maxRange(maxRange),
      _triggerPin(triggerPin),
      _convertCm(convertCm),
      _stream(stream) {
    setMaxRetries(MAXBOTIX_DEFAULT_MEASUREMENT_RETRIES);
}

// Delegating constructor
MaxBotixSonar::MaxBotixSonar(Stream& stream, int8_t powerPin, int8_t triggerPin,
                             int16_t maxRange, uint8_t measurementsToAverage,
                             bool convertCm)
    : MaxBotixSonar(&stream, powerPin, triggerPin, maxRange,
                    measurementsToAverage, convertCm) {}


// unfortunately, we really cannot know where the stream is attached.
String MaxBotixSonar::getSensorLocation() {
    // attach the trigger pin to the stream number
    String loc;
    loc.reserve(25);  // Reserve for "sonarStream_trigger" + pin number
    loc = "sonarStream_trigger" + String(_triggerPin);
    return loc;
}


bool MaxBotixSonar::setup() {
    // Set up the trigger, if applicable
    if (_triggerPin >= 0) {
        pinMode(_triggerPin, OUTPUT);
        digitalWrite(_triggerPin, LOW);
    }

    // Set the stream timeout
    // Even the slowest sensors should respond at a rate of 4Hz (250ms).
    _stream->setTimeout(250);

    return Sensor::setup();  // this will set pin modes and the setup status bit
}


// Parsing and tossing the header lines in the wake-up
bool MaxBotixSonar::wake() {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;
    // Set the trigger pin mode.
    // Reset this on every wake because pins are set to tri-state on sleep
    if (_triggerPin >= 0) {
        pinMode(_triggerPin, OUTPUT);
        digitalWrite(_triggerPin, LOW);
    }

    // NOTE: After the power is turned on to the MaxBotix, it sends several
    // lines of header to the serial port, beginning at ~65ms and finishing at
    // ~160ms. Although we are waiting for them to complete in the
    // "waitForWarmUp" function, the values will still be in the serial buffer
    // and need to be read to be cleared out For an HRXL without temperature
    // compensation, the headers are:
    // HRXL-MaxSonar-WRL
    // PN:MB7386
    // Copyright 2011-2013 MaxBotix Inc.
    // RoHS 1.8b090
    // 0713 TempI

    // NOTE ALSO:  Depending on what type of serial stream you are using, there
    // may also be a bunch of junk in the buffer that this will clear out.
    MS_DBG(F("Dumping Header Lines from MaxBotix on"), getSensorLocation());
    for (int i = 0; i < 6; i++) {
        String headerLine = _stream->readStringUntil('\r');
        MS_DBG(i, '-', headerLine);
    }
    // Clear anything else out of the stream buffer
    dumpBuffer();

    return true;
}

// The function to put the sensor to sleep
// Different from the standard in that empties and flushes the stream.
bool MaxBotixSonar::sleep() {
    // empty then flush the buffer
    while (_stream->available()) { _stream->read(); }
    _stream->flush();
    return Sensor::sleep();
}


bool MaxBotixSonar::startSingleMeasurement() {
    // Sensor::startSingleMeasurement() checks that if it's awake/active and
    // sets the timestamp and status bits.  If it returns false, there's no
    // reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    dumpBuffer();  // dump anything stuck in the stream buffer

    // If the sonar is running on a trigger, activate it
    if (_triggerPin >= 0) {
        MS_DBG(F("  Triggering Sonar with"), _triggerPin);
        digitalWrite(_triggerPin, HIGH);
        delayMicroseconds(30);  // Trigger must be held high for >20 µs
        digitalWrite(_triggerPin, LOW);

        // Update the time that a measurement was requested
        // For MaxBotix, we're actively triggering so mark time as now
        _millisMeasurementRequested = millis();
    }

    return true;
}


bool MaxBotixSonar::addSingleMeasurementResult() {
    // Perform common initialization checks
    if (!initializeMeasurementResult()) { return false; }

    // Initialize values
    bool    success = false;
    int16_t result  = MS_INVALID_VALUE;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Ask for a result and let the stream timeout be our "wait" for the
    // measurement
    result = static_cast<int16_t>(_stream->parseInt());
    _stream->read();  // Throw away the carriage return
    MS_DBG(F("  Sonar Range:"), result);

    // Check if result is valid
    // If it cannot obtain a result, the sonar sends a value just above its max
    // range. If the result becomes garbled or the sonar is disconnected,
    // parseInt returns 0. These sensors cannot read 0, so we know 0 is bad.
    if (result <= 0 || result >= _maxRange) {
        MS_DBG(F("  Bad or Suspicious Result, Retry #"), _currentRetries + 1);
        result = MS_INVALID_VALUE;
    } else {
        MS_DBG(F("  Good result found"));
        // convert result from cm to mm if convertCm is set to true
        if (_convertCm) { result *= 10; }
        success = true;
        verifyAndAddMeasurementResult(HRXL_VAR_NUM, result);
    }

    // dump anything left in the stream buffer
    dumpBuffer();

    // Return success value when finished
    return finalizeMeasurementAttempt(success);
}


void MaxBotixSonar::dumpBuffer() {
    auto junkChars = _stream->available();
    if (junkChars > 0) {
        MS_DBG(F("Dumping"), junkChars,
               F("characters from MaxBotix stream buffer:"));
        while (_stream->available()) {
#ifdef MS_MAXBOTIXSONAR_DEBUG
            MS_SERIAL_OUTPUT.print(_stream->read());
#else
            _stream->read();
#endif
        }
#ifdef MS_MAXBOTIXSONAR_DEBUG
        PRINTOUT(" ");
#endif
    }
}
