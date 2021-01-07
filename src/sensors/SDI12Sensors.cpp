/**
 * @file SDI12Sensors.cpp
 * @copyright 2020 Stroud Water Research Center
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
                           const uint8_t numReturnedVars,
                           uint32_t      warmUpTime_ms,
                           uint32_t      stabilizationTime_ms,
                           uint32_t      measurementTime_ms)
    : Sensor(sensorName, numReturnedVars, warmUpTime_ms, stabilizationTime_ms,
             measurementTime_ms, powerPin, dataPin, measurementsToAverage),
      _SDI12Internal(dataPin) {
    _SDI12address = SDI12address;
}
SDI12Sensors::SDI12Sensors(char* SDI12address, int8_t powerPin, int8_t dataPin,
                           uint8_t       measurementsToAverage,
                           const char*   sensorName,
                           const uint8_t numReturnedVars,
                           uint32_t      warmUpTime_ms,
                           uint32_t      stabilizationTime_ms,
                           uint32_t      measurementTime_ms)
    : Sensor(sensorName, numReturnedVars, warmUpTime_ms, stabilizationTime_ms,
             measurementTime_ms, powerPin, dataPin, measurementsToAverage),
      _SDI12Internal(dataPin) {
    _SDI12address = *SDI12address;
}
SDI12Sensors::SDI12Sensors(int SDI12address, int8_t powerPin, int8_t dataPin,
                           uint8_t       measurementsToAverage,
                           const char*   sensorName,
                           const uint8_t numReturnedVars,
                           uint32_t      warmUpTime_ms,
                           uint32_t      stabilizationTime_ms,
                           uint32_t      measurementTime_ms)
    : Sensor(sensorName, numReturnedVars, warmUpTime_ms, stabilizationTime_ms,
             measurementTime_ms, powerPin, dataPin, measurementsToAverage),
      _SDI12Internal(dataPin) {
    _SDI12address = SDI12address + '0';
}
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

#if defined __AVR__ || defined ARDUINO_ARCH_AVR
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
    myCommand += static_cast<char>(_SDI12address);
    myCommand += "!";  // sends 'acknowledge active' command [address][!]

    bool    didAcknowledge = false;
    uint8_t ntries         = 0;
    while (!didAcknowledge && ntries < 5) {
        MS_DEEP_DBG(F("    >>>"), myCommand);
        _SDI12Internal.sendCommand(myCommand);
        delay(30);

        // wait for acknowlegement with format:
        // [address]<CR><LF>
        String sdiResponse = _SDI12Internal.readStringUntil('\n');
        sdiResponse.trim();
        MS_DEEP_DBG(F("    <<<"), sdiResponse);

        // Empty the buffer again
        _SDI12Internal.clearBuffer();

        if (sdiResponse == String(_SDI12address)) {
            MS_DEEP_DBG(F("   "), getSensorNameAndLocation(),
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
    // MS_DBG(F("   Activating SDI-12 instance for"),
    //        getSensorNameAndLocation());
    // Check if this the currently active SDI-12 Object
    bool wasActive = _SDI12Internal.isActive();
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set correctly.
    // if (wasActive) {
    //     MS_DBG(F("   SDI-12 instance for"), getSensorNameAndLocation(),
    //            F("was already active!"));
    // }
    if (!wasActive) _SDI12Internal.begin();
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    // Check that the sensor is there and responding
    if (!requestSensorAcknowledgement()) return false;

    MS_DEEP_DBG(F("  Get sensor info"));
    String myCommand = "";
    myCommand += static_cast<char>(_SDI12address);
    myCommand += "I!";  // sends 'info' command [address][I][!]
    MS_DEEP_DBG(F("    >>>"), myCommand);
    _SDI12Internal.sendCommand(myCommand);
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
        MS_DBG(F("  SDI12 Address:"), sdi12Address);
        float sdi12Version = sdiResponse.substring(1, 3).toFloat();
        sdi12Version /= 10;
        MS_DBG(F("  SDI12 Version:"), sdi12Version);
        _sensorVendor = sdiResponse.substring(3, 11);
        _sensorVendor.trim();
        MS_DBG(F("  Sensor Vendor:"), _sensorVendor);
        _sensorModel = sdiResponse.substring(11, 17);
        _sensorModel.trim();
        MS_DBG(F("  Sensor Model:"), _sensorModel);
        _sensorVersion = sdiResponse.substring(17, 20);
        _sensorVersion.trim();
        MS_DBG(F("  Sensor Version:"), _sensorVersion);
        _sensorSerialNumber = sdiResponse.substring(20);
        _sensorSerialNumber.trim();
        MS_DBG(F("  Sensor Serial Number:"), _sensorSerialNumber);
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


#ifndef MS_SDI12_NON_CONCURRENT
// Sending the command to get a concurrent measurement
bool SDI12Sensors::startSingleMeasurement(void) {
    // Sensor::startSingleMeasurement() checks that if it's awake/active and
    // sets the timestamp and status bits.  If it returns false, there's no
    // reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    String startCommand;
    String sdiResponse;
    bool   wasActive;

    // MS_DBG(F("   Activating SDI-12 instance for"),
    //        getSensorNameAndLocation());
    // Check if this the currently active SDI-12 Object
    wasActive = _SDI12Internal.isActive();
    // if (wasActive) {
    //     MS_DBG(F("   SDI-12 instance for"), getSensorNameAndLocation(),
    //            F("was already active!"));
    // }
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

    MS_DBG(F("  Beginning concurrent measurement on"),
           getSensorNameAndLocation());
    startCommand = "";
    startCommand += _SDI12address;
    startCommand +=
        "C!";  // Start concurrent measurement - format  [address]['C'][!]
    MS_DEEP_DBG(F("    >>>"), startCommand);
    _SDI12Internal.sendCommand(startCommand);
    delay(30);  // It just needs this little delay

    // wait for acknowlegement with format
    // [address][ttt (3 char, seconds)][number of values to be returned,
    // 0-9]<CR><LF>
    sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    _SDI12Internal.clearBuffer();
    MS_DEEP_DBG(F("    <<<"), sdiResponse);

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    if (!wasActive) _SDI12Internal.end();

    // Verify the number of results the sensor will send
    uint8_t numVariables = sdiResponse.substring(4).toInt();
    if (numVariables != _numReturnedValues) {
        PRINTOUT(numVariables, F("results expected"),
                 F("This differs from the sensor's standard design of"),
                 _numReturnedValues, F("measurements!!"));
    }

    // Set the times we've activated the sensor and asked for a measurement
    if (sdiResponse.length() > 0) {
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
#endif //#ifndef MS_SDI12_NON_CONCURRENT


#if defined MS_SDI12_SINGLE_LINE_RESPONSE
/* This section defineesthree options ~ 
 the preferred option is no defines, 

 When the SDI-12 sensors send data in response to a single D! /D0! command, the size of the response is limited to either 35 or 75characters. 
 (SDI-12-ver1.4 sect 4.4.8).
 
 This sections parses a single line response from the instrument, and maintains compatibility for ModularSensors prior to 0.27.0
 This method is depreciated, and will be removed at some future release
*/
bool SDI12Sensors::addSingleMeasurementResult(void) {
    bool success = false;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        // MS_DBG(F("   Activating SDI-12 instance for"),
        //        getSensorNameAndLocation());
        // Check if this the currently active SDI-12 Object
        bool wasActive = _SDI12Internal.isActive();
        // if (wasActive) {
        //     MS_DBG(F("   SDI-12 instance for"), getSensorNameAndLocation(),
        //            F("was already active!"));
        // }
        // If it wasn't active, activate it now.
        // Use begin() instead of just setActive() to ensure timer is set
        // correctly.
        if (!wasActive) _SDI12Internal.begin();
        // Empty the buffer
        _SDI12Internal.clearBuffer();

        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
        String getDataCommand = "";
        getDataCommand += _SDI12address;
        // SDI-12 command to get data [address][D][dataOption][!]
        getDataCommand += "D0!";
        MS_DEEP_DBG(F("    >>>"), getDataCommand);
        _SDI12Internal.sendCommand(getDataCommand);
        delay(30);  // It just needs this little delay

        uint32_t start = millis();
        while (_SDI12Internal.available() < 3 && (millis() - start) < 1500) {}
        MS_DEEP_DBG(F("  Receiving results from"), getSensorNameAndLocation());
        _SDI12Internal.read();  // ignore the repeated SDI12 address
        for (uint8_t i = 0; i < _numReturnedValues; i++) {
            float result = _SDI12Internal.parseFloat();
            // The SDI-12 library should return -9999 on timeout
            if (result == -9999 || isnan(result)) result = -9999;
            MS_DEEP_DBG(F("    <<< Result #"), i, ':', result);
            verifyAndAddMeasurementResult(i, result);
        }
        // String sdiResponse = _SDI12Internal.readStringUntil('\n');
        // sdiResponse.trim();
        // _SDI12Internal.clearBuffer();
        // MS_DEEP_DBG(F("    <<<"), sdiResponse);

        // Empty the buffer again
        _SDI12Internal.clearBuffer();

        // De-activate the SDI-12 Object
        // Use end() instead of just forceHold to un-set the timers
        if (!wasActive) _SDI12Internal.end();

        success = true;
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
#elif defined MS_SDI12_NON_CONCURRENT
/*
The M!/C! is the MS_SDI12_NON_CONCURRENT option. 
With a concurrent measurement (C!) cmd, it initiates taking a measurement, 
and then it should proceed to take the measurement and wait until you ask it for data. 
While it's taking the concurrent measurement, it should ignore everything on the line except an explicit command to its own address. 
For fairly old sensors, or ones where the manufacturer screwed up the implementation (Insitu RDO..), 
the other option is a measurement with the M! command. 
With the M! command, you must wait until the measurement on that sensor is finished and the sensor sends back a service request (just an !). 
If there's any other command on the line before the sensor finishes, it aborts the measurement. 
So it's not ideal at all because if you have multiple sensors you have go one at a time start - wait - finish 
rather than starting all three at once and then asking for data when they ought to have finished. 
And, because of the way ModSen is structured, you can't go on to even non SDI-12 sensors in the interim. 
Unless you're super limited with pins and you just have to have two SDI-12 sensors on the same pin and they can't play nice, 
there's no reason to use this. 
That's why you have to explicitly define MS_SDI12_NON_CONCURRENT to use this and it then applies to everything.
*/
bool SDI12Sensors::addSingleMeasurementResult(void) {
    bool success = false;

    String startCommand;
    String sdiResponse;
    bool   wasActive;

    // MS_DBG(F("   Activating SDI-12 instance for"),
    //        getSensorNameAndLocation());
    // Check if this the currently active SDI-12 Object
    wasActive = _SDI12Internal.isActive();
    // if (wasActive) {
    //     MS_DBG(F("   SDI-12 instance for"), getSensorNameAndLocation(),
    //            F("was already active!"));
    // }
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set
    // correctly.
    if (!wasActive) _SDI12Internal.begin();
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(F("  Beginning concurrent measurement on"),
           getSensorNameAndLocation());
    startCommand = "";
    startCommand += _SDI12address;
    startCommand +=
        "M!";  // Start concurrent measurement - format  [address]['C'][!]
    MS_DEEP_DBG(F("    >>>"), startCommand);
    _SDI12Internal.sendCommand(startCommand);
    delay(30);  // It just needs this little delay

    // wait for acknowlegement with format
    // [address][ttt (3 char, seconds)][number of values to be returned,
    // 0-9]<CR><LF>
    sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    _SDI12Internal.clearBuffer();
    MS_DEEP_DBG(F("    <<<"), sdiResponse);

    // find out how long we have to wait (in seconds).
    uint8_t wait = sdiResponse.substring(1, 4).toInt();

    // Verify the number of results the sensor will send
    uint8_t numVariables = sdiResponse.substring(4).toInt();
    if (numVariables != _numReturnedValues) {
        PRINTOUT(numVariables, F("results expected"),
                 F("This differs from the sensor's standard design of"),
                 _numReturnedValues, F("measurements!!"));
    }

    // Set the times we've activated the sensor and asked for a measurement
    if (sdiResponse.length() > 0) {
        MS_DBG(F("    NON-concurrent measurement started."));
        // Update the time that a measurement was requested
        _millisMeasurementRequested = millis();
        // Set the status bit for measurement start success (bit 6)
        _sensorStatus |= 0b01000000;
    } else {
        MS_DBG(getSensorNameAndLocation(),
               F("did not respond to measurement request!"));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
    }

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        // Since this is not a concurrent measurement, we must sit around and
        // wait for the sensor to issue a service request telling us that the
        // measurement is ready.

        unsigned long timerStart = millis();
        while ((millis() - timerStart) < (1000 * (wait))) {
            if (_SDI12Internal.available())  // sensor can interrupt us to let
                                             // us know it is done early
            {
                MS_DEEP_DBG(F("    <<<"), _SDI12Internal.readStringUntil('\n'));
                _SDI12Internal.clearBuffer();
                break;
            }
        }
        // Wait for anything else and clear it out
        delay(30);
        _SDI12Internal.clearBuffer();

        // get the results
        success = getResults();

        // Empty the buffer again
        _SDI12Internal.clearBuffer();

        // De-activate the SDI-12 Object
        // Use end() instead of just forceHold to un-set the timers
        if (!wasActive) _SDI12Internal.end();
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
#else  //NOT (MS_SDI12_NON_CONCURRENT OR MS_SDI12_SINGLE_LINE_RESPONSE)
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
#endif

/* 
 When the SDI-12 sensors send data in response to a single D! /D0! command, the size of the response is limited to either 35 or 75characters. 
 (SDI-12-ver1.4 sect 4.4.8).
 This parses multiple line response from the instrument until all the defined variables are retrieved.

 To retrieve multiple lines, the instrument is polled with increasing D#'s. 
 Tested with 
    Insitu ? RDO
    Insitu LT500 
*/
bool SDI12Sensors::getResults(void) {
    // MS_DBG(F("   Activating SDI-12 instance for"),
    //        getSensorNameAndLocation());
    // Check if this the currently active SDI-12 Object
    bool wasActive = _SDI12Internal.isActive();
    __attribute__((unused)) char sdiNextChar;

    // if (wasActive) {
    //     MS_DBG(F("   SDI-12 instance for"), getSensorNameAndLocation(),
    //            F("was already active!"));
    // }
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set
    // correctly.
    if (!wasActive) _SDI12Internal.begin();
    // Empty the buffer
    _SDI12Internal.clearBuffer();
 
    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
    uint8_t resultsReceived = 0;
    uint8_t cmd_number      = 0;
    while (resultsReceived < _numReturnedValues && cmd_number <= 9) {
        bool   gotResults     = false;
        String getDataCommand = "";
        getDataCommand += _SDI12address;
        // SDI-12 command to get data [address][D][dataOption][!]
        getDataCommand += "D";
        getDataCommand += cmd_number;
        getDataCommand += "!";
        MS_DEEP_DBG(F("    >>>"), getDataCommand);
        _SDI12Internal.sendCommand(getDataCommand);
        delay(30);  // It just needs this little delay

        uint32_t start = millis();
        while (_SDI12Internal.available() < 3 && (millis() - start) < 1500) {}
        //MS_DBG(F("  Receiving results from"), getSensorNameAndLocation());
        sdiNextChar = static_cast<char>(_SDI12Internal.read());
        MS_DEEP_DBG(F("    <<<"),sdiNextChar);
        // ^^ ignore the repeated SDI12 address

        start = millis();
        while (0 < _SDI12Internal.available()) {
            int c = _SDI12Internal.peek();
            if (c == '-' || (c >= '0' && c <= '9') || c == '.'|| c == '+') {
                float result = _SDI12Internal.parseFloat();
                // The SDI-12 library should return -9999 on timeout
                if (result == -9999 || isnan(result)) result = -9999;
                MS_DEEP_DBG(F("    <<<"), String(result, 10));
                verifyAndAddMeasurementResult(resultsReceived, result);
                if (result != -9999) {
                    gotResults = true;
                    resultsReceived++;
                }
            } else if (c >= 0 && c != '\r' && c != '\n') {
                sdiNextChar = static_cast<char>(_SDI12Internal.read());
                MS_DEEP_DBG(F("    <<<"), sdiNextChar);
            } else {  // no point -1's and new lines to debugging port
                _SDI12Internal.read();
            }
            delay(10);  // 1 character ~ 7.5ms
            STANDARD_SERIAL_OUTPUT.write(c); //NJH Fast write debug
            if ((millis() - start) > 10000) {
                PRINTOUT("SDI12Sensors parse timeout",_SDI12Internal.readStringUntil('\n'));
                break;
            }
        }
        if (!gotResults) {
            MS_DBG(F("  No results received, will not continue requests!"));
            break;  // don't do another loop if we got nothing
        }
        MS_DBG(F("  Total Results Received: "), resultsReceived,
               F(", Remaining: "), _numReturnedValues - resultsReceived);
        cmd_number++;
    }
    // String sdiResponse = _SDI12Internal.readStringUntil('\n');
    // sdiResponse.trim();
    // _SDI12Internal.clearBuffer();
    // MS_DEEP_DBG(F("    <<<"), sdiResponse);

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    if (!wasActive) _SDI12Internal.end();

    return _numReturnedValues == resultsReceived;
}


