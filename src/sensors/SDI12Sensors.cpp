/**
 * @file SDI12Sensors.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the SDI12Sensors class.
 */

#define LIBCALL_ENABLEINTERRUPT  // To prevent compiler/linker crashes
#include <EnableInterrupt.h>     // To handle external and pin change interrupts

#include "SDI12Sensors.h"


// The constructor - need the number of measurements the sensor will return,
// SDI-12 address, the power pin, and the data pin
SDI12Sensors::SDI12Sensors(char SDI12address, int8_t powerPin, int8_t dataPin,
                           uint8_t       measurementsToAverage,
                           const char*   sensorName,
                           const uint8_t totalReturnedValues,
                           uint32_t      warmUpTime_ms,
                           uint32_t      stabilizationTime_ms,
                           uint32_t measurementTime_ms, int8_t extraWakeTime,
                           uint8_t incCalcValues)
    : Sensor(sensorName, totalReturnedValues, warmUpTime_ms,
             stabilizationTime_ms, measurementTime_ms, powerPin, dataPin,
             measurementsToAverage, incCalcValues),
      _SDI12Internal(dataPin),
      _SDI12address(SDI12address),
      _extraWakeTime(extraWakeTime) {}
SDI12Sensors::SDI12Sensors(char* SDI12address, int8_t powerPin, int8_t dataPin,
                           uint8_t       measurementsToAverage,
                           const char*   sensorName,
                           const uint8_t totalReturnedValues,
                           uint32_t      warmUpTime_ms,
                           uint32_t      stabilizationTime_ms,
                           uint32_t measurementTime_ms, int8_t extraWakeTime,
                           uint8_t incCalcValues)
    : Sensor(sensorName, totalReturnedValues, warmUpTime_ms,
             stabilizationTime_ms, measurementTime_ms, powerPin, dataPin,
             measurementsToAverage, incCalcValues),
      _SDI12Internal(dataPin),
      _SDI12address(*SDI12address),
      _extraWakeTime(extraWakeTime) {}
SDI12Sensors::SDI12Sensors(int SDI12address, int8_t powerPin, int8_t dataPin,
                           uint8_t       measurementsToAverage,
                           const char*   sensorName,
                           const uint8_t totalReturnedValues,
                           uint32_t      warmUpTime_ms,
                           uint32_t      stabilizationTime_ms,
                           uint32_t measurementTime_ms, int8_t extraWakeTime,
                           uint8_t incCalcValues)
    : Sensor(sensorName, totalReturnedValues, warmUpTime_ms,
             stabilizationTime_ms, measurementTime_ms, powerPin, dataPin,
             measurementsToAverage, incCalcValues),
      _SDI12Internal(dataPin),
      _SDI12address(static_cast<char>(SDI12address + '0')),
      _extraWakeTime(extraWakeTime) {}
// Destructor
SDI12Sensors::~SDI12Sensors() {}


bool SDI12Sensors::setup(void) {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    // Begin the SDI-12 interface
    _SDI12Internal.begin();

    // Library default timeout should be 150ms, which is 10 times that specified
    // by the SDI-12 protocol for a sensor response.
    // May want to bump it up even further here.
    _SDI12Internal.setTimeout(150);
    // Force the timeout value to be -9999 (This should be library default.)
    _SDI12Internal.setTimeoutValue(-9999);

#if defined(__AVR__) || defined(ARDUINO_ARCH_AVR)
    // Allow the SDI-12 library access to interrupts
    MS_DBG(F("Enabling interrupts for SDI12 on pin"), _dataPin);
    enableInterrupt(_dataPin, SDI12::handleInterrupt, CHANGE);
#endif

    retVal &= getSensorInfo();

    // Empty the SDI-12 buffer
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    _SDI12Internal.end();

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    if (!retVal) {  // if set-up failed
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
        // UN-set the set-up bit (bit 0) since setup failed!
        _sensorStatus &= 0b11111110;
    }

    return retVal;
}


