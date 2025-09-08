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

// The constructor - need the sensor type, modbus address, power pin, stream for
// data, and number of readings to average
ANBpH::ANBpH(byte modbusAddress, Stream* stream, int8_t powerPin,
             int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage)
    : Sensor("ANBpHSensor", ANB_PH_NUM_VARIABLES, ANB_PH_WARM_UP_TIME_MS,
             ANB_PH_STABILIZATION_TIME_MS, ANB_PH_1ST_VALUE_HIGH_SALT, powerPin,
             -1, measurementsToAverage, ANB_PH_INC_CALC_VARIABLES),
      _anb_sensor(modbusAddress, stream, enablePin),
      _stream(stream),
      _RS485EnablePin(enablePin),
      _powerPin2(powerPin2) {
#ifdef MS_ANB_SENSORS_PH_DEBUG_DEEP
    _anb_sensor.setDebugStream(&MS_SERIAL_OUTPUT);
#endif
}
ANBpH::ANBpH(byte modbusAddress, Stream& stream, int8_t powerPin,
             int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage)
    : Sensor("ANBpHSensor", ANB_PH_NUM_VARIABLES, ANB_PH_WARM_UP_TIME_MS,
             ANB_PH_STABILIZATION_TIME_MS, ANB_PH_1ST_VALUE_HIGH_SALT, powerPin,
             -1, measurementsToAverage, ANB_PH_INC_CALC_VARIABLES),
      _anb_sensor(modbusAddress, stream, enablePin),
      _modbusAddress(modbusAddress),
      _stream(&stream),
      _RS485EnablePin(enablePin),
      _powerPin2(powerPin2) {
#ifdef MS_ANB_SENSORS_PH_DEBUG_DEEP
    _anb_sensor.setDebugStream(&MS_SERIAL_OUTPUT);
#endif
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
    if (_powerPin2 >= 0) { pinMode(_powerPin2, OUTPUT); }

    // This sensor needs power for setup!
    delay(10);
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }

    // Confirm Modbus Address
    MS_DBG(F("Selected modbus address:"));
    MS_DBG(F("- Decimal: "), String(_modbusAddress, DEC));
    MS_DBG(F("- Hexidecimal: "), _modbusAddress < 16 ? F("0x0") : F("0x"),
           String(_modbusAddress, HEX));

    MS_DBG(F("Trying to get any modbus response..."));
    waitForWarmUp();
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

    // Set Sensor Salinity Mode
    MS_DBG(F("Set sensor salinity mode..."));
    bool salinitySet = _anb_sensor.setSalinityMode(_salinityMode);
    MS_DBG(F("..."), salinitySet ? F("success") : F("failed"));
    retVal &= salinitySet;

    // Set Immersion Rule
    MS_DBG(F("Set sensor immersion rule to"),
           _immersionSensorEnabled ? "enabled" : "disabled", F("..."));
    bool immersionSet = _anb_sensor.enableImmersionSensor();
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
    if (!wasOn) { powerDown(); }

    return retVal;
}


