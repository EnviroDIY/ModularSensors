/**
 * @file FreescaleMPL115A2.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Bobby Schulz <schu3119@umn.edu>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the MPL115A2 sensor subclass and the variable subclasses
 * MPL115A2_Temp and MPL115A2_Pressure.
 *
 * These are used for the Freescale Semiconductor MPL115A2 Miniature I2C Digital
 * Barometer.
 *
 * This depends on the https://github.com/adafruit/Adafruit_MPL115A2 library
 *
 * Documentation for the sensor can be found at:
 * https://www.adafruit.com/product/992 and
 * https://github.com/adafruit/Adafruit-MPL115A2-Breakout-PCB
 *
 * For Pressure:
 *  Resolution is 1.5 hPa
 *  Accuracy ±10 hPa
 *  Range is 500-1150 hPa
 *
 * Sensor takes about 1.6 ms to respond
 * Assume sensor is immediately stable
 */

// Header Guards
#ifndef SRC_SENSORS_FREESCALEMPL115A2_H_
#define SRC_SENSORS_FREESCALEMPL115A2_H_

// Debugging Statement
// #define MS_FREESCALEMPL115A2_DEBUG

#ifdef MS_FREESCALEMPL115A2_DEBUG
#define MS_DEBUGGING_STD "FreescaleMPL115A2"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_MPL115A2.h>

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the MPL115A2 can report 2 values.
#define MPL115A2_NUM_VARIABLES 2
/// Sensor::_warmUpTime_ms; the MPL115A2 warms up in 6ms.
#define MPL115A2_WARM_UP_TIME_MS 6
/// Sensor::_stabilizationTime_ms; the MPL115A2 is stable after 0ms.
#define MPL115A2_STABILIZATION_TIME_MS 0
/// Sensor::_measurementTime_ms; the MPL115A2 takes 3ms to complete a
/// measurement.
#define MPL115A2_MEASUREMENT_TIME_MS 3

/// Decimals places in string representation; temperature should have 2.
#define MPL115A2_TEMP_RESOLUTION 2
/// Variable number; temperature is stored in sensorValues[0].
#define MPL115A2_TEMP_VAR_NUM 0

/// Decimals places in string representation; pressure should have 2.
#define MPL115A2_PRESSURE_RESOLUTION 2
/// Variable number; pressure is stored in sensorValues[1].
#define MPL115A2_PRESSURE_VAR_NUM 1


// The main class for the MPL115A2
class MPL115A2 : public Sensor {
 public:
    explicit MPL115A2(int8_t powerPin, uint8_t measurementsToAverage = 1);
    ~MPL115A2();

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This begins the Wire library (sets pin modes for I2C), reads coefficients
     * from the MPL115A2, and updates the #_sensorStatus.  The sensor must be
     * powered for setup. This doesn't return anything to indicate failure or
     * success, we just have to hope it worked.
     *
     * @return **true** The setup was successful
     * @return **false** Some part of the setup failed
     */
    bool setup(void) override;
    /**
     * @copydoc Sensor::getSensorLocation()
     */
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 protected:
    Adafruit_MPL115A2 mpl115a2_internal;
    uint8_t           _i2cAddressHex;
};


// Defines the Temperature Variable
class MPL115A2_Temp : public Variable {
 public:
    /**
     * @brief Construct a new MPL115A2_Temp object.
     *
     * @param parentSense The parent MPL115A2 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is MPL115A2_Temp.
     */
    explicit MPL115A2_Temp(MPL115A2* parentSense, const char* uuid = "",
                           const char* varCode = "MPL115A2_Temp")
        : Variable(parentSense, (const uint8_t)MPL115A2_TEMP_VAR_NUM,
                   (uint8_t)MPL115A2_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new MPL115A2_Temp object.
     *
     * @note This must be tied with a parent MPL115A2 before it can be used.
     */
    MPL115A2_Temp()
        : Variable((const uint8_t)MPL115A2_TEMP_VAR_NUM,
                   (uint8_t)MPL115A2_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "MPL115A2_Temp") {}
    /**
     * @brief Destroy the MPL115A2_Temp object - no action needed.
     */
    ~MPL115A2_Temp() {}
};


// Defines the Pressure Variable
class MPL115A2_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new MPL115A2_Pressure object.
     *
     * @param parentSense The parent MPL115A2 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is MPL115A2_Pressure.
     */
    explicit MPL115A2_Pressure(MPL115A2* parentSense, const char* uuid = "",
                               const char* varCode = "MPL115A2_Pressure")
        : Variable(parentSense, (const uint8_t)MPL115A2_PRESSURE_VAR_NUM,
                   (uint8_t)MPL115A2_PRESSURE_RESOLUTION, "atmosphericPressure",
                   "kilopascal", varCode, uuid) {}
    /**
     * @brief Construct a new MPL115A2_Pressure object.
     *
     * @note This must be tied with a parent MPL115A2 before it can be used.
     */
    MPL115A2_Pressure()
        : Variable((const uint8_t)MPL115A2_PRESSURE_VAR_NUM,
                   (uint8_t)MPL115A2_PRESSURE_RESOLUTION, "atmosphericPressure",
                   "kilopascal", "MPL115A2_Pressure") {}
    /**
     * @brief Destroy the MPL115A2_Pressure object - no action needed.
     */
    ~MPL115A2_Pressure() {}
};


#endif  // SRC_SENSORS_FREESCALEMPL115A2_H_
