/**
 * @file ANBpH.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 * @brief Implements the ANBpH class.
 */

#include "ANBpH.h"

// The constructor
ANBpH::ANBpH(byte modbusAddress, Stream* stream, int8_t powerPin,
             int16_t loggingIntervalMinutes, int8_t powerPin2, int8_t enablePin,
             uint8_t measurementsToAverage)
    : Sensor("ANBpHSensor", ANB_PH_NUM_VARIABLES, ANB_PH_WARM_UP_TIME_MS,
             ANB_PH_STABILIZATION_TIME_MS, ANB_PH_2ND_VALUE_LOW_SALT, powerPin,
             -1, measurementsToAverage, ANB_PH_INC_CALC_VARIABLES),
      _anb_sensor(modbusAddress, stream, enablePin),
      _modbusAddress(modbusAddress),
      _stream(stream),
      _loggingIntervalMinutes(loggingIntervalMinutes),
      _RS485EnablePin(enablePin) {
#ifdef MS_ANB_SENSORS_PH_DEBUG_DEEP
    _anb_sensor.setDebugStream(&MS_SERIAL_OUTPUT);
#endif
    setSecondaryPowerPin(powerPin2);
    setAllowedMeasurementRetries(5);
}
ANBpH::ANBpH(byte modbusAddress, Stream& stream, int8_t powerPin,
             int16_t loggingIntervalMinutes, int8_t powerPin2, int8_t enablePin,
             uint8_t measurementsToAverage)
    : Sensor("ANBpHSensor", ANB_PH_NUM_VARIABLES, ANB_PH_WARM_UP_TIME_MS,
             ANB_PH_STABILIZATION_TIME_MS, ANB_PH_2ND_VALUE_LOW_SALT, powerPin,
             -1, measurementsToAverage, ANB_PH_INC_CALC_VARIABLES),
      _anb_sensor(modbusAddress, stream, enablePin),
      _modbusAddress(modbusAddress),
      _stream(&stream),
      _loggingIntervalMinutes(loggingIntervalMinutes),
      _RS485EnablePin(enablePin) {
#ifdef MS_ANB_SENSORS_PH_DEBUG_DEEP
    _anb_sensor.setDebugStream(&MS_SERIAL_OUTPUT);
#endif
    setSecondaryPowerPin(powerPin2);
    setAllowedMeasurementRetries(5);
}
// Destructor
ANBpH::~ANBpH() {}


// The sensor installation location on the Mayfly
String ANBpH::getSensorLocation(void) {
    String sensorLocation = F("modbus_0x");
    if (_modbusAddress < 16) sensorLocation += "0";
    sensorLocation += String(_modbusAddress, HEX);
    return sensorLocation;
}


