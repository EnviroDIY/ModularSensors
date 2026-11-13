/**
 * @file SDI12Sensors.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the SDI12Sensors class.
 */

#include "SDI12Sensors.h"

/**
 * @def MS_SDI12_USE_CRC
 * @brief Whether or not to use CRC checking on the SDI-12 data.
 * @note If this is defined, the SDI-12 library will check the CRC for all
 * sensors!  This cannot be set on a per-sensor basis.
 */
#ifdef MS_SDI12_NO_CRC_CHECK
#define MS_SDI12_USE_CRC false
#else
#define MS_SDI12_USE_CRC true
#endif


// The constructor - need the number of measurements the sensor will return,
// SDI-12 address, the power pin, and the data pin
SDI12Sensors::SDI12Sensors(char SDI12address, int8_t powerPin, int8_t dataPin,
                           uint8_t       measurementsToAverage,
                           const char*   sensorName,
                           const uint8_t totalReturnedValues,
                           uint32_t      warmUpTime_ms,
                           uint32_t      stabilizationTime_ms,
                           uint32_t measurementTime_ms, uint8_t incCalcValues)
    : Sensor(sensorName, totalReturnedValues, warmUpTime_ms,
             stabilizationTime_ms, measurementTime_ms, powerPin, dataPin,
             measurementsToAverage, incCalcValues),
      _SDI12Internal(dataPin),
      _SDI12Recorder(_SDI12Internal),
      _SDI12address(SDI12address) {}
// Delegating constructor
SDI12Sensors::SDI12Sensors(char* SDI12address, int8_t powerPin, int8_t dataPin,
                           uint8_t       measurementsToAverage,
                           const char*   sensorName,
                           const uint8_t totalReturnedValues,
                           uint32_t      warmUpTime_ms,
                           uint32_t      stabilizationTime_ms,
                           uint32_t measurementTime_ms, uint8_t incCalcValues)
    : SDI12Sensors(*SDI12address, powerPin, dataPin, measurementsToAverage,
                   sensorName, totalReturnedValues, warmUpTime_ms,
                   stabilizationTime_ms, measurementTime_ms, incCalcValues) {}
// Delegating constructor
SDI12Sensors::SDI12Sensors(int SDI12address, int8_t powerPin, int8_t dataPin,
                           uint8_t       measurementsToAverage,
                           const char*   sensorName,
                           const uint8_t totalReturnedValues,
                           uint32_t      warmUpTime_ms,
                           uint32_t      stabilizationTime_ms,
                           uint32_t measurementTime_ms, uint8_t incCalcValues)
    : SDI12Sensors(static_cast<char>(SDI12address + '0'), powerPin, dataPin,
                   measurementsToAverage, sensorName, totalReturnedValues,
                   warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
                   incCalcValues) {}


bool SDI12Sensors::setup() {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    delay(10);
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();
    activate();

    // Set some SDI-12 object parameters
    // NOTE: These are **not** set/or unset by the begin/end functions, so
    // calling them once here in setup is fine.

    // Library default timeout should be 150ms, which is 10 times that specified
    // by the SDI-12 protocol for a sensor response.
    // May want to bump it up even further here.
    _SDI12Internal.setTimeout(150);
    // Force the timeout value to be MS_INVALID_VALUE
    _SDI12Internal.setTimeoutValue(MS_INVALID_VALUE);

    retVal &= getSensorInfo();

    deactivate();

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    if (!retVal) {  // if set-up failed
        // Set the status error bit (bit 7)
        setStatusBit(ERROR_OCCURRED);
        // UN-set the set-up bit (bit 0) since setup failed!
        clearStatusBit(SETUP_SUCCESSFUL);
    }

    return retVal;
}


void SDI12Sensors::activate() {
    // Begin the SDI-12 interface
    _SDI12Internal.begin();

    // Empty the buffer
    _SDI12Internal.clearBuffer();
}

void SDI12Sensors::deactivate() {
    // Empty the SDI-12 buffer
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    _SDI12Internal.end();
}


bool SDI12Sensors::requestSensorAcknowledgement() {
    MS_DBG(F("  Asking for sensor acknowledgement"));
    MS_DEEP_DBG(F("    >>>"), _SDI12address, '!');

    bool didAcknowledge = _SDI12Recorder.requestAcknowledgement(_SDI12address);
    if (didAcknowledge) {
        MS_DEEP_DBG(F("    <<<"), _SDI12Recorder.acknowledgedAddress());
        MS_DBG(F("   "), getSensorNameAndLocation(), F("replied as expected."));
    } else {
        MS_DBG(F("   "), getSensorNameAndLocation(), F("did not reply!"));
    }

    return didAcknowledge;
}


