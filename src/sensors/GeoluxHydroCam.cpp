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
                               Logger& baseLogger, int8_t powerPin2,
                               const char* imageResolution,
                               const char* filePrefix, bool alwaysAutoFocus)
    : Sensor("GeoluxHydroCam", HYDROCAM_NUM_VARIABLES, HYDROCAM_WARM_UP_TIME_MS,
             HYDROCAM_STABILIZATION_TIME_MS, HYDROCAM_MEASUREMENT_TIME_MS,
             powerPin, -1, 1, HYDROCAM_INC_CALC_VARIABLES),
      _powerPin2(powerPin2),
      _imageResolution(imageResolution),
      _filePrefix(filePrefix),
      _alwaysAutoFocus(alwaysAutoFocus),
      _baseLogger(&baseLogger),
      _stream(stream),
      _camera(stream) {}


GeoluxHydroCam::GeoluxHydroCam(Stream& stream, int8_t powerPin,
                               Logger& baseLogger, int8_t powerPin2,
                               const char* imageResolution,
                               const char* filePrefix, bool alwaysAutoFocus)
    : Sensor("GeoluxHydroCam", HYDROCAM_NUM_VARIABLES, HYDROCAM_WARM_UP_TIME_MS,
             HYDROCAM_STABILIZATION_TIME_MS, HYDROCAM_MEASUREMENT_TIME_MS,
             powerPin, -1, 1, HYDROCAM_INC_CALC_VARIABLES),
      _powerPin2(powerPin2),
      _imageResolution(imageResolution),
      _filePrefix(filePrefix),
      _alwaysAutoFocus(alwaysAutoFocus),
      _baseLogger(&baseLogger),
      _stream(&stream),
      _camera(&stream) {}

// Destructor
GeoluxHydroCam::~GeoluxHydroCam() {}

String GeoluxHydroCam::getLastSavedImageName() {
    return _filename;
}


String GeoluxHydroCam::getSensorLocation(void) {
    return F("cameraSerial");
}


bool GeoluxHydroCam::setup(void) {
    bool success =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    // We want to turn on all possible measurement parameters
    delay(10);
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }

    while (!isWarmedUp()) {
        // wait
    };

    MS_DBG(F("Setting camera image resolution to"), _imageResolution);
    success &= _camera.setResolution(_imageResolution);
    _camera.waitForReady(50L, 15000L);

#if defined(MS_GEOLUXHYDROCAM_DEBUG) && defined(MS_OUTPUT) && \
    !defined(MS_SILENT)
    MS_DBG(F("Printing all camera info"));
    _camera.printCameraInfo(MS_OUTPUT);

#if defined(MS_2ND_OUTPUT)
    _camera.printCameraInfo(MS_2ND_OUTPUT);
#endif

    MS_DBG(F("Camera is serial number:"), _camera.getCameraSerialNumber());
    MS_DBG(F("Current camera firmware is:"), _camera.getCameraFirmware());
    MS_DBG(F("Current image resolution is:"), _camera.getResolution());
    MS_DBG(F("Current jpg compression quality is:"), _camera.getQuality());
    MS_DBG(F("Current maximum jpg size is:"), _camera.getJPEGMaximumSize());
#endif

    if (!success) {
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
        // UN-set the set-up bit (bit 0) since setup failed!
        _sensorStatus &= 0b11111110;
    }

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    return success;
}


// Parsing and tossing the header lines in the wake-up
bool GeoluxHydroCam::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;

    if (_alwaysAutoFocus) { return _camera.runAutofocus() == GeoluxCamera::OK; }

    return true;
}

// The function to put the sensor to sleep
// Different from the standard in that empties and flushes the stream.
bool GeoluxHydroCam::sleep(void) {
    // empty then flush the buffer
    while (_stream->available()) { _stream->read(); }
    _stream->flush();
    return Sensor::sleep();
};

bool GeoluxHydroCam::startSingleMeasurement(void) {
    // Sensor::startSingleMeasurement() checks that if it's awake/active and
    // sets the timestamp and status bits.  If it returns false, there's no
    // reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    bool success = true;
    MS_DBG(F("Requesting that the camera take a picture ... "));
    if (_camera.takeSnapshot() == GeoluxCamera::OK) {
        MS_DBG(F("picture started successfully!"));
    } else {
        MS_DBG(F("Snapshot failed!"));
        success = false;
    }

    if (success) {
        // Update the time that a measurement was requested
        _millisMeasurementRequested = millis();
    } else {
        // Otherwise, make sure that the measurement start time and success bit
        // (bit 6) are unset
        MS_DBG(getSensorNameAndLocation(),
               F("did not successfully start a measurement."));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
    }

    return true;
}