bool ANBpH::setup(void) {
    bool retVal = Sensor::setup();  // this will set pin modes and the setup
                                    // status bit
    if (_RS485EnablePin >= 0) { pinMode(_RS485EnablePin, OUTPUT); }

    // This sensor needs power for setup!
    delay(10);
    MS_DEEP_DBG(F("Powering up for setup..."));
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }

    // Confirm Modbus Address
    MS_DBG(F("Selected modbus address:"));
    MS_DBG(F("- Decimal: "), String(_modbusAddress, DEC));
    MS_DBG(F("- Hexidecimal: "), _modbusAddress < 16 ? F("0x0") : F("0x"),
           String(_modbusAddress, HEX));

    MS_DEEP_DBG(F("Waiting for warm-up..."));
    waitForWarmUp();

    MS_DBG(F("Trying to get any modbus response..."));
    bool gotModbusResponse = _anb_sensor.gotModbusResponse();
    if (!gotModbusResponse) {
        MS_DBG(F("Did not get a modbus response, trying to force Modbus "
                 "enable..."));
        _anb_sensor.forceModbus();
        MS_DBG(F("Trying again get a modbus response..."));
        gotModbusResponse = _anb_sensor.gotModbusResponse();
        MS_DBG(F("..."), gotModbusResponse ? F("success") : F("failed"));
    }
    retVal &= gotModbusResponse;
    if (!gotModbusResponse) {
        // Set the status error bit (bit 7)
        setStatusBit(ERROR_OCCURRED);
        // UN-set the set-up bit (bit 0) since setup failed!
        clearStatusBit(SETUP_SUCCESSFUL);
        return retVal;
    }

    // Print Sensor Information
    MS_DBG(F(" Getting sensor information..."));

    // Print the sensor serial number
    String SN = _anb_sensor.getSerialNumber();
    MS_DBG(F("    Serial Number:"), SN);

    // Print the sensor manufacturer
    String manufacturer = _anb_sensor.getManufacturer();
    MS_DBG(F("    Manufacturer:"), manufacturer);

    // Print the sensor name
    String name = _anb_sensor.getName();
    MS_DBG(F("    Name:"), name);

    // Print the sensor sub-name
    String subname = _anb_sensor.getSubName();
    MS_DBG(F("    Subname:"), subname);

    // Print the sensor interface version
    String interfaceVersion = _anb_sensor.getInterfaceVersion();
    MS_DBG(F("    Interface Version:"), interfaceVersion);

    // Print the sensor driver version
    String driverVersion = _anb_sensor.getDriverVersion();
    MS_DBG(F("    Driver Version:"), driverVersion);

    // Configure sensor
    MS_DBG(F("Configuring sensor..."));

    // Set Sensor Control Mode
    // NOTE: We always use controlled mode: this library is the controller
    MS_DBG(F("Set sensor control mode to controlled..."));
    bool modeSet = _anb_sensor.setControlMode(ANBSensorMode::CONTROLLED);
    MS_DBG(F("..."), modeSet ? F("success") : F("failed"));
    retVal &= modeSet;

    // Set Power Style based on the power pin
    MS_DBG(F("Set sensor power style to"),
           _powerPin >= 0 ? F("ON_MEASUREMENT") : F("ALWAYS_POWERED"),
           F("..."));
    bool powerStyleSet;
    if (_powerPin >= 0) {
        powerStyleSet =
            _anb_sensor.setPowerStyle(ANBPowerStyle::ON_MEASUREMENT);
    } else {
        powerStyleSet =
            _anb_sensor.setPowerStyle(ANBPowerStyle::ALWAYS_POWERED);
    }
    MS_DBG(F("..."), powerStyleSet ? F("success") : F("failed"));
    retVal &= powerStyleSet;

    bool     intervalSet        = false;
    uint16_t programmedInterval = _loggingIntervalMinutes;
    if (_loggingIntervalMinutes == 0 && _powerPin >= 0) {
        programmedInterval = 10;
        MS_DBG(F("Requested interval of 0 minutes is invalid when power is "
                 "cycled; using"),
               programmedInterval, F("minutes."));
    }
    if (_loggingIntervalMinutes < 10 && _loggingIntervalMinutes != 0) {
        programmedInterval = 10;
        MS_DBG(F("Requested interval of"), _loggingIntervalMinutes,
               F("minutes is too short; using"), programmedInterval,
               F("minutes."));
    }
    if (_loggingIntervalMinutes > 240 && _loggingIntervalMinutes != 0) {
        programmedInterval = 240;
        MS_DBG(F("Requested interval of"), _loggingIntervalMinutes,
               F("minutes is too long; using"), programmedInterval,
               F("minutes."));
    }
    if (_powerPin >= 0) {
        // Set sampling interval to the expected sampling interval if the sensor
        MS_DBG(F("Set sensor sampling interval to"), programmedInterval,
               F("minutes..."));
        intervalSet = _anb_sensor.setIntervalTime(programmedInterval);
    } else {
        // Set sampling interval to continuous if the sensor will be
        // continuously powered (ie, a power style of ALWAYS_POWERED).
        MS_DBG(F("Set sensor sampling interval to 0 (continuous)..."));
        intervalSet = _anb_sensor.setIntervalTime(0);
    }
    MS_DBG(F("..."), intervalSet ? F("success") : F("failed"));
    retVal &= intervalSet;

    // Set Sensor Salinity Mode
    retVal &= setSalinityMode(_salinityMode);

    // Set Immersion Rule
    MS_DBG(F("Set sensor immersion rule to"),
           _immersionSensorEnabled ? F("enabled") : F("disabled"), F("..."));
    bool immersionSet =
        _anb_sensor.enableImmersionSensor(_immersionSensorEnabled);
    MS_DBG(F("..."), immersionSet ? F("success") : F("failed"));
    retVal &= immersionSet;

    // Set the sensor RTC if possible
    retVal &= setSensorRTC();

    if (!retVal) {
        // Set the status error bit (bit 7)
        setStatusBit(ERROR_OCCURRED);
        // UN-set the set-up bit (bit 0) since setup failed!
        clearStatusBit(SETUP_SUCCESSFUL);
    }

    // Turn the power back off it it had been turned on
    if (!wasOn) {
        MS_DEEP_DBG(F("Powering down after setup"));
        powerDown();
    }

    return retVal;
}


