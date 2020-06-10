/**
 * @file TIINA219.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Neil Hancock
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the TIINA219 sensor subclass and the variale subclasses
 * TIINA219_Current, TIINA219_Volt, and TIINA219_Power.
 *
 * These are for the Texas Instruments INA219 current/voltage sensor.
 *
 * This depends on the Adafruit INA219 Library
 *
 * Documentation for the sensor can be found at:
 * https://learn.adafruit.com/adafruit-ina219-current-sensor-breakout and
 * http://www.ti.com/product/INA219
 *
 * For Current Sensor with 0.1Ohms:
 *  Full scale ranges are
 *   +-3.2Amps resolution is 0.8mA
 *   +-0.4Amps resolution is 0.1mA
 *  Absolute Accuracy is range dependent, and approx 2LSB (R accuracy unknown)
 *
 * For Voltage :
 *  Resolution is +-0.001V
 *  Accuracy is ?
 *  Range is 0 to 26V
 *
 * A single conversion takes >532 µs (586 µs typical) at 12 bit resolution
 */

// Header Guards
#ifndef SRC_SENSORS_TIINA219_H_
#define SRC_SENSORS_TIINA219_H_

// Debugging Statement
// #define MS_TIINA219_DEBUG

#ifdef MS_TIINA219_DEBUG
#define MS_DEBUGGING_STD "TIINA219"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_INA219.h>

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the INA219 can report 3 values.
#define INA219_NUM_VARIABLES 3

/// Sensor::_warmUpTime_ms; the INA219 warms up in 100ms.
#define INA219_WARM_UP_TIME_MS 100
/// Sensor::_stabilizationTime_ms; the INA219 is stable after 4000ms.
#define INA219_STABILIZATION_TIME_MS 4000
// 0.5 s for good numbers, but optimal at 4 s based on tests using
// INA219timingTest.ino
/// Sensor::_measurementTime_ms; the INA219 takes 1100ms to complete a
/// measurement.
#define INA219_MEASUREMENT_TIME_MS 1100
// 1.0 s according to datasheet, but slightly better stdev when 1.1 s

/// Decimals places in string representation; current should have 4.
#define INA219_CURRENT_MA_RESOLUTION 4
/// Variable number; current is stored in sensorValues[0].
#define INA219_CURRENT_MA_VAR_NUM 0

/// Decimals places in string representation; bus voltage should have 4.
#define INA219_BUS_VOLTAGE_RESOLUTION 4
/// Variable number; bus voltage is stored in sensorValues[1].
#define INA219_BUS_VOLTAGE_VAR_NUM 1

/// Decimals places in string representation; power draw should have 2.
#define INA219_POWER_MW_RESOLUTION 2
/// Variable number; power draw is stored in sensorValues[2].
#define INA219_POWER_MW_VAR_NUM 2

/// The default address of the INA219
#define INA219_ADDRESS_BASE 0x40

/**
 * @brief The main class for the TexasInstruments INA219
 sensor
 */
class TIINA219 : public Sensor {
 public:
    explicit TIINA219(int8_t  powerPin,
                      uint8_t i2cAddressHex         = INA219_ADDRESS_BASE,
                      uint8_t measurementsToAverage = 1);
    ~TIINA219();

    bool   wake(void) override;
    bool   setup(void) override;
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 protected:
    Adafruit_INA219 ina219_phy;
    uint8_t         _i2cAddressHex;
};


// Defines the Temperature Variable
class TIINA219_Current : public Variable {
 public:
    /**
     * @brief Construct a new TIINA219_Current object.
     *
     * @param parentSense The parent TIINA219 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is TIINA219Amp.
     */
    explicit TIINA219_Current(TIINA219* parentSense, const char* uuid = "",
                              const char* varCode = "TIINA219Amp")
        : Variable(parentSense, (const uint8_t)INA219_CURRENT_MA_VAR_NUM,
                   (uint8_t)INA219_CURRENT_MA_RESOLUTION, "amp", "mA", varCode,
                   uuid) {}
    /**
     * @brief Construct a new TIINA219_Current object.
     *
     * @note This must be tied with a parent TIINA219 before it can be used.
     */
    TIINA219_Current()
        : Variable((const uint8_t)INA219_CURRENT_MA_VAR_NUM,
                   (uint8_t)INA219_CURRENT_MA_RESOLUTION, "amp", "mA",
                   "TIINA219Amp") {}
    /**
     * @brief Destroy the TIINA219_Current object - no action needed.
     */
    ~TIINA219_Current() {}
};


// Defines the Volt Variable
class TIINA219_Volt : public Variable {
 public:
    /**
     * @brief Construct a new TIINA219_Volt object.
     *
     * @param parentSense The parent TIINA219 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is TIINA219Volt.
     */
    explicit TIINA219_Volt(TIINA219* parentSense, const char* uuid = "",
                           const char* varCode = "TIINA219Volt")
        : Variable(parentSense, (const uint8_t)INA219_BUS_VOLTAGE_VAR_NUM,
                   (uint8_t)INA219_BUS_VOLTAGE_RESOLUTION, "Volt", "V", varCode,
                   uuid) {}
    /**
     * @brief Construct a new TIINA219_Volt object.
     *
     * @note This must be tied with a parent TIINA219 before it can be used.
     */
    TIINA219_Volt()
        : Variable((const uint8_t)INA219_BUS_VOLTAGE_VAR_NUM,
                   (uint8_t)INA219_BUS_VOLTAGE_RESOLUTION, "Volt", "V",
                   "TIINA219Volt") {}
    /**
     * @brief Destroy the TIINA219_Volt object - no action needed.
     */
    ~TIINA219_Volt() {}
};


// Defines the Power Variable
class TIINA219_Power : public Variable {
 public:
    /**
     * @brief Construct a new TIINA219_Power object.
     *
     * @param parentSense The parent TIINA219 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is TIINA219Power.
     */
    explicit TIINA219_Power(TIINA219* parentSense, const char* uuid = "",
                            const char* varCode = "TIINA219Power")
        : Variable(parentSense, (const uint8_t)INA219_POWER_MW_VAR_NUM,
                   (uint8_t)INA219_POWER_MW_RESOLUTION, "milliwatt", "mW",
                   varCode, uuid) {}
    /**
     * @brief Construct a new TIINA219_Power object.
     *
     * @note This must be tied with a parent TIINA219 before it can be used.
     */
    TIINA219_Power()
        : Variable((const uint8_t)INA219_POWER_MW_VAR_NUM,
                   (uint8_t)INA219_POWER_MW_RESOLUTION, "milliwatt", "mW",
                   "TIINA219Power") {}
    /**
     * @brief Destroy the TIINA219_Power object - no action needed.
     */
    ~TIINA219_Power() {}
};

#endif  // SRC_SENSORS_TIINA219_H_