bool SDI12Sensors::requestSensorAcknowledgement(void) {
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(F("  Asking for sensor acknowlegement"));
    String myCommand = "";
    myCommand += _SDI12address;
    myCommand += "!";  // sends 'acknowledge active' command [address][!]

    bool    didAcknowledge = false;
    uint8_t ntries         = 0;
    while (!didAcknowledge && ntries < 5) {
        _SDI12Internal.sendCommand(myCommand, _extraWakeTime);
        MS_DEEP_DBG(F("    >>>"), myCommand);
        delay(30);

        // wait for acknowlegement with format:
        // [address]<CR><LF>
        String sdiResponse = _SDI12Internal.readStringUntil('\n');
        sdiResponse.trim();
        MS_DEEP_DBG(F("    <<<"), sdiResponse);

        // Empty the buffer again
        _SDI12Internal.clearBuffer();

        if (sdiResponse == String(_SDI12address)) {
            MS_DBG(F("   "), getSensorNameAndLocation(),
                   F("replied as expected."));
            didAcknowledge = true;
        } else if (sdiResponse.startsWith(String(_SDI12address))) {
            MS_DBG(F("   "), getSensorNameAndLocation(),
                   F("replied, unexpectedly"));
            didAcknowledge = true;
        } else {
            MS_DBG(F("   "), getSensorNameAndLocation(), F("did not reply!"));
            didAcknowledge = false;
        }

        ntries++;
    }

    return didAcknowledge;
}


// A helper function to run the "sensor info" SDI12 command
bool SDI12Sensors::getSensorInfo(void) {
    // Check if this the currently active SDI-12 Object
    bool wasActive = _SDI12Internal.isActive();
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set correctly.
    if (!wasActive) _SDI12Internal.begin();
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    // Check that the sensor is there and responding
    if (!requestSensorAcknowledgement()) return false;

    MS_DBG(F("  Getting sensor info"));
    String myCommand = "";
    myCommand += _SDI12address;
    myCommand += "I!";  // sends 'info' command [address][I][!]
    _SDI12Internal.sendCommand(myCommand, _extraWakeTime);
    MS_DEEP_DBG(F("    >>>"), myCommand);
    delay(30);

    // wait for acknowlegement with format:
    // [address][SDI12 version supported (2 char)][vendor (8 char)][model (6
    // char)][version (3 char)][serial number (<14 char)]<CR><LF>
    String sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    MS_DEEP_DBG(F("    <<<"), sdiResponse);

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    if (!wasActive) _SDI12Internal.end();

    if (sdiResponse.length() > 1) {
        String sdi12Address = sdiResponse.substring(0, 1);
        MS_DBG(F("   SDI12 Address:"), sdi12Address);
        float sdi12Version = sdiResponse.substring(1, 3).toFloat();
        sdi12Version /= 10;
        MS_DBG(F("   SDI12 Version:"), sdi12Version);
        _sensorVendor = sdiResponse.substring(3, 11);
        _sensorVendor.trim();
        MS_DBG(F("   Sensor Vendor:"), _sensorVendor);
        _sensorModel = sdiResponse.substring(11, 17);
        _sensorModel.trim();
        MS_DBG(F("   Sensor Model:"), _sensorModel);
        _sensorVersion = sdiResponse.substring(17, 20);
        _sensorVersion.trim();
        MS_DBG(F("   Sensor Version:"), _sensorVersion);
        _sensorSerialNumber = sdiResponse.substring(20);
        _sensorSerialNumber.trim();
        MS_DBG(F("   Sensor Serial Number:"), _sensorSerialNumber);
        // Suppress the DDI serial start-up string on meter sensors.  This
        // shouldn't be sent if the SDI-12 address is non-zero, but we'll
        // explicitly suppress it just in case.
        if (_sensorVendor == "METER" && _SDI12address == 0) {
            MS_DBG(F("  Suppressing DDI string on Meter sensor"));
            myCommand = "";
            myCommand += _SDI12address;
            myCommand += "XO1!";  // sends extended command
                                  // [address][XO][suppressionState][!]
                                  // 0: DDI unsuppressed
                                  // 1: DDI suppressed
            _SDI12Internal.sendCommand(myCommand, _extraWakeTime);
            MS_DEEP_DBG(F("    >>>"), myCommand);
            delay(30);
            String supressionResponse = _SDI12Internal.readStringUntil('\n');
            supressionResponse.trim();
            MS_DEEP_DBG(F("    <<<"), supressionResponse);
        }
        return true;
    } else {
        return false;
    }
}