// A helper function to run the "sensor info" SDI12 command
bool SDI12Sensors::getSensorInfo() {
    activate();

    // Check that the sensor is there and responding
    if (!requestSensorAcknowledgement()) {
        deactivate();
        return false;
    }

    MS_DBG(F("  Getting sensor info"));
    MS_DEEP_DBG(F("    >>>"), _SDI12address, F("I!"));
    SDI12Recorder::Identification sensorInfo;
    bool didIdentify = _SDI12Recorder.identify(_SDI12address, sensorInfo);

    if (didIdentify) {
        MS_DBG(F("   SDI12 Address:"), sensorInfo.address);
        float sdi12Version = String(sensorInfo.sdi12Version).toFloat();
        sdi12Version /= 10;
        MS_DBG(F("   SDI12 Version:"), sdi12Version);
        _sensorVendor = sensorInfo.vendorID;
        _sensorVendor.trim();
        MS_DBG(F("   Sensor Vendor:"), _sensorVendor);
        _sensorModel = sensorInfo.sensorModel;
        _sensorModel.trim();
        MS_DBG(F("   Sensor Model:"), _sensorModel);
        _sensorVersion = sensorInfo.sensorVersion;
        _sensorVersion.trim();
        MS_DBG(F("   Sensor Version:"), _sensorVersion);
        _sensorSerialNumber = sensorInfo.sensorInfo;
        _sensorSerialNumber.trim();
        MS_DBG(F("   Sensor Serial Number:"), _sensorSerialNumber);
        // Suppress the DDI serial start-up string on meter sensors.  This
        // shouldn't be sent if the SDI-12 address is non-zero, but we'll
        // explicitly suppress it just in case.
        if (_sensorVendor == "METER" && _SDI12address == '0') {
            MS_DBG(F("  Suppressing DDI string on Meter sensor"));
            // Sends the extended command [address][XO][suppressionState][!]
            // 0: DDI unsuppressed
            // 1: DDI suppressed
            MS_DEEP_DBG(F("    >>>"), _SDI12address, F("XO1!"));
            _SDI12Recorder.startRawCommand(_SDI12address, F("XO1"));
            while (_SDI12Recorder.isBusy()) {
                _SDI12Recorder.update();
                yield();
            }
            MS_DEEP_DBG(F("    <<<"), _SDI12address, _SDI12Recorder.response());
        }
        deactivate();
        return true;
    } else {
        deactivate();
        return false;
    }
}


// The sensor vendor
String SDI12Sensors::getSensorVendor() {
    return _sensorVendor;
}

// The sensor model
String SDI12Sensors::getSensorModel() {
    return _sensorModel;
}

// The sensor version
String SDI12Sensors::getSensorVersion() {
    return _sensorVersion;
}

// The sensor serial number
String SDI12Sensors::getSensorSerialNumber() {
    return _sensorSerialNumber;
}


// The sensor installation location on the Mayfly
String SDI12Sensors::getSensorLocation() {
    String sensorLocation;
    sensorLocation.reserve(
        20);  // Reserve for "SDI12-" + address + "_Pin" + pin
    sensorLocation = F("SDI12-");
    sensorLocation += String(_SDI12address) + F("_Pin") + String(_dataPin);
    return sensorLocation;
}

// Sending the command to start a measurement
int16_t SDI12Sensors::startSDI12Measurement(bool isConcurrent) {
    if (isConcurrent) {
        MS_DBG(F("  Beginning concurrent measurement on"),
               getSensorNameAndLocation());
        _SDI12Recorder.startConcurrentMeasurement(_SDI12address, 0U,
                                                  MS_SDI12_USE_CRC);
    } else {
        MS_DBG(F("  Beginning NON-concurrent (standard) measurement on"),
               getSensorNameAndLocation());
        _SDI12Recorder.startMeasurement(_SDI12address, 0U, MS_SDI12_USE_CRC);
    }

    while (_SDI12Recorder.isBusy()) {
        _SDI12Recorder.update();
        yield();
    }

    if (_SDI12Recorder.result() != SDI12Recorder::RESULT_SUCCESS) {
        MS_DBG(F("   SDI-12 measurement request failed with recorder result"),
               static_cast<uint8_t>(_SDI12Recorder.result()));
        return -1;
    }

    uint16_t wait         = _SDI12Recorder.measurementSeconds();
    uint8_t  numVariables = _SDI12Recorder.expectedValueCount();
    _millisMeasurementRequested =
        _SDI12Recorder.measurementAcknowledgementMillis();
    MS_DEEP_DBG(F("   Responding address:"), _SDI12address, F("wait time:"),
                wait, F("result count:"), numVariables);

    // Print a warning if the wait is going to be longer than we expect
    if (wait > ceil(_measurementTime_ms / 1000)) {
        MS_DBG(F("   Wait time is too long"), wait * 1000, F("instead of"),
               _measurementTime_ms);
    }
    // Print a warning if the number of returned results is wrong
    uint8_t expectedResults = _numReturnedValues - _incCalcValues;
    if (numVariables != expectedResults) {
        MS_DBG(F("   Wrong number of results expected"), numVariables,
               F("instead of"), expectedResults);
    }

    // Return how long we're expecting to wait for a measurement
    // NOTE:  The sensor generally returns a value rounded up to the next
    // second.
    return static_cast<int16_t>(wait);
}


