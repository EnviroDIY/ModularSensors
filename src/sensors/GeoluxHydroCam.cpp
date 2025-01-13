/**
 * @file GeoluxHydroCam.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the GeoluxHydroCam class.
 */

#include "GeoluxHydroCam.h"


GeoluxHydroCam::GeoluxHydroCam(Stream* stream, int8_t powerPin,
                               const char* imageResolution,
                               const char* filePrefix, bool alwaysAutoFocus)
    : Sensor("GeoluxHydroCam", HYDROCAM_NUM_VARIABLES, HYDROCAM_WARM_UP_TIME_MS,
             HYDROCAM_STABILIZATION_TIME_MS, HYDROCAM_MEASUREMENT_TIME_MS,
             powerPin, -1, 1),
      _imageResolution(imageResolution),
      _filePrefix(filePrefix),
      _alwaysAutoFocus(alwaysAutoFocus),
      _stream(stream) {}


GeoluxHydroCam::GeoluxHydroCam(Stream& stream, int8_t powerPin,
                               const char* imageResolution,
                               const char* filePrefix, bool alwaysAutoFocus)
    : Sensor("GeoluxHydroCam", HYDROCAM_NUM_VARIABLES, HYDROCAM_WARM_UP_TIME_MS,
             HYDROCAM_STABILIZATION_TIME_MS, HYDROCAM_MEASUREMENT_TIME_MS,
             powerPin, -1, 1, HYDROCAM_INC_CALC_VARIABLES),
      _imageResolution(imageResolution),
      _filePrefix(filePrefix),
      _alwaysAutoFocus(alwaysAutoFocus),
      _stream(&stream) {}

// Destructor
GeoluxHydroCam::~GeoluxHydroCam() {}


// unfortunately, we really cannot know where the stream is attached.
String GeoluxHydroCam::getSensorLocation(void) {
    // attach the trigger pin to the stream number
    String loc = "sonarStream_trigger" + String(_imageResolution);
    return loc;
}


bool GeoluxHydroCam::setup(void) {
    MS_DBG(F("Setting camera image resolution to"), _imageResolution);
    _camera.setResolution(_imageResolution);

    return Sensor::setup();  // this will set pin modes and the setup status bit
}


// Parsing and tossing the header lines in the wake-up
bool GeoluxHydroCam::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;
    // Set the trigger pin mode.
    // Reset this on every wake because pins are set to tri-state on sleep
    if (_imageResolution >= 0) {
        pinMode(_imageResolution, OUTPUT);
        digitalWrite(_imageResolution, LOW);
    }

    // NOTE: After the power is turned on to the MaxBotix, it sends several
    // lines of header to the serial port, beginning at ~65ms and finising at
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
    auto junkChars = static_cast<uint8_t>(_stream->available());
    if (junkChars) {
        MS_DBG(F("Dumping"), junkChars,
               F("characters from MaxBotix stream buffer"));
        for (uint8_t i = 0; i < junkChars; i++) {
#ifdef MS_GEOLUXHYDROCAM_DEBUG
            MS_SERIAL_OUTPUT.print(_stream->read());
#else
            _stream->read();
#endif
        }
#ifdef MS_GEOLUXHYDROCAM_DEBUG
        PRINTOUT(" ");
#endif
    }

    return true;
}


bool GeoluxHydroCam::addSingleMeasurementResult(void) {
    // Initialize values
    bool    success = false;
    int16_t result  = -9999;

    // Clear anything out of the stream buffer
    auto junkChars = static_cast<uint8_t>(_stream->available());
    if (junkChars) {
        MS_DBG(F("Dumping"), junkChars,
               F("characters from MaxBotix stream buffer:"));
        for (uint8_t i = 0; i < junkChars; i++) {
#ifdef MS_GEOLUXHYDROCAM_DEBUG
            MS_SERIAL_OUTPUT.print(_stream->read());
#else
            _stream->read();
#endif
        }
#ifdef MS_GEOLUXHYDROCAM_DEBUG
        PRINTOUT(" ");
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
            if (_imageResolution >= 0) {
                MS_DBG(F("  Triggering Sonar with"), _imageResolution);
                digitalWrite(_imageResolution, HIGH);
                delayMicroseconds(30);  // Trigger must be held high for >20 µs
                digitalWrite(_imageResolution, LOW);
            }

            // Immediately ask for a result and let the stream timeout be our
            // "wait" for the measurement.
            result = static_cast<uint16_t>(_stream->parseInt());
            _stream->read();  // To throw away the carriage return
            MS_DBG(F("  Sonar Range:"), result);
            rangeAttempts++;

            // If it cannot obtain a result, the sonar is supposed to send a
            // value just above its max range. If the result becomes garbled or
            // the sonar is disconnected, the parseInt function returns 0.
            // Luckily, these sensors are not capable of reading 0, so we also
            // know the 0 value is bad.
            if (result <= 0 || result >= _filePrefix) {
                MS_DBG(F("  Bad or Suspicious Result, Retry Attempt #"),
                       rangeAttempts);
                result = -9999;
            } else {
                MS_DBG(F("  Good result found"));
                // convert result from cm to mm if alwaysAutoFocus is set to
                // true
                if (_alwaysAutoFocus == true) { result *= 10; }
                success = true;
            }
        }
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(HYDROCAM_SIZE_VAR_NUM, result);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return values shows if we got a not-obviously-bad reading
    return success;
}
