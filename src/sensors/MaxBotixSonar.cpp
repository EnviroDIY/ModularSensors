/**
 * @file MaxBotixSonar.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the MaxBotixSonar class.
 */

#include "MaxBotixSonar.h"


MaxBotixSonar::MaxBotixSonar(Stream* stream, int8_t powerPin, int8_t triggerPin,
                             uint8_t measurementsToAverage)
    : Sensor("MaxBotixMaxSonar", HRXL_NUM_VARIABLES, HRXL_WARM_UP_TIME_MS,
             HRXL_STABILIZATION_TIME_MS, HRXL_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage),
      _triggerPin(triggerPin),
      _stream(stream) {}
MaxBotixSonar::MaxBotixSonar(Stream& stream, int8_t powerPin, int8_t triggerPin,
                             uint8_t measurementsToAverage)
    : Sensor("MaxBotixMaxSonar", HRXL_NUM_VARIABLES, HRXL_WARM_UP_TIME_MS,
             HRXL_STABILIZATION_TIME_MS, HRXL_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage, HRXL_INC_CALC_VARIABLES),
      _triggerPin(triggerPin),
      _stream(&stream) {}
// Destructor
MaxBotixSonar::~MaxBotixSonar() {}


// unfortunately, we really cannot know where the stream is attached.
String MaxBotixSonar::getSensorLocation(void) {
    // attach the trigger pin to the stream number
    String loc = "sonarStream_trigger" + String(_triggerPin);
    return loc;
}


bool MaxBotixSonar::setup(void) {
    // Set up the trigger, if applicable
    if (_triggerPin >= 0) {
        pinMode(_triggerPin, OUTPUT);
        digitalWrite(_triggerPin, LOW);
    }

    // Set the stream timeout
    // Even the slowest sensors should respond at a rate of 6Hz (166ms).
    _stream->setTimeout(180);

    return Sensor::setup();  // this will set pin modes and the setup status bit
}


// Parsing and tossing the header lines in the wake-up
bool MaxBotixSonar::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;

    // NOTE: After the power is turned on to the MaxBotix, it sends several
    // lines of header to the serial port, beginning at ~65ms and finising at
    // ~160ms. Although we are waiting for them to complete in the
    // "waitForWarmUp" function, the values will still be in the serial buffer
    // and need to be read to be cleared out For an HRXL without temperature
    // compensation, the headers are: HRXL-MaxSonar-WRL PN:MB7386 Copyright
    // 2011-2013 MaxBotix Inc. RoHS 1.8b090  0713 TempI

    // NOTE ALSO:  Depending on what type of serial stream you are using, there
    // may also be a bunch of junk in the buffer that this will clear out.
    MS_DBG(F("Dumping Header Lines from MaxBotix on"), getSensorLocation());
    for (int i = 0; i < 6; i++) {
        String headerLine = _stream->readStringUntil('\r');
        MS_DBG(i, '-', headerLine);
    }
    // Clear anything else out of the stream buffer
    auto junkChars = static_cast<uint8_t>(_stream->available());
    if (junkChars) {
        MS_DBG(F("Dumping"), junkChars,
               F("characters from MaxBotix stream buffer"));
        for (uint8_t i = 0; i < junkChars; i++) {
#ifdef MS_MAXBOTIXSONAR_DEBUG
            DEBUGGING_SERIAL_OUTPUT.print(_stream->read());
#else
            _stream->read();
#endif
        }
#ifdef MS_MAXBOTIXSONAR_DEBUG
        DEBUGGING_SERIAL_OUTPUT.println();
#endif
    }

    return true;
}


bool MaxBotixSonar::addSingleMeasurementResult(void) {
    // Initialize values
    bool    success = false;
    int16_t result  = -9999;

    // Clear anything out of the stream buffer
    auto junkChars = static_cast<uint8_t>(_stream->available());
    if (junkChars) {
        MS_DBG(F("Dumping"), junkChars,
               F("characters from MaxBotix stream buffer:"));
        for (uint8_t i = 0; i < junkChars; i++) {
#ifdef MS_MAXBOTIXSONAR_DEBUG
            DEBUGGING_SERIAL_OUTPUT.print(_stream->read());
#else
            _stream->read();
#endif
        }
#ifdef MS_MAXBOTIXSONAR_DEBUG
        DEBUGGING_SERIAL_OUTPUT.println();
#endif
    }

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        uint8_t rangeAttempts = 0;
        while (success == false && rangeAttempts < 25) {
            // If the sonar is running on a trigger, activating the trigger
            // should in theory happen within the startSingleMeasurement
            // function.  Because we're really taking up to 25 measurements
            // for each "single measurement" until a valid value is returned
            // and the measurement time is <166ms, we'll actually activate
            // the trigger here.
            if (_triggerPin >= 0) {
                MS_DBG(F("  Triggering Sonar with"), _triggerPin);
                digitalWrite(_triggerPin, HIGH);
                delayMicroseconds(30);  // Trigger must be held high for >20 µs
                digitalWrite(_triggerPin, LOW);
            }

            // Immediately ask for a result and let the stream timeout be our
            // "wait" for the measurement.
            result = static_cast<uint16_t>(_stream->parseInt());
            _stream->read();  // To throw away the carriage return
            MS_DBG(F("  Sonar Range:"), result);
            rangeAttempts++;

            // If it cannot obtain a result , the sonar is supposed to send a
            // value just above it's max range.  For 10m models, this is 9999,
            // for 5m models it's 4999.  The sonar might also send readings of
            // 300 or 500 (the blanking distance) if there are too many acoustic
            // echos. If the result becomes garbled or the sonar is
            // disconnected, the parseInt function returns 0.  Luckily, these
            // sensors are not capable of reading 0, so we also know the 0 value
            // is bad.
            if (result <= 300 || result == 500 || result == 4999 ||
                result == 9999 || result == 0) {
                MS_DBG(F("  Bad or Suspicious Result, Retry Attempt #"),
                       rangeAttempts);
                result = -9999;
            } else {
                MS_DBG(F("  Good result found"));
                success = true;
            }
        }
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(HRXL_VAR_NUM, result);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return values shows if we got a not-obviously-bad reading
    return success;
}