#ifndef MS_SDI12_NON_CONCURRENT
// Sending the command to get a concurrent measurement
bool SDI12Sensors::startSingleMeasurement() {
    // Sensor::startSingleMeasurement() checks that if it's awake/active and
    // sets the timestamp and status bits.  If it returns false, there's no
    // reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    // activate the SDI-12 object
    activate();

    // Check that the sensor is there and responding
    if (!requestSensorAcknowledgement()) {
        setStatusBit(ERROR_OCCURRED);
        _millisMeasurementRequested = 0;
        clearStatusBit(MEASUREMENT_SUCCESSFUL);
        deactivate();
        return false;
    }

    // send the commands to start the measurement; true = concurrent
    // the returned wait time should always be non-zero
    int16_t wait = startSDI12Measurement(true);

    // Empty the buffer and de-activate the SDI-12 Object
    deactivate();

    // Set the times we've activated the sensor and asked for a measurement
    if (wait >= 0) {
        MS_DBG(F("    Concurrent measurement started."));
        // Set the status bit for measurement start success (bit 6)
        setStatusBit(MEASUREMENT_SUCCESSFUL);
        return true;
    } else {
        // Set the status error bit (bit 7)
        setStatusBit(ERROR_OCCURRED);
        MS_DBG(getSensorNameAndLocation(),
               F("did not respond to measurement request!"));
        _millisMeasurementRequested = 0;
        clearStatusBit(MEASUREMENT_SUCCESSFUL);
        return false;
    }
}
#endif

bool SDI12Sensors::getResults(bool verify_crc) {
    // activate the SDI-12 object
    activate();

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
    // CRC validation is retained with the recorder's measurement context.
    // Keep the parameter for compatibility with existing derived classes.
    (void)verify_crc;

    uint8_t expectedResults = _numReturnedValues - _incCalcValues;
    float   results[MAX_NUMBER_VARS];
    _SDI12Recorder.requestData(_SDI12address, results, expectedResults);
    while (_SDI12Recorder.isBusy()) {
        _SDI12Recorder.update();
        yield();
    }

    bool success = _SDI12Recorder.result() == SDI12Recorder::RESULT_SUCCESS &&
        _SDI12Recorder.dataValueCount() == expectedResults;
    if (success) {
        for (uint8_t resultNumber = 0; resultNumber < expectedResults;
             resultNumber++) {
            MS_DEEP_DBG(F("Moving result #"), resultNumber, '(',
                        results[resultNumber],
                        F(") to the sensor value array"));
            verifyAndAddMeasurementResult(resultNumber, results[resultNumber]);
        }
    } else {
        MS_DBG(F("   SDI-12 data request failed with recorder result"),
               static_cast<uint8_t>(_SDI12Recorder.result()));
    }

    MS_DBG(F("  Total Results Received:"), _SDI12Recorder.dataValueCount(),
           F("Expected:"), expectedResults);

    // Empty the buffer and de-activate the SDI-12 Object
    deactivate();

    return success;
}


#ifndef MS_SDI12_NON_CONCURRENT
// This function is using concurrent measurements, so the MEASUREMENT_SUCCESSFUL
// bit was set in the specialized startSingleMeasurement function based on
// whether the response to the SDI-12 start measurement command.
bool SDI12Sensors::addSingleMeasurementResult() {
    // Perform common initialization checks
    if (!initializeMeasurementResult()) { return false; }

    bool success = getResults(MS_SDI12_USE_CRC);

    // Return success value when finished
    return finalizeMeasurementAttempt(success);
}
#else   // concurrent measurement disabled
// This is for non-concurrent measurements, so this function must both start the
// measurement and get results at once.  For non-concurrent measurements, the
// MEASUREMENT_SUCCESSFUL bit is set in the generic sensor
// startSingleMeasurement function from sensor base, which only verifies that
// the sensor is awake and capable of starting measurements.
bool SDI12Sensors::addSingleMeasurementResult() {
    // Perform common initialization checks
    if (!initializeMeasurementResult()) { return false; }

    bool success = false;

    // activate the SDI-12 object
    activate();

    // Check that the sensor is there and responding
    if (requestSensorAcknowledgement()) {
        // send the commands to start the measurement; false = not concurrent
        // the returned wait time should always be non-zero
        int16_t wait = startSDI12Measurement(false);

        // Set the times we've activated the sensor and asked for a measurement
        if (wait >= 0) {
            MS_DBG(F("    NON-concurrent measurement started."));
            // Re-set the status bit for measurement start success (bit 6)
            setStatusBit(MEASUREMENT_SUCCESSFUL);

            // get the results
            success = getResults(MS_SDI12_USE_CRC);
        } else {
            // If there's no measurement, need to make sure we send over all
            // of the "failed" result values
            MS_DBG(getSensorNameAndLocation(),
                   F("is not currently measuring!"));
            for (uint8_t i = 0; i < _numReturnedValues; i++) {
                verifyAndAddMeasurementResult(
                    i, static_cast<float>(MS_INVALID_VALUE));
            }
        }
    }

    // Empty the buffer and de-activate the SDI-12 Object
    deactivate();

    // Return success value when finished
    return finalizeMeasurementAttempt(success);
}
#endif  // #ifndef MS_SDI12_NON_CONCURRENT
