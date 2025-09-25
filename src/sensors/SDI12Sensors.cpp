/**
 * @file SDI12Sensors.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the SDI12Sensors class.
 */

/**
 * @brief To prevent compiler/linker crashes with enable interrupt library, we
 * must define LIBCALL_ENABLEINTERRUPT before importing EnableInterrupt within a
 * library.
 */
#define LIBCALL_ENABLEINTERRUPT
// To handle external and pin change interrupts
#include "ModSensorInterrupts.h"

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
    // Force the timeout value to be -9999 (This should be library default.)
    _SDI12Internal.setTimeoutValue(-9999);

#if defined(__AVR__) || defined(ARDUINO_ARCH_AVR)
    // Allow the SDI-12 library access to interrupts
    MS_DBG(F("Enabling interrupts for SDI12 on pin"), _dataPin);
    enableInterrupt(_dataPin, SDI12::handleInterrupt, CHANGE);
#endif

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


void SDI12Sensors::activate(void) {
    // Begin the SDI-12 interface
    _SDI12Internal.begin();

    // Empty the buffer
    _SDI12Internal.clearBuffer();
}

void SDI12Sensors::deactivate(void) {
    // Empty the SDI-12 buffer
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    _SDI12Internal.end();
}


bool SDI12Sensors::requestSensorAcknowledgement(void) {
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(F("  Asking for sensor acknowledgement"));
    String myCommand = "";
    myCommand += _SDI12address;
    myCommand += "!";  // sends 'acknowledge active' command [address][!]

    bool    didAcknowledge = false;
    uint8_t ntries         = 0;
    while (!didAcknowledge && ntries < 5) {
        _SDI12Internal.sendCommand(myCommand, _extraWakeTime);
        MS_DEEP_DBG(F("    >>>"), myCommand);
        delay(30);

        // wait for acknowledgement with format:
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
                   F("replied, strangely"));
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
    activate();

    // Check that the sensor is there and responding
    if (!requestSensorAcknowledgement()) {
        deactivate();
        return false;
    }

    MS_DBG(F("  Getting sensor info"));
    String myCommand = "";
    myCommand += _SDI12address;
    myCommand += "I!";  // sends 'info' command [address][I][!]
    _SDI12Internal.sendCommand(myCommand, _extraWakeTime);
    MS_DEEP_DBG(F("    >>>"), myCommand);
    delay(30);

    // wait for acknowledgement with format:
    // [address][SDI12 version supported (2 char)][vendor (8 char)][model (6
    // char)][version (3 char)][serial number (<14 char)]<CR><LF>
    String sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    MS_DEEP_DBG(F("    <<<"), sdiResponse);

    // Empty the buffer and de-activate the SDI-12 Object
    deactivate();

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
            String suppressionResponse = _SDI12Internal.readStringUntil('\n');
            suppressionResponse.trim();
            MS_DEEP_DBG(F("    <<<"), suppressionResponse);
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
    String returnedAddress;

    // Try up to 5 times to start a measurement
    uint8_t numVariables   = 0;
    uint8_t ntries         = 0;
    bool    didAcknowledge = false;
    int8_t  wait           = -1;  // NOTE: The wait time can be 0!
    while (!didAcknowledge && ntries < 5) {
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
            startCommand += "C";  // Start concurrent measurement - 'C'
        } else {
            startCommand += "M";  // Start standard measurement - 'M'
        }
        if (MS_SDI12_USE_CRC) {
            startCommand += "C";  // Add C to request a CRC
        }
        startCommand += "!";  // All commands end with '!'
        _SDI12Internal.clearBuffer();
        _SDI12Internal.sendCommand(startCommand, _extraWakeTime);
        delay(30);  // It just needs this little delay
        MS_DEEP_DBG(F("    >>>"), startCommand);

        // wait for acknowledgement with format
        // [address][ttt (3 char, seconds)][number of values to be returned,
        // 0-9]<CR><LF>
        sdiResponse = _SDI12Internal.readStringUntil('\n');
        sdiResponse.trim();
        _SDI12Internal.clearBuffer();
        MS_DEEP_DBG(F("    <<<"), sdiResponse);

        // find out how long we have to wait (in seconds).
        if (sdiResponse.length() > 3) {
            returnedAddress = sdiResponse.substring(0, 1);
            wait = static_cast<uint8_t>(sdiResponse.substring(1, 4).toInt());
            numVariables =
                static_cast<uint8_t>(sdiResponse.substring(4).toInt());
        }
        MS_DEEP_DBG(F("   Responding address:"), returnedAddress,
                    F("wait time:"), wait, F("result count:"), numVariables);
        // Only require that the responding address be correct to consider the
        // result to have been started
        if (returnedAddress == String(_SDI12address)) {
            didAcknowledge = true;
        } else {
            // print a warning if the responding address is wrong (and try
            // again)
            MS_DBG(F("   Wrong address replied, got"), returnedAddress,
                   F("instead of"), _SDI12address);
        }
        // Print a warning if the wait is going to be longer than we expect
        if (wait > ceil(_measurementTime_ms / 1000)) {
            MS_DBG(F("   Wait time is too long"), wait * 1000, F("instead of"),
                   _measurementTime_ms);
        }
        // Print a warning if the number of returned results is wrong
        if (numVariables != _numReturnedValues) {
            MS_DBG(F("   Wrong number of results expected"), wait * 1000,
                   F("instead of"), (_numReturnedValues - _incCalcValues));
        }

        // Empty the buffer again
        _SDI12Internal.clearBuffer();
        ntries++;
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
    int8_t wait = startSDI12Measurement(true);

    // Empty the buffer and de-activate the SDI-12 Object
    deactivate();

    // Set the times we've activated the sensor and asked for a measurement
    if (wait >= 0) {
        MS_DBG(F("    Concurrent measurement started."));
        // Update the time that a measurement was requested
        _millisMeasurementRequested = millis();
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
    uint8_t resultsReceived = 0;
    uint8_t cmd_number      = 0;
    uint8_t cmd_retries     = 0;

    // the result is structured <addr><values><CR><LF> or
    // <addr><values><CRC><CR><LF>
    // the value portion must be structred as pd.d
    // - p - the polarity sign (+ or -)
    // - d - numeric digits before the decimal place
    // - . - the decimal point (optional)
    // - d - numeric digits after the decimal point

    // From SDI-12 Protocol v1.4, Section 4.4 SDI-12 Commands and Responses:
    // The maximum number of characters that can be returned in the <values>
    // part of the response to a D command is either 35 or 75. If the D command
    // is issued to retrieve data in response to a concurrent measurement
    // command, or in response to a high-volume ASCII measurement command, the
    // maximum is 75. The maximum is also 75 in response to a continuous
    // measurement command. Otherwise, the maximum is 35.
    int max_sdi_response = 76;
    // From SDI-12 Protocol v1.4, Table 11 The send data command (aD0!, aD1! . .
    // . aD9!):
    // - the maximum number of digits for a data value is 7, even without a
    // decimal point
    // - the minimum number of digits for a data value (excluding the decimal
    // point) is 1
    // - the maximum number of characters in a data value is 9 (the (polarity
    // sign + 7 digits + the decimal point))
    // - SRGD Note: The polarity symbol (+ or -) acts as a delimeter between the
    // numeric values
    int max_sdi_digits = 10;

    String compiled_response = "";

    bool success = true;

    // When requesting data, the sensor sends back up to ~80 characters at a
    // time to each data request.  If it needs to return more results than can
    // fit in the first data request (D0), we need to make additional requests
    // (D1-9).  Since this is a parent to all sensors, we're going to keep
    // requesting data until we either get as many results as we expect or no
    // more data is returned.
    while (resultsReceived < (_numReturnedValues - _incCalcValues) &&
           cmd_number <= 9 && cmd_retries < 5) {
        MS_DEEP_DBG(F("Attempt"), cmd_retries, F("to get data number"),
                    cmd_number);
        bool    gotResults  = false;
        uint8_t cmd_results = 0;
        // Assemble the command based on how many commands we've already sent,
        // starting with D0 and ending with D9
        // SDI-12 command to get data [address][D][dataOption][!]
        _SDI12Internal.clearBuffer();
        String getDataCommand = "";
        getDataCommand += _SDI12address;
        getDataCommand += "D";
        getDataCommand += cmd_number;
        getDataCommand += "!";
        _SDI12Internal.sendCommand(getDataCommand, _extraWakeTime);
        delay(30);  // It just needs this little delay
        MS_DEEP_DBG(F("    >>>"), getDataCommand);

        // Wait for the first few characters to arrive.  The response from a
        // data request should always have more than three characters
        // TODO: Is this needed? The readBytesUntil() function uses the timeout
        // for every byte it attempts to read
        uint32_t start = millis();
        while (_SDI12Internal.available() < 3 && (millis() - start) < 1500) {
            // wait
        }

        // create a temporary buffer for incoming values
        char resp_buffer[max_sdi_response] = {'\0'};

        // read bytes into the char array until we get to a new line (\r\n)
        size_t bytes_read = _SDI12Internal.readBytesUntil('\n', resp_buffer,
                                                          max_sdi_response);
        MS_DEEP_DBG(F("Received"), bytes_read, F(" characters"));

        size_t data_bytes_read = bytes_read -
            1;  // subtract one for the /r before the /n
        String sdiResponse = String(resp_buffer);
        compiled_response += sdiResponse;
        sdiResponse.trim();
        MS_DEEP_DBG(F("    <<<"), sdiResponse);

        // read and clear anything else from the buffer
        int extra_chars = 0;
        while (_SDI12Internal.available()) {
#if defined(MS_SDI12SENSORS_DEBUG_DEEP) && !defined(MS_SILENT)
            // read and print
            MS_SERIAL_OUTPUT.write(_SDI12Internal.read());
#else
            // just read
            _SDI12Internal.read();
#endif
            extra_chars++;
        }
        if (extra_chars > 0) {
            MS_DEEP_DBG(extra_chars, F("additional characters received."));
        }
        _SDI12Internal.clearBuffer();

        // check the crc, break if it's incorrect
        if (verify_crc) {
            bool crcMatch = _SDI12Internal.verifyCRC(sdiResponse);
            // subtract the 3 characters of the CRC from the total number of
            // data values
            data_bytes_read = data_bytes_read - 3;
            if (crcMatch) {
                MS_DEEP_DBG(F("CRC valid"));
            } else {
                MS_DBG(F("CRC check failed!"));
                success = false;
                // if we failed CRC in the response, add one to the retry
                // attempts but do not bump up the command number or transfer
                // any results because we want to retry the same data command to
                // try get a valid response
                cmd_retries++;
                // stop processing; no reason to read the numbers when we
                // already know something's wrong
                continue;
            }
        }

        // check the address, break if it's incorrect
        // NOTE: If the address is wrong because the response is garbled, the
        // CRC check above should fail. But we still verify the address in case
        // we're not checking the CRC or we got a well formed response from the
        // wrong sensor.
        char returnedAddress = resp_buffer[0];
        if (returnedAddress != _SDI12address) {
            MS_DBG(F("Wrong address returned!"));
            MS_DBG(F("Expected"), String(_SDI12address), F("Got"),
                   String(returnedAddress));
            MS_DEEP_DBG(String(resp_buffer));
            success = false;
            // if we didn't get the correct address, add one to the retry
            // attempts but do not bump up the command number or transfer any
            // results because we want to retry the same data command to try get
            // a valid response
            cmd_retries++;
            // stop processing; no reason to read the numbers when we already
            // know something's wrong
            continue;
        }

        // create a temporary rx buffer for incoming values
        // this buffer will have a maximum size of the number of measured values
        // of the sensor
        float cmd_rx[_numReturnedValues - _incCalcValues];
        // flag to mark a bad numeric value within the response
        bool bad_read = false;
        // another small buffer to hold the float values for atof
        char float_buffer[max_sdi_digits] = {'\0'};
        // flag to mark if we've gotten a decimal point in the float buffer, so
        // we can check for bad repeated decimals
        bool got_decimal = false;
#if defined(MS_SDI12SENSORS_DEBUG) && !defined(MS_SILENT)
        // mark to keep track of the decimal point in the float buffer, just
        // used for pretty printing
        char* dec_pl = float_buffer;
#endif
        // a pointer to our place in the float buffer - start at start of buffer
        uint8_t fb_pos = 0;
        // iterate through the char array and to check results
        // NOTE: start at 1 since we already looked at the address!
        for (size_t i = 1; i <= data_bytes_read; i++) {
            // Get the character at position
            char c = resp_buffer[i];
            // if we get a polarity sign (+ or -) that is not the first
            // character after the address, or we've reached the end of the
            // buffer, then we're at the end of the previous number and can
            // parse the float buffer
            if ((((c == '-' || c == '+') && i != 1) || i == data_bytes_read) &&
                strnlen(float_buffer, max_sdi_digits) > 0) {
                // Read the float only skipping the '+' if it's included as the
                // polarity.  Reading the numbers as a float will remove them
                // from the buffer.
                float result = atof(float_buffer);
#if defined(MS_SDI12SENSORS_DEBUG) && !defined(MS_SILENT)
                // NOTE: This bit below is just for pretty-printing
                dec_pl              = strchr(float_buffer, '.');
                size_t len_post_dec = 0;
                if (dec_pl != nullptr) {
                    len_post_dec = strnlen(dec_pl, max_sdi_digits) - 1;
                }
                MS_DBG(F("Result"), cmd_results, F("Raw value:"), float_buffer,
                       F("Parsed value:"), String(result, len_post_dec));
#endif
                // The SDI-12 library should return our set timeout value of
                // -9999 on timeout
                if (result == -9999 || isnan(result)) {
                    MS_DBG(F("Result is not valid!"));
                    result = -9999;
                }
                // Put the read value into the temporary buffer. After each
                // result is read, tick up the number of results received so
                // that the next one goes in the next spot in the holding
                // buffer.
                cmd_rx[cmd_results] = result;
                // add how many results we have
                if (result != -9999) {
                    gotResults = true;
                    cmd_results++;
                }
                // empty the float buffer so it's ready for the next number
                float_buffer[0] = '\0';
                fb_pos          = 0;
                got_decimal     = false;
            }
            // if we just read the last byte, continue to the next iteration
            // which will parse the float out of it
            if (i == data_bytes_read) { continue; }
            // if we're mid-number and there's a digit, a decimal, or a negative
            // sign in the sdi12 response buffer, add it to the current float
            // buffer
            if (c == '-' || (c >= '0' && c <= '9') ||
                (c == '.' && !got_decimal)) {
                float_buffer[fb_pos] = c;
                fb_pos++;
                float_buffer[fb_pos] = '\0';  // null terminate the buffer
            } else if (c == '+') {
                // if we get a "+", it's a valid SDI-12 polarity indicator, but
                // not something accepted by atof in parsing the float, so we
                // just ignore it NOTE: A mis-read like this should also cause
                // the CRC to be wrong, but still check here in case we're not
                // using a CRC.
            } else {  //(c != '-' && c != '+' && (c < '0' || c > '9') && c !=
                      //'.')
#if defined(MS_SDI12SENSORS_DEBUG_DEEP) && !defined(MS_SILENT)
                MS_SERIAL_OUTPUT.print("Invalid data response character: ");
                MS_SERIAL_OUTPUT.write(c);
                MS_SERIAL_OUTPUT.println();
#endif
                bad_read = true;
            }
            // if we get a decimal, mark it so we can verify we don't get
            // repeated decimals
            if (c == '.') { got_decimal = true; }
        }

        // if we got results and none of them are bad, transfer from the
        // temporary buffer to the sensor's variable array
        if (gotResults && !bad_read) {
            for (uint8_t cr = 0; cr < cmd_results; cr++) {
                MS_DEEP_DBG(F("Moving result #"), cr, '(', cmd_rx[cr],
                            F(") to result"), resultsReceived,
                            F("of the sensor value array"));
                verifyAndAddMeasurementResult(resultsReceived, cmd_rx[cr]);
                resultsReceived++;
            }
            MS_DBG(F("  Total Results Received:"), resultsReceived,
                   F("Remaining:"),
                   (_numReturnedValues - _incCalcValues) - resultsReceived);
            cmd_number++;
        } else {
            // if we got a bad charater in the response, add one to the retry
            // attempts but do not bump up the command number or transfer any
            // results because we want to retry the same data command to try get
            // a valid response
            MS_DBG(F("No good results!  Will retry!"));
            cmd_retries++;
        }
    }

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    MS_DEEP_DBG(F("After"), cmd_number, F("data commands got"), resultsReceived,
                F("results of the expected"),
                _numReturnedValues - _incCalcValues, F("expected. This is a"),
                resultsReceived == _numReturnedValues - _incCalcValues
                    ? F("success.")
                    : F("failure."));

    // Empty the buffer and de-activate the SDI-12 Object
    deactivate();

    return success && (_numReturnedValues - _incCalcValues) == resultsReceived;
}


#ifndef MS_SDI12_NON_CONCURRENT
bool SDI12Sensors::addSingleMeasurementResult(void) {
    bool success = false;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        success = getResults(MS_SDI12_USE_CRC);
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
    clearStatusBits(MEASUREMENT_ATTEMPTED, MEASUREMENT_SUCCESSFUL);
    // Bump the number of completed measurement attempts
    _measurementAttemptsCompleted++;

    if (success) {
        // Bump the number of successful measurements
        _measurementsSucceeded++;
    }

    return success;
}
#else
bool SDI12Sensors::addSingleMeasurementResult(void) {
    bool success = false;

    String startCommand;
    String sdiResponse;

    // activate the SDI-12 object
    activate();

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
            setStatusBit(MEASUREMENT_SUCCESSFUL);

            // Since this is not a concurrent measurement, we must sit around
            // and wait for the sensor to issue a service request telling us
            // that the measurement is ready.

            uint32_t timerStart = millis();
            while ((millis() - timerStart) <
                   static_cast<uint32_t>(1000 * (wait))) {
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
            success = getResults(MS_SDI12_USE_CRC);
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

    // Empty the buffer and de-activate the SDI-12 Object
    deactivate();

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    clearStatusBits(MEASUREMENT_ATTEMPTED, MEASUREMENT_SUCCESSFUL);
    // Bump the number of completed measurement attempts
    _measurementAttemptsCompleted++;

    if (success) {
        // Bump the number of successful measurements
        _measurementsSucceeded++;
    }

    return success;
}
#endif  // #ifndef MS_SDI12_NON_CONCURRENT
