/**
 * @file SensirionSHT4x.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the SensirionSHT4x class.
 */

#include "SensirionSHT4x.h"


// The constructors
SensirionSHT4x::SensirionSHT4x(TwoWire* theI2C, int8_t powerPin, bool useHeater,
                               uint8_t measurementsToAverage)
    : Sensor("SensirionSHT4x", SHT4X_NUM_VARIABLES, SHT4X_WARM_UP_TIME_MS,
             SHT4X_STABILIZATION_TIME_MS, SHT4X_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage),
      _useHeater(useHeater),
      _i2c(theI2C) {}
SensirionSHT4x::SensirionSHT4x(int8_t powerPin, bool useHeater,
                               uint8_t measurementsToAverage)
    : Sensor("SensirionSHT4x", SHT4X_NUM_VARIABLES, SHT4X_WARM_UP_TIME_MS,
             SHT4X_STABILIZATION_TIME_MS, SHT4X_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage, SHT4X_INC_CALC_VARIABLES),
      _useHeater(useHeater),
      _i2c(&Wire) {}
// Destructor
SensirionSHT4x::~SensirionSHT4x() {}


String SensirionSHT4x::getSensorLocation(void) {
    return F("I2C_0x44");
}


bool SensirionSHT4x::setup(void) {
    _i2c->begin();  // Start the wire library (sensor power not required)
    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    _i2c->setTimeout(0);

    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    // The SHT4x's begin() does a soft reset to check for sensor communication.
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    // Run begin fxn because it returns true or false for success in contact
    // Make 5 attempts
    uint8_t ntries  = 0;
    bool    success = false;
    while (!success && ntries < 5) {
        success = sht4x_internal.begin(_i2c);
        ntries++;
    }

    // Set sensor for high precision
    sht4x_internal.setPrecision(SHT4X_HIGH_PRECISION);

    // Initially, set the sensor up to *not* use the heater
    sht4x_internal.setHeater(SHT4X_NO_HEATER);

    // Set status bits
    if (!success) {
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
        // UN-set the set-up bit (bit 0) since setup failed!
        _sensorStatus &= 0b11111110;
    }
    retVal &= success;

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    return retVal;
}


bool SensirionSHT4x::addSingleMeasurementResult(void) {
    // Initialize float variables
    float temp_val  = -9999;
    float humid_val = -9999;
    bool  ret_val   = false;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        // Make sure the heater is *not* going to run.  We want the ambient
        // values.
        sht4x_internal.setHeater(SHT4X_NO_HEATER);

        // we need to create Adafruit "sensor events" to use the library
        sensors_event_t temp_event;
        sensors_event_t humidity_event;
        ret_val = sht4x_internal.getEvent(&humidity_event, &temp_event);

        // get the values from the sensor events
        temp_val  = temp_event.temperature;
        humid_val = humidity_event.relative_humidity;

        if (!ret_val || isnan(temp_val)) temp_val = -9999;
        if (!ret_val || isnan(humid_val)) humid_val = -9999;

        MS_DBG(F("  Temp:"), temp_val, F("°C"));
        MS_DBG(F("  Humidity:"), humid_val, '%');
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(SHT4X_TEMP_VAR_NUM, temp_val);
    verifyAndAddMeasurementResult(SHT4X_HUMIDITY_VAR_NUM, humid_val);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return ret_val;
}


// The function to run the internal heater before going to sleep
bool SensirionSHT4x::sleep(void) {
    if (_useHeater) { return Sensor::sleep(); }

    if (!checkPowerOn()) { return true; }
    if (_millisSensorActivated == 0) {
        MS_DBG(getSensorNameAndLocation(), F("was not measuring!"));
        return true;
    }

    bool success = true;
    MS_DBG(F("Running heater on"), getSensorNameAndLocation(),
           F("at maximum for 1s to remove condensation."));


    // Set up to send a heat command at the highest and longest cycle.
    // Because we're only doing this once per logging cycle (most commonly every
    // 5 minutes, we want to blast the heater for the tiny bit of time it will
    // be on.
    sht4x_internal.setHeater(SHT4X_HIGH_HEATER_1S);

    // we need to create Adafruit "sensor events" to use the library
    // NOTE:  we're not going to use the temperatures and humidity returned
    // here. The SHT4x simply doesn't have any way command to run the heater
    // without measuring.  It's a sensor limitation, not a library limit.  The
    // Adafruit library will always block until the heater turns off - in this
    // case 1 second. Usually blocking steps are a problem, but in this case we
    // need the block because ModularSensors does not currently support a sleep
    // time like it supports a wake time.
    sensors_event_t temp_event;
    sensors_event_t humidity_event;
    success = sht4x_internal.getEvent(&humidity_event, &temp_event);

    // Set the command back to no heat for the next measurement.
    sht4x_internal.setHeater(SHT4X_NO_HEATER);

    if (success) {
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the measurement request time
        _millisMeasurementRequested = 0;
        // Unset the status bits for sensor activation (bits 3 & 4) and
        // measurement request (bits 5 & 6)
        _sensorStatus &= 0b10000111;
        MS_DBG(F("Done"));
    } else {
        MS_DBG(getSensorNameAndLocation(),
               F("did not successfully run the heater."));
    }

    return success;
}