bool GeoluxHydroCam::addSingleMeasurementResult(void) {
    // Initialize values
    bool    success           = false;
    int32_t bytes_transferred = -9999;
    int32_t byte_error        = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        // set a new filename based on the current RTC time
        String filename = _baseLogger->generateFileName(
            true, HYDROCAM_FILE_EXTENSION, _filePrefix);
        MS_DBG(F("Attempting to create the file: "), filename);

        // Initialise the SD card
        // skip everything else if there's no SD card, otherwise it might hang
        if (!_baseLogger->initializeSDCard()) return false;

        // Create and then open the file in write mode
        if (imgFile.open(filename.c_str(), O_CREAT | O_WRITE | O_AT_END)) {
            MS_DBG(F("Created new file:"), filename);
            success = true;
        } else {
            MS_DBG(F("Failed to create the image file"), filename);
            success = false;
        }

        int32_t image_size = _camera.getImageSize();
        MS_DBG(F("Completed image is"), image_size, F("bytes."));
        success &= image_size != 0;

        if (success) {
            // dump anything in the camera stream, just in case
            _camera.streamDump();

            // Disable the watch-dog timer to reduce interrupts during transfer
            MS_DBG(F("Disabling the watchdog during file transfer"));
            extendedWatchDog::disableWatchDog();

            // transfer the image from the camera to a file on the SD card
            MS_START_DEBUG_TIMER;
            bytes_transferred = _camera.transferImage(imgFile, image_size);
            byte_error        = abs(bytes_transferred - image_size);
            // Close the image file
            imgFile.close();

            // See how long it took us
            MS_DBG(F("Wrote"), bytes_transferred, F("of expected"), image_size,
                   F("bytes to the SD card - a difference of"), byte_error,
                   F("bytes"));
            MS_DBG(F("Total read/write time was"), MS_PRINT_DEBUG_TIMER,
                   F("ms"));

            // Re-enable the watchdog
            MS_DBG(F("Re-enabling the watchdog after file transfer"));
            extendedWatchDog::enableWatchDog();

            // Store the last image name
            _filename = filename;

            success = bytes_transferred == image_size;
            MS_DBG(F("Image transfer was a"),
                   success ? F("success") : F("failure"));
        }
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(HYDROCAM_SIZE_VAR_NUM, bytes_transferred);
    verifyAndAddMeasurementResult(HYDROCAM_ERROR_VAR_NUM, byte_error);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return values shows if we got a not-obviously-bad reading
    return success;
}