// The sensor vendor
String SDI12Sensors::getSensorVendor(void) {
    return _sensorVendor;
}

// The sensor model
String SDI12Sensors::getSensorModel(void) {
    return _sensorModel;
}

// The sensor version
String SDI12Sensors::getSensorVersion(void) {
    return _sensorVersion;
}

// The sensor serial number
String SDI12Sensors::getSensorSerialNumber(void) {
    return _sensorSerialNumber;
}


// The sensor installation location on the Mayfly
String SDI12Sensors::getSensorLocation(void) {
    String sensorLocation = F("SDI12-");
    sensorLocation += String(_SDI12address) + F("_Pin") + String(_dataPin);
    return sensorLocation;
}

// Sending the command to start a measurement
int8_t SDI12Sensors::startSDI12Measurement(bool isConcurrent) {
    String startCommand;
    String sdiResponse;

    // Try up to 3 times to start a measurement
    uint8_t numVariables = 0;
    uint8_t ntries       = 0;
    int8_t  wait         = -1;  // NOTE: The wait time can be 0!
    while (numVariables != (_numReturnedValues - _incCalcValues) &&
           ntries < 5) {
        if (isConcurrent) {
            MS_DBG(F("  Beginning concurrent measurement on"),
                   getSensorNameAndLocation());
        } else {
            MS_DBG(F("  Beginning NON-concurrent (standard) measurement on"),
                   getSensorNameAndLocation());
        }
        startCommand = "";
        startCommand += _SDI12address;
        if (isConcurrent) {
            startCommand += "C!";  // Start concurrent measurement - format
                                   // [address]['C'][!]
        } else {
            startCommand += "M!";  // Start standard measurement - format
            // [address]['M'][!]
        }
        _SDI12Internal.clearBuffer();
        _SDI12Internal.sendCommand(startCommand, _extraWakeTime);
        delay(30);  // It just needs this little delay
        MS_DEEP_DBG(F("    >>>"), startCommand);

        // wait for acknowlegement with format
        // [address][ttt (3 char, seconds)][number of values to be returned,
        // 0-9]<CR><LF>
        sdiResponse = _SDI12Internal.readStringUntil('\n');
        sdiResponse.trim();
        _SDI12Internal.clearBuffer();
        MS_DEEP_DBG(F("    <<<"), sdiResponse);

        // find out how long we have to wait (in seconds).
        if (sdiResponse.length() > 3) {
            wait = static_cast<uint8_t>(sdiResponse.substring(1, 4).toInt());
            numVariables =
                static_cast<uint8_t>(sdiResponse.substring(4).toInt());
        }

        // Empty the buffer again
        _SDI12Internal.clearBuffer();
        ntries++;
    }

    // Verify the number of results the sensor will send
    if (numVariables != (_numReturnedValues - _incCalcValues)) {
        PRINTOUT(numVariables, F("results expected"),
                 F("This differs from the sensor's standard design of"),
                 (_numReturnedValues - _incCalcValues), F("measurements!!"));
    }

    // Return how long we're expecting to wait for a measurement
    // NOTE:  The sensor generally returns a value rounded up to the next
    // second.
    return wait;
}


