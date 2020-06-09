/**
 * @file MaximDS3231.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the MaximDS3231 sensor subclass and the MaximDS3231_Temp
 * variable subclass.
 *
 * These are for the DS3231 real time clock which is required for all AVR
 * boards.
 *
 * This depends on the EnviroDIY DS3231 library.
 *
 * For temperature from the DS3231:
 *  Resolution is 0.25°C
 *  Accuracy is ±3°C
 *  Range is 0°C to +70°C
 *
 * The clock should have a separate power supply and never be turned off.
 * We assume it's always warmed up and stable.
 * The temperature conversion typically takes 125 ms, with a max time of 200 ms.
 */

// Header Guards
#ifndef SRC_SENSORS_MAXIMDS3231_H_
#define SRC_SENSORS_MAXIMDS3231_H_

// Debugging Statement
// #define MS_MAXIMDS3231_DEBUG

#ifdef MS_MAXIMDS3231_DEBUG
#define MS_DEBUGGING_STD "MaximDS3231"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the DS3231 can report 1 value.
#define DS3231_NUM_VARIABLES 1
/// Sensor::_warmUpTime_ms; the DS3231 should never be powered off so there is
/// no warm-up time.
#define DS3231_WARM_UP_TIME_MS 0
/// Sensor::_stabilizationTime_ms; the DS3231 should never be powered off so
/// there is no stabilization time.
#define DS3231_STABILIZATION_TIME_MS 0
/// Sensor::_measurementTime_ms; the DS3231 takes 200ms to complete a
/// measurement.
#define DS3231_MEASUREMENT_TIME_MS 200

/// Decimals places in string representation; temperature should have 2.
#define DS3231_TEMP_RESOLUTION 2
/// Variable number; temperature is stored in sensorValues[0].
#define DS3231_TEMP_VAR_NUM 0


// The "Main" class for the DS3231
// Only need a sleep and wake since these DON'T use the default of powering up
// and down
class MaximDS3231 : public Sensor {
 public:
    // Only input is the number of readings to average
    explicit MaximDS3231(uint8_t measurementsToAverage = 1);
    // Destructor
    ~MaximDS3231();

    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This starts communication with the RTC (if it had not already been
     * started).  This will also turn off any interrupts currently scheduled on
     * the RTC.  The clock should be continuously powered, so we never need to
     * worry about power up.
     *
     * @return **true** The setup was successful
     * @return **false** Some part of the setup failed
     */
    bool setup(void) override;

    bool startSingleMeasurement(void) override;
    bool addSingleMeasurementResult(void) override;
};


class MaximDS3231_Temp : public Variable {
 public:
    /**
     * @brief Construct a new MaximDS3231_Temp object.
     *
     * @param parentSense The parent MaximDS3231 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is BoardTemp.
     */
    explicit MaximDS3231_Temp(MaximDS3231* parentSense, const char* uuid = "",
                              const char* varCode = "BoardTemp")
        : Variable(parentSense, (const uint8_t)DS3231_TEMP_VAR_NUM,
                   (uint8_t)DS3231_TEMP_RESOLUTION, "temperatureDatalogger",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new MaximDS3231_Temp object.
     *
     * @note This must be tied with a parent MaximDS3231 before it can be used.
     */
    MaximDS3231_Temp()
        : Variable((const uint8_t)DS3231_TEMP_VAR_NUM,
                   (uint8_t)DS3231_TEMP_RESOLUTION, "temperatureDatalogger",
                   "degreeCelsius", "BoardTemp") {}
    /**
     * @brief Destroy the MaximDS3231_Temp object - no action needed.
     */
    ~MaximDS3231_Temp() {}
};

#endif  // SRC_SENSORS_MAXIMDS3231_H_