bool ANBpH::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;

    MS_DEEP_DBG(F("Checking for modbus response confirming"),
                getSensorNameAndLocation(), F("is awake"));
    bool is_ready = isSensorReady(&anbSensor::isSensorReady,
                                  ANB_PH_MINIMUM_REQUEST_SPACING,
                                  _millisPowerOn);
    if (!is_ready) {
        MS_DEEP_DBG(getSensorNameAndLocation(),
                    F("isn't responding to modbus commands; wake failed!"));
        // Set the status error bit (bit 7)
        setStatusBit(ERROR_OCCURRED);
        // Make sure that the wake time and wake success bit (bit 4) are unset
        _millisSensorActivated = 0;
        clearStatusBit(WAKE_SUCCESSFUL);
        return false;
    }

    MS_DEEP_DBG(getSensorNameAndLocation(),
                F("responded properly to modbus commands; it must be awake."));

    // If the sensor is being power cycled, set the clock before each
    // measurement. The sensor stores the measurements on its internal storage,
    // so it's best to have the correct time.
    if (_powerPin >= 0) { setSensorRTC(); }

    // Send the command to begin taking readings, trying up to 5 times
    bool    success = false;
    uint8_t ntries  = 0;
    MS_DBG(F("Start scanning on"), getSensorNameAndLocation());
    while (!success && ntries < 5) {
        MS_DEEP_DBG('(', ntries + 1, F("):"));
        success = _anb_sensor.start();
        ntries++;
    }

    if (success) {
        MS_DEEP_DBG(getSensorNameAndLocation(), F("started scanning."));
        // Update the time that a measurement was requested
        _millisSensorActivated = millis();
        _lastModbusCommandTime = 0;
    } else {
        // Set the status error bit (bit 7)
        setStatusBit(ERROR_OCCURRED);
        // Make sure that the wake time and wake success bit (bit 4) are unset
        _millisSensorActivated = 0;
        clearStatusBit(WAKE_SUCCESSFUL);
        return false;
    }

    return true;
}


// The function to put the sensor to sleep
// Different from the standard in that it stops measurements and empties and
// flushes the stream.
bool ANBpH::sleep(void) {
    // empty then flush the buffer
    while (_stream->available()) { _stream->read(); }
    _stream->flush();

    // if it's not powered, it's asleep
    if (!checkPowerOn()) { return true; }
    // if it was never awake, it's probably asleep
    if (_millisSensorActivated == 0) {
        MS_DBG(getSensorNameAndLocation(), F("was not measuring!"));
        return true;
    }

    // Send the command to stop taking readings, trying up to 5 times
    bool    success = false;
    uint8_t ntries  = 0;
    MS_DBG(F("Stop Measurement on"), getSensorNameAndLocation());
    while (!success && ntries < 5) {
        MS_DBG('(', ntries + 1, F("):"));
        success = _anb_sensor.stop();
        ntries++;
    }
    if (success) {
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the measurement request time
        _millisMeasurementRequested = 0;
        // Unset the status bits for sensor activation (bits 3 & 4) and
        // measurement request (bits 5 & 6)
        clearStatusBits(WAKE_ATTEMPTED, WAKE_SUCCESSFUL, MEASUREMENT_ATTEMPTED,
                        MEASUREMENT_SUCCESSFUL);
        MS_DBG(F("Measurements stopped."));
    } else {
        MS_DBG(F("Measurements NOT stopped!"));
    }

    // empty then flush the buffer
    while (_stream->available()) { _stream->read(); }
    _stream->flush();

    return success;
}

bool ANBpH::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool              success    = false;
    float             pH         = -9999;
    float             temp       = -9999;
    float             sal        = -9999;
    float             spcond     = -9999;
    float             raw_cond   = -9999;
    ANBHealthCode     health     = ANBHealthCode::UNKNOWN;
    ANBStatusCode     status     = ANBStatusCode::UNKNOWN;
    ANBDiagnosticCode diagnostic = ANBDiagnosticCode::UNKNOWN;

#ifdef MS_ANB_SENSORS_PH_DEBUG_DEEP
    // Print the sensor RTC
    int8_t  seconds = -1;
    int8_t  minutes = -1;
    int8_t  hours   = -1;
    int8_t  day     = -1;
    int8_t  month   = -1;
    int16_t year    = -1;
    _anb_sensor.getRTC(seconds, minutes, hours, day, month, year);
    char time_buff[20] = {'\0'};
    sprintf(time_buff, "%04d-%02d-%02d %02d:%02d:%02d", year & 0x1FFF,
            month & 0x3f, day & 0x3f, hours & 0x3f, minutes & 0x3f,
            seconds & 0x3f);
    MS_DBG(F("    Current internal RTC value on"), getSensorNameAndLocation(),
           ':', time_buff);