#ifndef MS_SDI12_NON_CONCURRENT
// Sending the command to get a concurrent measurement
bool SDI12Sensors::startSingleMeasurement(void) {
    // Sensor::startSingleMeasurement() checks that if it's awake/active and
    // sets the timestamp and status bits.  If it returns false, there's no
    // reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    // Check if this the currently active SDI-12 Object
    bool wasActive = _SDI12Internal.isActive();
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set correctly.
    if (!wasActive) _SDI12Internal.begin();
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    // Check that the sensor is there and responding
    if (!requestSensorAcknowledgement()) {
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
        return false;
    }

    // send the commands to start the measurement; true = concurrent
    // the returned wait time should always be non-zero
    int8_t wait = startSDI12Measurement(true);

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    if (!wasActive) _SDI12Internal.end();

    // Set the times we've activated the sensor and asked for a measurement
    if (wait >= 0) {
        MS_DBG(F("    Concurrent measurement started."));
        // Update the time that a measurement was requested
        _millisMeasurementRequested = millis();
        // Set the status bit for measurement start success (bit 6)
        _sensorStatus |= 0b01000000;
        return true;
    } else {
        MS_DBG(getSensorNameAndLocation(),
               F("did not respond to measurement request!"));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
        return false;
    }
}
#endif

bool SDI12Sensors::getResults(void) {
    // Check if this the currently active SDI-12 Object
    bool wasActive = _SDI12Internal.isActive();
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set
    // correctly.
    if (!wasActive) _SDI12Internal.begin();
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
    uint8_t resultsReceived = 0;
    uint8_t cmd_number      = 0;

    // When requesting data, the sensor sends back up to ~80 characters at a
    // time to each data request.  If it needs to return more results than can
    // fit in the first data request (D0), we need to make additional requests
    // (D1-9).  Since this is a parent to all sensors, we're going to keep
    // requesting data until we either get as many results as we expect or no
    // more data is returned.
    while (resultsReceived < (_numReturnedValues - _incCalcValues) &&
           cmd_number <= 9) {
        bool gotResults = false;
        // Assemble the command based on how many commands we've already sent,
        // starting with D0 and ending with D9
        // SDI-12 command to get data [address][D][dataOption][!]
        String getDataCommand = "";
        getDataCommand += _SDI12address;
        getDataCommand += "D";
        getDataCommand += cmd_number;
        getDataCommand += "!";
        _SDI12Internal.sendCommand(getDataCommand, _extraWakeTime);
        delay(30);  // It just needs this little delay
        MS_DEEP_DBG(F("    >>>"), getDataCommand);

        // Wait for the first few charaters to arrive.  The response from a data
        // request should always have more than three characters
        uint32_t start = millis();
        while (_SDI12Internal.available() < 3 && (millis() - start) < 1500) {
            // wait
        }
        // read the returned address to remove it from the buffer
        auto returnedAddress = static_cast<char>(_SDI12Internal.read());
        // print out a warning if the address doesn't match up
        if (returnedAddress != _SDI12address) {
            MS_DBG(F("Warning, expecting data from"), _SDI12address,
                   F("but got data from"), returnedAddress);
        }
        // Start printing out the returned data
        MS_DEEP_DBG(F("    <<<"), returnedAddress);

        // While there is any data left in the buffer
        while (_SDI12Internal.available() && (millis() - start) < 3000) {
            // First peek to see if the next character in the buffer is a number
            int c = _SDI12Internal.peek();
            // if there's a number, a decimal, or a negative sign next in the
            // buffer, start reading it as a float.
            if (c == '-' || (c >= '0' && c <= '9') || c == '.') {
                // Read the float without skipping any in-valid characters.
                // We don't want to skip anything because we want to be able to
                // debug and see exactly which characters the sensor sent over
                // if they weren't numbers.
                // Reading the numbers as a float will remove them from the
                // buffer.
                float result = _SDI12Internal.parseFloat(SKIP_NONE);
                // The SDI-12 library should return -9999 on timeout
                if (result == -9999 || isnan(result)) result = -9999;
                // Print out what we got
                MS_DBG(F("    <<<"), String(result, 10));
                // Verify that the number is valid and add it to the result
                // array. After each result is read, tick up the number of
                // results received so that the next one goes in the next spot
                // in the variable array.
                verifyAndAddMeasurementResult(resultsReceived, result);
                if (result != -9999) {
                    gotResults = true;
                    resultsReceived++;
                }
                // if the next spot in the buffer isn't a number, we don't want
                // to try and parse it, but we do want to print it out to the
                // debugging port
            } else {
// if we're debugging print out the non-numeric character
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
                MS_DEEP_DBG(F("    <<<"),
                            static_cast<char>(_SDI12Internal.read()));
#else
                // if we're not debugging, just read the character to make sure
                // it's removed from the buffer
                _SDI12Internal.read();
#endif
            }
            delay(10);  // 1 character ~ 7.5ms
        }
        if (!gotResults) {
            MS_DBG(F("  No results received, will not continue requests!"));
            break;  // don't do another loop if we got nothing
        }
        MS_DBG(F("  Total Results Received: "), resultsReceived,
               F(", Remaining: "),
               (_numReturnedValues - _incCalcValues) - resultsReceived);
        cmd_number++;
    }

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    if (!wasActive) _SDI12Internal.end();

    return (_numReturnedValues - _incCalcValues) == resultsReceived;
}