// This turns on sensor power
void GeoluxHydroCam::powerUp(void) {
    if (_powerPin >= 0) {
        // Reset power pin mode every power up because pins are set to tri-state
        // on sleep
        pinMode(_powerPin, OUTPUT);
        MS_DBG(F("Powering"), getSensorNameAndLocation(), F("with pin"),
               _powerPin);
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    if (_powerPin2 >= 0) {
        // Reset power pin mode every power up because pins are set to tri-state
        // on sleep
        pinMode(_powerPin2, OUTPUT);
        MS_DBG(F("Applying secondary power to"), getSensorNameAndLocation(),
               F("with pin"), _powerPin2);
        digitalWrite(_powerPin2, HIGH);
    }
    if (_powerPin < 0 && _powerPin2 < 0) {
        MS_DBG(F("Power to"), getSensorNameAndLocation(),
               F("is not controlled by this library."));
    }
    // Set the status bit for sensor power attempt (bit 1) and success (bit 2)
    _sensorStatus |= 0b00000110;
}


// This turns off sensor power
void GeoluxHydroCam::powerDown(void) {
    if (_powerPin >= 0) {
        MS_DBG(F("Turning off power to"), getSensorNameAndLocation(),
               F("with pin"), _powerPin);
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the measurement request time
        _millisMeasurementRequested = 0;
        // Unset the status bits for sensor power (bits 1 & 2),
        // activation (bits 3 & 4), and measurement request (bits 5 & 6)
        _sensorStatus &= 0b10000001;
    }
    if (_powerPin2 >= 0) {
        MS_DBG(F("Turning off secondary power to"), getSensorNameAndLocation(),
               F("with pin"), _powerPin2);
        digitalWrite(_powerPin2, LOW);
    }
    if (_powerPin < 0 && _powerPin2 < 0) {
        MS_DBG(F("Power to"), getSensorNameAndLocation(),
               F("is not controlled by this library."));
        // Do NOT unset any status bits or timestamps if we didn't really power
        // down!
    }
}


// This checks to see if enough time has passed for warm-up
bool GeoluxHydroCam::isWarmedUp(bool debug) {
    // If the sensor doesn't have power, then it will never be warmed up,
    // so the warm up time is essentially already passed.
    if (!bitRead(_sensorStatus, 2)) {
        if (debug) {
            MS_DBG(getSensorNameAndLocation(),
                   F("does not have power and cannot warm up!"));
        }
        return true;
    }

    uint32_t elapsed_since_power_on = millis() - _millisPowerOn;
    // If the sensor has power and enough time has elapsed, it's warmed up
    if (elapsed_since_power_on > HYDROCAM_STABILIZATION_TIME_MAX) {
        MS_DBG(F("It's been"), elapsed_since_power_on, F("ms, and"),
               getSensorNameAndLocation(), F("timed out after power up."));
        return true;  // timeout
    } else if (elapsed_since_power_on > _warmUpTime_ms) {
        if (debug) {
            MS_DBG(F("It's been"), elapsed_since_power_on, F("ms, and"),
                   getSensorNameAndLocation(), F("might be warmed up!"));
        }
        GeoluxCamera::geolux_status camera_status = _camera.getStatus();
        bool is_ready = camera_status == GeoluxCamera::OK ||
            camera_status == GeoluxCamera::NONE;
        if (debug) {
            if (is_ready) {
                MS_DBG(F("It's been"), elapsed_since_power_on, F("ms, and"),
                       getSensorNameAndLocation(), F("says it's ready."));
            } else {
                MS_DBG(F("It's been"), elapsed_since_power_on, F("ms, and"),
                       getSensorNameAndLocation(),
                       F("says it's not ready yet."));
            }
        }
        return is_ready;
    } else {
        // wait at least the minimum warm-up time
        return false;
    }
}


// This checks to see if enough time has passed for stability
bool GeoluxHydroCam::isStable(bool debug) {
    // If the sensor failed to activate, it will never stabilize, so the
    // stabilization time is essentially already passed
    if (!bitRead(_sensorStatus, 4)) {
        if (debug) {
            MS_DBG(getSensorNameAndLocation(),
                   F("is not active and cannot stabilize!"));
        }
        return true;
    }

    uint32_t elapsed_since_wake_up = millis() - _millisSensorActivated;
    uint32_t minTime               = _stabilizationTime_ms + _alwaysAutoFocus
                      ? HYDROCAM_AUTOFOCUS_TIME_MS
                      : 0L;
    uint32_t maxTime = HYDROCAM_STABILIZATION_TIME_MAX + _alwaysAutoFocus
        ? HYDROCAM_AUTOFOCUS_TIME_MAX
        : 0L;
    // If the sensor has been activated and enough time has elapsed, it's stable
    if (elapsed_since_wake_up > maxTime) {
        MS_DBG(F("It's been"), elapsed_since_wake_up, F("ms, and"),
               getSensorNameAndLocation(),
               F("timed out waiting for \"stabilization\""));
        return true;  // timeout
    } else if (elapsed_since_wake_up > minTime) {
        if (debug) {
            MS_DBG(F("It's been"), elapsed_since_wake_up, F("ms, and"),
                   getSensorNameAndLocation(),
                   F("might be ready to take an image."));
        }
        GeoluxCamera::geolux_status camera_status = _camera.getStatus();
        bool is_ready = camera_status == GeoluxCamera::OK ||
            camera_status == GeoluxCamera::NONE;
        if (debug) {
            if (is_ready) {
                MS_DBG(F("It's been"), elapsed_since_wake_up, F("ms, and"),
                       getSensorNameAndLocation(),
                       F("says it's ready to take an image."));
            } else {
                MS_DBG(F("It's been"), elapsed_since_wake_up, F("ms, and"),
                       getSensorNameAndLocation(),
                       F("says it's not ready to image yet."));
            }
        }
        return is_ready;
    } else {
        // Wait at least the minimum readiness time
        return false;
    }
}


// This checks to see if enough time has passed for measurement completion
bool GeoluxHydroCam::isMeasurementComplete(bool debug) {
    // If a measurement failed to start, the sensor will never return a result,
    // so the measurement time is essentially already passed
    if (!bitRead(_sensorStatus, 6)) {
        if (debug) {
            MS_DBG(getSensorNameAndLocation(),
                   F("is not taking an image and will not return a value!"));
        }
        return true;
    }

    uint32_t elapsed_since_meas_start = millis() - _millisMeasurementRequested;
    if (elapsed_since_meas_start > HYDROCAM_MEASUREMENT_TIME_MAX) {
        MS_DBG(F("It's been"), elapsed_since_meas_start, F("ms, and"),
               getSensorNameAndLocation(),
               F("timed out waiting for image to complete"));
        return true;  // timeout
    } else if (elapsed_since_meas_start > _measurementTime_ms) {
        if (debug) {
            MS_DBG(F("It's been"), elapsed_since_meas_start, F("ms, and"),
                   getSensorNameAndLocation(),
                   F("might have finished an image."));
        }
        GeoluxCamera::geolux_status camera_status = _camera.getStatus();
        bool is_ready = camera_status == GeoluxCamera::OK ||
            camera_status == GeoluxCamera::NONE;
        if (debug) {
            if (is_ready) {
                MS_DBG(F("It's been"), elapsed_since_meas_start, F("ms, and"),
                       getSensorNameAndLocation(),
                       F("says it's finished with an image."));
            } else {
                MS_DBG(F("It's been"), elapsed_since_meas_start, F("ms, and"),
                       getSensorNameAndLocation(),
                       F("says it's not finished yet."));
            }
        }
        return is_ready;
    } else {
        // If an image has started by the minimum imaging time hasn't passed, we
        // need to wait
        return false;
    }
}