#endif

    MS_DBG(F("Get Values from"), getSensorNameAndLocation());
    success = _anb_sensor.getValues(pH, temp, sal, spcond, raw_cond, health,
                                    diagnostic);
    status  = _anb_sensor.getStatusCode();

    // Print the values for debugging
    MS_DBG(F("  pH:"), pH);
    MS_DBG(F("  Temperature (C):"), temp);
    MS_DBG(F("  Salinity (ppt):"), sal);
    MS_DBG(F("  Specific Conductance (µS/cm):"), spcond);
    MS_DBG(F("  Raw Conductance (µS/cm):"), raw_cond);
    MS_DBG(F("  Health Code:"), static_cast<int16_t>(health), '-',
           _anb_sensor.getHealthString(health));
    MS_DBG(F("  Diagnostic Code:"), static_cast<int16_t>(diagnostic), '-',
           _anb_sensor.getDiagnosticString(diagnostic));
    MS_DBG(F("  Status Code:"), static_cast<int16_t>(status), '-',
           _anb_sensor.getStatusString(status));

    if (health == ANBHealthCode::NOT_IMMERSED) {
        PRINTOUT(F("  WARNING: ANB pH sensor is not immersed!"));
    }

    // We consider a measurement successful if we got a modbus response and
    // the pH value is in range or the health code says the sensor is not
    // immersed. We accept the not immersed condition as a successful
    // measurement because the sensor will not retry for at least 5 minutes
    // after an immersion error.
    success &= ((0.0 < pH && pH < 14.00) ||
                health == ANBHealthCode::NOT_IMMERSED);

    // Put values into the array - if it's a success or our last try
    if (success || _retryAttemptsMade >= _allowedMeasurementRetries) {
        verifyAndAddMeasurementResult(ANB_PH_PH_VAR_NUM, pH);
        verifyAndAddMeasurementResult(ANB_PH_TEMP_VAR_NUM, temp);
        verifyAndAddMeasurementResult(ANB_PH_SALINITY_VAR_NUM, sal);
        verifyAndAddMeasurementResult(ANB_PH_SPCOND_VAR_NUM, spcond);
        verifyAndAddMeasurementResult(ANB_PH_EC_VAR_NUM, raw_cond);
        verifyAndAddMeasurementResult(ANB_PH_HEALTH_CODE_VAR_NUM,
                                      static_cast<int16_t>(health));
        verifyAndAddMeasurementResult(ANB_PH_DIAGNOSTIC_CODE_VAR_NUM,
                                      static_cast<int16_t>(diagnostic));
        verifyAndAddMeasurementResult(ANB_PH_STATUS_CODE_VAR_NUM,
                                      static_cast<int16_t>(status));
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}

// check if the sensor is ready
bool ANBpH::isSensorReady(bool (anbSensor::*checkReadyFxn)(), uint32_t spacing,
                          uint32_t
#if defined(MS_ANB_SENSORS_PH_DEBUG)
                              startTime
#endif
) {
    uint32_t elapsed_since_last_request = millis() - _lastModbusCommandTime;
#if defined(MS_ANB_SENSORS_PH_DEBUG)
    uint32_t elapsed_since_start_time = millis() - startTime;
#endif
    if (elapsed_since_last_request < spacing) {
        // MS_DEEP_DBG(
        //     F("It's only been"), elapsed_since_last_request,
        //     F("ms since last status check. Wait a bit before checking
        //     again."));
        return false;
    }
    bool ready = (_anb_sensor.*checkReadyFxn)();
    if (ready) {
        MS_DBG(F("It's been"), elapsed_since_start_time, F("ms, and"),
               getSensorNameAndLocation(), F("is ready."));
        // if it's ready, then it's ok to ask it again right away
        _lastModbusCommandTime = 0;
    } else {
        MS_DBG(F("It's been"), elapsed_since_start_time, F("ms, and"),
               getSensorNameAndLocation(), F("is not ready yet."));
        // if the sensor isn't ready, force a wait before checking again
        _lastModbusCommandTime = millis();
    }
    return ready;
}