// Start measurements
bool ANBpH::startSingleMeasurement(void) {
    // Sensor::startSingleMeasurement() checks that if it's awake/active and
    // sets the timestamp and status bits.  If it returns false, there's no
    // reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    // If the sensor is being power cycled, set the clock before each
    // measurement. The sensor stores the measurements on its internal storage,
    // so it's best to have the correct time.
    if (_powerPin >= 0) { setSensorRTC(); }

    // Send the command to begin taking readings, trying up to 5 times
    bool    success = false;
    uint8_t ntries  = 0;
    MS_DBG(F("Start Measurement on"), getSensorNameAndLocation());
    while (!success && ntries < 5) {
        MS_DBG('(', ntries + 1, F("):"));
        success = _anb_sensor.start();
        ntries++;
    }

    if (success) {
        // Update the time that the sensor was activated
        _millisSensorActivated = millis();
        MS_DBG(getSensorNameAndLocation(), F("activated and measuring."));
    } else {
        MS_DBG(getSensorNameAndLocation(), F("was NOT activated!"));
        // Make sure the activation time is zero and the wake success bit (bit
        // 4) is unset
        _millisSensorActivated = 0;
        clearStatusBit(WAKE_SUCCESSFUL);
    }

    return success;
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

    // Send the command to begin taking readings, trying up to 5 times
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


// This turns on sensor power
void ANBpH::powerUp(void) {
    if (_powerPin >= 0) {
        // Reset power pin mode every power up because pins are set to tri-state
        // on sleep
        pinMode(_powerPin, OUTPUT);
        MS_DBG(F("Powering"), getSensorNameAndLocation(), F("with pin"),
               _powerPin);
        digitalWrite(_powerPin, HIGH);
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
        // Mark the power-on time, just in case it  had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
    } else {
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    // Reset enable pin because pins are set to tri-state on sleep
    if (_RS485EnablePin >= 0) { pinMode(_RS485EnablePin, OUTPUT); }
    // Set the status bit for sensor power attempt (bit 1) and success (bit 2)
    setStatusBits(POWER_ATTEMPTED, POWER_SUCCESSFUL);
}


// This turns off sensor power
void ANBpH::powerDown(void) {
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
        clearStatusBits(POWER_ATTEMPTED, POWER_SUCCESSFUL, WAKE_ATTEMPTED,
                        WAKE_SUCCESSFUL, MEASUREMENT_ATTEMPTED,
                        MEASUREMENT_SUCCESSFUL);
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


bool ANBpH::addSingleMeasurementResult(void) {
    bool success = false;
    // Initialize variables for each value
    float             pH, temp, sal, spcond, raw_cond = -9999;
    ANBHealthCode     health     = ANBHealthCode::UNKNOWN;
    ANBStatusCode     status     = ANBStatusCode::UNKNOWN;
    ANBDiagnosticCode diagnostic = ANBDiagnosticCode::UNKNOWN;

#ifdef MS_ANB_SENSORS_PH_DEBUG_DEEP
    // Print the sensor RTC
    uint16_t seconds = -1;
    uint16_t minutes = -1;
    uint16_t hours   = -1;
    uint16_t day     = -1;
    uint16_t month   = -1;
    uint16_t year    = -1;
    _anb_sensor.getRTC(seconds, minutes, hours, day, month, year);
    char time_buff[16] = {'\0'};
    sprintf(time_buff, "%04d%02d%02d_%02d%02d%02d", year, month, day, hours,
            minutes, seconds);
    MS_DBG(F("    Current RTC value on sensor:"), time_buff);
#endif

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        // Print Moisture Values
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
        MS_DBG(F("  Health Code:"), static_cast<int16_t>(health));
        MS_DBG(F("  Diagnostic Code:"), static_cast<int16_t>(diagnostic));
        MS_DBG(F("  Status Code:"), static_cast<int16_t>(status));

        if (health == ANBHealthCode::NOT_IMMERSED) {
            MS_DBG(F("  WARNING: Sensor is not immersed!"));
        }

        // Put values into the array
        verifyAndAddMeasurementResult(0, pH);
        verifyAndAddMeasurementResult(1, temp);
        verifyAndAddMeasurementResult(2, sal);
        verifyAndAddMeasurementResult(3, spcond);
        verifyAndAddMeasurementResult(4, raw_cond);
        verifyAndAddMeasurementResult(5, static_cast<int16_t>(health));
        verifyAndAddMeasurementResult(6, static_cast<int16_t>(status));
        verifyAndAddMeasurementResult(7, static_cast<int16_t>(diagnostic));
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    clearStatusBits(MEASUREMENT_ATTEMPTED, MEASUREMENT_SUCCESSFUL);

    // Return true when finished
    return success;
}

// check if the sensor is ready
bool ANBpH::isSensorReady(bool (anbSensor::*checkReadyFxn)(),
                          uint32_t spacing) {
    uint32_t elapsed_since_last_request = millis() - _lastModbusCommandTime;
    if (elapsed_since_last_request < spacing) {
        // MS_DEEP_DBG(
        //     F("It's only been"), elapsed_since_last_request,
        //     F("ms since last status check. Wait a bit before checking
        //     again."));
        return false;
    }
    bool ready = (_anb_sensor.*checkReadyFxn)();
    if (ready) {
        // if it's ready, then it's ok to ask it again right away
        _lastModbusCommandTime = 0;
    } else {
        // if the sensor isn't ready, force a wait before checking again
        _lastModbusCommandTime = millis();
    }
    return ready;
}


// This checks to see if enough time has passed for warm-up
bool ANBpH::isWarmedUp(bool debug) {
#if defined(MS_ANB_SENSORS_PH_DEBUG_DEEP)
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
        if (debug) {
            MS_DBG(F("It's been"), elapsed_since_power_on, F("ms, and"),
                   getSensorNameAndLocation(), F("might be warmed up!"));
        }
        bool is_ready = isSensorReady(&anbSensor::gotModbusResponse);
        if (is_ready) {
            MS_DBG(F("It's been"), elapsed_since_power_on, F("ms, and"),
                   getSensorNameAndLocation(),
                   F("got a valid modbus response."));
        } else {
            if (debug) {
                MS_DBG(F("It's been"), elapsed_since_power_on, F("ms, and"),
                       getSensorNameAndLocation(),
                       F("hasn't given a valid modbus response yet."));
            }
        }
        return is_ready;
    } else {
        // wait at least the minimum warm-up time
        return false;
    }
}


// This checks to see if enough time has passed for stability
bool ANBpH::isStable(bool debug) {
#if defined(MS_ANB_SENSORS_PH_DEBUG_DEEP)
    debug = true;
#endif
    // If the sensor failed to activate, it will never stabilize, so the
    // stabilization time is essentially already passed
    if (!getStatusBit(WAKE_SUCCESSFUL)) {
        if (debug) {
            MS_DBG(getSensorNameAndLocation(),
                   F("is not active and cannot stabilize!"));
        }
        return true;
    }

    uint32_t elapsed_since_wake_up = millis() - _millisSensorActivated;
    uint32_t minTime               = _stabilizationTime_ms;
    uint32_t maxTime               = ANB_PH_STABILIZATION_TIME_MAX;
    // If the sensor has been activated and enough time has elapsed, it's stable
    if (elapsed_since_wake_up > maxTime) {
        MS_DBG(F("It's been"), elapsed_since_wake_up, F("ms, and"),
               getSensorNameAndLocation(),
               F("timed out waiting for a valid status code."));
        return true;  // timeout
    } else if (elapsed_since_wake_up > minTime) {
        if (debug) {
            MS_DBG(F("It's been"), elapsed_since_wake_up, F("ms, and"),
                   getSensorNameAndLocation(),
                   F("might be ready to start a measurement."));
        }
        bool is_ready = isSensorReady(&anbSensor::isSensorReady);
        if (is_ready) {
            MS_DBG(F("It's been"), elapsed_since_wake_up, F("ms, and"),
                   getSensorNameAndLocation(),
                   F("gave a valid status code, indicating it's ready to "
                     "start a measurement."));
        } else {
            if (debug) {
                MS_DBG(F("It's been"), elapsed_since_wake_up, F("ms, and"),
                       getSensorNameAndLocation(),
                       F("hasn't given a valid status code yet."));
            }
        }
        return is_ready;
    } else {
        // Wait at least the minimum readiness time
        return false;
    }
}

uint32_t ANBpH::getImmersionErrorTime(void) {
    return _powerPin >= 0 ? ANB_PH_2ND_IMMERSION_ERROR
                          : ANB_PH_1ST_IMMERSION_ERROR;
}

uint32_t ANBpH::getMeasurementTime(void) {
    if (_powerPin >= 0) {
        if (_salinityMode == ANBSalinityMode::HIGH_SALINITY) {
            return ANB_PH_1ST_VALUE_HIGH_SALT;
        } else {
            return ANB_PH_1ST_VALUE_LOW_SALT;
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

    // If no pin was provided for power, we assume it's always powered and use
    // the maximum wait time for the second measurement as our maximum wait.
    // If a pin was provided for power, we assume it's on-demand powered and use
    // the maximum wait time for the first measurement as our maximum wait.
    uint32_t maxWait = getMeasurementTime() +
        ANB_PH_MEASUREMENT_TIME_BUFFER * 2;

    // Since the sensor takes so very long startSlowQuery measure when it's
    // power cycled, if we know it's going to be a while, we drop the query
    // frequency to once every 5 seconds because there's no point in asking more
    // often than that.
    uint32_t startSlowQuery = getImmersionErrorTime() +
        ANB_PH_MEASUREMENT_TIME_BUFFER;
    uint32_t endSlowQuery = getMeasurementTime() -
        ANB_PH_MEASUREMENT_TIME_BUFFER;

    // The minimum time before we start checking for a result depends on whether
    // the immersion sensor is enabled or not and on the power style.
    // If the immersion sensor is enabled, we wait the minimum time that it
    // would return a not-immersed error to start querying. If the immersion
    // sensor is not enabled, we wait the minimum time for a measurement to be
    // ready based on the power style.
    uint32_t minWait = 0;
    if (_immersionSensorEnabled) {
        minWait = getImmersionErrorTime();
    } else {
        minWait = getMeasurementTime() - ANB_PH_MEASUREMENT_TIME_BUFFER;
    }

    uint32_t elapsed_since_meas_start = millis() - _millisMeasurementRequested;
    // if the sensor is always powered and it's past the maximum for the second
    // measurement or it's on-demand powered and it's past the maximum for the
    // first measurement, the measurement failed, but our wait is over
    if (elapsed_since_meas_start > maxWait) {
        MS_DBG(F("It's been"), elapsed_since_meas_start, F("ms, and"),
               getSensorNameAndLocation(),
               F("timed out waiting for a measurement to complete."));
        return true;  // timeout
        // If the immersion sensor is on, start querying after the minimum time
        // that it would return a not-immersed error
    } else if (elapsed_since_meas_start > minWait) {
        if (debug) {
            MS_DBG(F("It's been"), elapsed_since_meas_start, F("ms, and"),
                   getSensorNameAndLocation(),
                   F("might have finished a measurement."));
        }
        bool is_ready = false;
        if (elapsed_since_meas_start < startSlowQuery) {
            is_ready = isSensorReady(&anbSensor::isMeasurementComplete, 500L);
        } else if (elapsed_since_meas_start > startSlowQuery &&
                   elapsed_since_meas_start < endSlowQuery) {
            is_ready = isSensorReady(&anbSensor::isMeasurementComplete, 15000L);
        } else {
            is_ready = isSensorReady(&anbSensor::isMeasurementComplete, 1000L);
        }
        if (is_ready) {
            MS_DBG(F("It's been"), elapsed_since_meas_start, F("ms, and"),
                   getSensorNameAndLocation(),
                   F("says it's finished with a measurement."));
        } else {
            if (debug) {
                MS_DBG(F("It's been"), elapsed_since_meas_start, F("ms, and"),
                       getSensorNameAndLocation(),
                       F("says it's not finished measuring yet."));
            }
        }
        return is_ready;
    } else {
        // If it's started but the minimum measurement time hasn't passed, we
        // need to wait
        return false;
    }
}


bool ANBpH::setSalinityMode(ANBSalinityMode newSalinityMode) {
    _salinityMode = newSalinityMode;
    return _anb_sensor.setSalinityMode(newSalinityMode);
}

bool ANBpH::enableImmersionSensor(bool enable) {
    _immersionSensorEnabled = enable;
    return _anb_sensor.enableImmersionSensor(enable);
}

bool ANBpH::setSensorRTC() {
    // Set the sensor clock to the current time
    MS_DEEP_DBG(F("Attempting to set sensor RTC..."));
    if (loggerClock::isRTCSane()) {
        // Create a temporary variable for the epoch time
        // NOTE: time_t is a typedef for uint32_t, defined in time.h
        time_t t = Logger::getNowLocalEpoch();
        // create a temporary time struct
        // tm is a struct for time parts, defined in time.h
        struct tm* tmp = gmtime(&t);
        MS_DEEP_DBG(F("Time components: "), tmp->tm_year, F(" - "),
                    tmp->tm_mon + 1, F(" - "), tmp->tm_mday, F("    "),
                    tmp->tm_hour, F(" : "), tmp->tm_min, F(" : "), tmp->tm_sec);

        // Set RTC
        // NOTE: The sensor's RTC resets every time the sensor loses power.
        MS_DBG(F("Set sensor RTC..."));
        bool rtcSet = _anb_sensor.setRTC(tmp->tm_sec, tmp->tm_min, tmp->tm_hour,
                                         tmp->tm_mday, tmp->tm_mon + 1,
                                         tmp->tm_year + 1900);
        MS_DBG(F("..."), rtcSet ? F("success") : F("failed"));

        // Print the sensor RTC to cross check
        uint16_t seconds = -1;
        uint16_t minutes = -1;
        uint16_t hours   = -1;
        uint16_t day     = -1;
        uint16_t month   = -1;
        uint16_t year    = -1;
        _anb_sensor.getRTC(seconds, minutes, hours, day, month, year);
#if defined(MS_ANB_SENSORS_PH_DEBUG)
        char time_buff[16] = {'\0'};
        sprintf(time_buff, "%04d%02d%02d_%02d%02d%02d", year, month, day, hours,
                minutes, seconds);
        MS_DBG(F("    RTC after set:"), time_buff);
#endif

        return rtcSet;
    } else {
        MS_DBG(
            F("Current logger time is not sane, so not setting sensor RTC!"));
        return true;
    }
}

// cSpell:ignore spcond