#ifndef MS_SDI12_NON_CONCURRENT
bool SDI12Sensors::addSingleMeasurementResult(void) {
    bool success = false;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        success = getResults();
    } else {
        // If there's no measurement, need to make sure we send over all
        // of the "failed" result values
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
        for (uint8_t i = 0; i < _numReturnedValues; i++) {
            verifyAndAddMeasurementResult(i, static_cast<float>(-9999));
        }
    }

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}
#else
bool SDI12Sensors::addSingleMeasurementResult(void) {
    bool success = false;

    String startCommand;
    String sdiResponse;

    // Check if this the currently active SDI-12 Object
    bool wasActive = _SDI12Internal.isActive();
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set
    // correctly.
    if (!wasActive) _SDI12Internal.begin();
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    // Check that the sensor is there and responding
    if (requestSensorAcknowledgement()) {
        // send the commands to start the measurement; false = not concurrent
        // the returned wait time should always be non-zero
        int8_t wait = startSDI12Measurement(false);

        // Set the times we've activated the sensor and asked for a measurement
        if (wait >= 0) {
            MS_DBG(F("    NON-concurrent measurement started."));
            // Update the time that a measurement was requested
            _millisMeasurementRequested = millis();
            // Set the status bit for measurement start success (bit 6)
            _sensorStatus |= 0b01000000;

            // Since this is not a concurrent measurement, we must sit around
            // and wait for the sensor to issue a service request telling us
            // that the measurement is ready.

            uint32_t timerStart = millis();
            while ((millis() - timerStart) < (1000 * (wait))) {
                // sensor can interrupt us to let us know it is done early
                if (_SDI12Internal.available()) {
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
                    // if we're debugging print out early response
                    MS_DEEP_DBG(F("    <<<"),
                                _SDI12Internal.readStringUntil('\n'));
                    _SDI12Internal.clearBuffer();
                    break;
#else
                    // if we're not debugging, just read the response to make
                    // sure it's removed from the buffer
                    _SDI12Internal.readStringUntil('\n');
                    _SDI12Internal.clearBuffer();
                    break;
#endif
                }
            }
            // Wait for anything else and clear it out
            delay(30);
            _SDI12Internal.clearBuffer();

            // get the results
            success = getResults();
        } else {
            // If there's no measurement, need to make sure we send over all
            // of the "failed" result values
            MS_DBG(getSensorNameAndLocation(),
                   F("is not currently measuring!"));
            for (uint8_t i = 0; i < _numReturnedValues; i++) {
                verifyAndAddMeasurementResult(i, static_cast<float>(-9999));
            }
        }
    } else {
        // If there's no response, we still need to send over all the failed
        // values
        for (uint8_t i = 0; i < _numReturnedValues; i++) {
            verifyAndAddMeasurementResult(i, static_cast<float>(-9999));
        }
    }

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    if (!wasActive) _SDI12Internal.end();

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}
#endif  // #ifndef MS_SDI12_NON_CONCURRENT