// This checks to see if enough time has passed for warm-up
bool ANBpH::isWarmedUp(bool debug) {
#if defined(MS_ANB_SENSORS_PH_DEBUG_DEEP) || defined(MS_SENSORBASE_DEBUG)
    debug = true;
#endif
    // If the sensor doesn't have power, then it will never be warmed up,
    // so the warm up time is essentially already passed.
    if (!getStatusBit(POWER_SUCCESSFUL)) {
        if (debug) {
            MS_DBG(getSensorNameAndLocation(),
                   F("does not have power and cannot warm up!"));
        }
        return true;
    }

    uint32_t elapsed_since_power_on = millis() - _millisPowerOn;
    // If it's past the maximum warm-up time, warm-up failed, but our wait is
    // over
    if (elapsed_since_power_on > ANB_PH_WARM_UP_TIME_MAX) {
        MS_DBG(F("It's been"), elapsed_since_power_on, F("ms, and"),
               getSensorNameAndLocation(), F("timed out after power up."));
        return true;  // timeout
    } else if (elapsed_since_power_on > _warmUpTime_ms) {
        bool is_ready = isSensorReady(&anbSensor::isSensorReady,
                                      ANB_PH_MINIMUM_REQUEST_SPACING,
                                      _millisPowerOn);
        if (is_ready) {
            MS_DBG(F("It's been"), elapsed_since_power_on, F("ms, and"),
                   getSensorNameAndLocation(),
                   F("got a valid modbus response meaning it's warmed up."));
        }
        return is_ready;
    } else {
        // wait at least the minimum warm-up time
        return false;
    }
}

uint32_t ANBpH::getStartMeasurementWindow(void) {
    if (_powerPin >= 0 && _retryAttemptsMade == 0) {
        if (_salinityMode == ANBSalinityMode::HIGH_SALINITY) {
            return ANB_PH_1ST_VALUE_HIGH_SALT;
        } else {
            return ANB_PH_1ST_VALUE_LOW_SALT;
        }
    } else {
        return 0;
    }
}

// If no pin was provided for power, we assume it's always powered and use
// the maximum wait time for the second measurement as our maximum wait.
// If a pin was provided for power, we assume it's on-demand powered and use
// the maximum wait time for the first measurement as our maximum wait.
uint32_t ANBpH::getEndMeasurementWindow(void) {
    if (_powerPin >= 0 && _retryAttemptsMade == 0) {
        if (_salinityMode == ANBSalinityMode::HIGH_SALINITY) {
            return ANB_PH_1ST_VALUE_HIGH_SALT_MAX;
        } else {
            return ANB_PH_1ST_VALUE_LOW_SALT_MAX;
        }
    } else {
        if (_salinityMode == ANBSalinityMode::HIGH_SALINITY) {
            return ANB_PH_2ND_VALUE_HIGH_SALT;
        } else {
            return ANB_PH_2ND_VALUE_LOW_SALT;
        }
    }
}

// This checks to see if enough time has passed for measurement completion
bool ANBpH::isMeasurementComplete(bool debug) {
#if defined(MS_ANB_SENSORS_PH_DEBUG_DEEP)
    debug = true;
#endif
    // If a measurement failed to start, the sensor will never return a result,
    // so the measurement time is essentially already passed
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        if (debug) {
            MS_DBG(
                getSensorNameAndLocation(),
                F("is not taking a measurement and will not return a value!"));
        }
        return true;
    }

    uint32_t elapsed_since_meas_start = millis() - _millisMeasurementRequested;

    // After the first measurement, the sensor will always report that a
    // measurement is ready, but a new value will not be available for at
    // least 10.5 (high salinity) or 14 (low salinity) seconds.
    if (_retryAttemptsMade > 0) {
        if (elapsed_since_meas_start > _measurementTime_ms) {
            if (debug) {
                MS_DBG(F("It's been"), elapsed_since_meas_start,
                       F("ms, and measurement by"), getSensorNameAndLocation(),
                       F("should be complete!"));
            }
            return true;
        } else {
            // If the sensor is measuring but the time hasn't passed, we still
            // need to wait
            return false;
        }
    }

    // If we're past the maximum wait time, the measurement failed, but our wait
    // is over
    if (elapsed_since_meas_start > getEndMeasurementWindow()) {
        MS_DBG(F("It's been"), elapsed_since_meas_start, F("ms, and"),
               getSensorNameAndLocation(),
               F("timed out waiting for a measurement to complete."));
        return true;  // timeout
    }

    bool is_ready = false;
    // Since the sensor takes so very long to measure when it's power cycled, if
    // we know it's going to be a while, we drop the query frequency to once
    // every 15 seconds because there's no point in asking more often than that.
    if (elapsed_since_meas_start <= getStartMeasurementWindow()) {
        is_ready = isSensorReady(&anbSensor::isMeasurementComplete, 15000L,
                                 _millisMeasurementRequested);
    }
    // Check every second when we're in the window of when a
    // measurement might be ready
    if (elapsed_since_meas_start > getStartMeasurementWindow() &&
        elapsed_since_meas_start <= getEndMeasurementWindow()) {
        is_ready = isSensorReady(&anbSensor::isMeasurementComplete, 1000L,
                                 _millisMeasurementRequested);
    }
    if (is_ready) {
        MS_DBG(F("It's been"), elapsed_since_meas_start, F("ms, and"),
               getSensorNameAndLocation(),
               F("says it's finished with a measurement."));
    }
    return is_ready;
}


bool ANBpH::setSalinityMode(ANBSalinityMode newSalinityMode) {
    MS_DBG(F("Set sensor salinity mode..."));
    bool salinitySet = _anb_sensor.setSalinityMode(newSalinityMode);
    MS_DBG(F("..."), salinitySet ? F("success") : F("failed"));
    if (!salinitySet) { return false; }
    // If we succeeded in setting the salinity mode, update the local copy and
    // the measurement time
    _salinityMode = newSalinityMode;
    if (_salinityMode == ANBSalinityMode::HIGH_SALINITY) {
        _measurementTime_ms = ANB_PH_2ND_VALUE_HIGH_SALT;
    } else {
        _measurementTime_ms = ANB_PH_2ND_VALUE_LOW_SALT;
    }
    return true;
}

bool ANBpH::enableImmersionSensor(bool enable) {
    _immersionSensorEnabled = enable;
    return _anb_sensor.enableImmersionSensor(enable);
}

bool ANBpH::setSensorRTC() {
    // Set the sensor clock to the current time
    MS_DEEP_DBG(F("Attempting to set sensor RTC on"),
                getSensorNameAndLocation(), F("..."));
    if (!loggerClock::isRTCSane()) {
        MS_DBG(
            F("Current logger time is not sane, so not setting sensor RTC!"));
        return true;
    }

    int8_t  seconds   = -1;
    int8_t  minutes   = -1;
    int8_t  hours     = -1;
    int8_t  day       = -1;
    int8_t  month     = -1;
    int16_t year      = -1;
    uint8_t tz_offset = 0;  // Neutral value, will be overwritten by getNowParts
    Logger::getNowParts(seconds, minutes, hours, day, month, year, tz_offset);
#if defined(MS_ANB_SENSORS_PH_DEBUG_DEEP)
    char time_buff_l[20] = {'\0'};
    sprintf(time_buff_l, "%04d-%02d-%02d %02d:%02d:%02d", year & 0x1FFF,
            month & 0x3f, day & 0x3f, hours & 0x3f, minutes & 0x3f,
            seconds & 0x3f);
    MS_DEEP_DBG(F("    Logger date/time:"), time_buff_l);
#endif

    // Set RTC
    // NOTE: The sensor's RTC resets every time the sensor loses power.
    MS_DBG(F("Set"), getSensorNameAndLocation(), F("RTC..."));
    bool rtcSet = _anb_sensor.setRTC(seconds, minutes, hours, day, month, year);
    MS_DBG(F("..."), rtcSet ? F("success") : F("failed"));
    // a delay after setting the RTC helps it "take"
    delay(1000L);

    // Print the sensor RTC to cross check
    int8_t  seconds2 = -1;
    int8_t  minutes2 = -1;
    int8_t  hours2   = -1;
    int8_t  day2     = -1;
    int8_t  month2   = -1;
    int16_t year2    = -1;
    _anb_sensor.getRTC(seconds2, minutes2, hours2, day2, month2, year2);
#if defined(MS_ANB_SENSORS_PH_DEBUG)
    char time_buff[20] = {'\0'};
    sprintf(time_buff, "%04d-%02d-%02d %02d:%02d:%02d", year2 & 0x1FFF,
            month2 & 0x3f, day2 & 0x3f, hours2 & 0x3f, minutes2 & 0x3f,
            seconds2 & 0x3f);
    MS_DBG(F("    Internal RTC value on"), getSensorNameAndLocation(),
           F("after set:"), time_buff);
#endif

    return rtcSet;
}

// cSpell:ignore spcond
