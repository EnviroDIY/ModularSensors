/**
 * @file RainCounterI2C.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Bobby Schulz <schu3119@umn.edu>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the RainCounterI2C sensor subclass and the variable
 * subclasses RainCounterI2C_Tips and RainCounterI2C_Depth.
 *
 * These are for an external tip counter, used to measure rainfall via a tipping
 * bucket rain gauge
 *
 * The tip counter works on an Adafruit Trinket.  Documentation for it can be
 * found at: https://github.com/EnviroDIY/TippingBucketRainCounter
 *
 * For Rainfall:
 *  Accuracy and resolution are dependent on the sensor used
 *  Standard resolution is 0.01" or 0.2mm of rainfall (depending on if sensor is
 * set to english or metric)
 *
 * Assume sensor is immediately stable
 */

// Header Guards
#ifndef SRC_SENSORS_RAINCOUNTERI2C_H_
#define SRC_SENSORS_RAINCOUNTERI2C_H_

// Debugging Statement
// #define MS_RAINCOUNTERI2C_DEBUG

#ifdef MS_RAINCOUNTERI2C_DEBUG
#define MS_DEBUGGING_STD "RainCounterI2C"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the tipping bucket counter can report 2 values.
#define BUCKET_NUM_VARIABLES 2
/// Sensor::_warmUpTime_ms; the tipping bucket counter warms up in 0ms.
#define BUCKET_WARM_UP_TIME_MS 0
/// Sensor::_stabilizationTime_ms; the tipping bucket counter is stable after
/// 0ms.
#define BUCKET_STABILIZATION_TIME_MS 0
/// Sensor::_measurementTime_ms; the tipping bucket counter takes 0ms to
/// complete a measurement.
#define BUCKET_MEASUREMENT_TIME_MS 0

/// Decimals places in string representation; rain depth should have 2.
#define BUCKET_RAIN_RESOLUTION 2
/// Variable number; rain depth is stored in sensorValues[0].
#define BUCKET_RAIN_VAR_NUM 0

/// Decimals places in string representation; the number of tips should have 0.
#define BUCKET_TIPS_RESOLUTION 0
/// Variable number; the number of tips is stored in sensorValues[1].
#define BUCKET_TIPS_VAR_NUM 1

// The main class for the external tipping bucket counter
class RainCounterI2C : public Sensor {
 public:
    // The constructor - all arguments are optional
    // Address of I2C device is 0x08 by default
    // Depth of rain per tip event in mm is 0.2mm by default
    explicit RainCounterI2C(uint8_t i2cAddressHex = 0x08,
                            float   rainPerTip    = 0.2);
    // Destructor
    ~RainCounterI2C();

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets the #_powerPin mode, begins the Wire library (sets pin levels
     * and modes for I2C), and updates the #_sensorStatus.  No sensor power is
     * required.
     *
     * @return **true** The setup was successful
     * @return **false** Some part of the setup failed
     */
    bool   setup(void) override;
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 protected:
    float   _rainPerTip;
    uint8_t _i2cAddressHex;
};

// Defines the tip varible, shows the number of tips since last read
class RainCounterI2C_Tips : public Variable {
 public:
    /**
     * @brief Construct a new RainCounterI2C_Tips object.
     *
     * @param parentSense The parent RainCounterI2C providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is RainCounterI2CTips
     */
    explicit RainCounterI2C_Tips(RainCounterI2C* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "RainCounterI2CTips")
        : Variable(parentSense, (const uint8_t)BUCKET_TIPS_VAR_NUM,
                   (uint8_t)BUCKET_TIPS_RESOLUTION, "precipitation", "event",
                   varCode, uuid) {}
    /**
     * @brief Construct a new RainCounterI2C_Tips object.
     *
     * @note This must be tied with a parent RainCounterI2C before it can be
     * used.
     */
    RainCounterI2C_Tips()
        : Variable((const uint8_t)BUCKET_TIPS_VAR_NUM,
                   (uint8_t)BUCKET_TIPS_RESOLUTION, "precipitation", "event",
                   "RainCounterI2CTips") {}
    /**
     * @brief Destroy the RainCounterI2C_Tips object - no action needed.
     */
    ~RainCounterI2C_Tips() {}
};

// Defines the depth of rain variable, shows the number of mm since the last
// read
class RainCounterI2C_Depth : public Variable {
 public:
    /**
     * @brief Construct a new RainCounterI2C_Depth object.
     *
     * @param parentSense The parent RainCounterI2C providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is RainCounterI2CVol
     */
    explicit RainCounterI2C_Depth(RainCounterI2C* parentSense,
                                  const char*     uuid    = "",
                                  const char*     varCode = "RainCounterI2CVol")
        : Variable(parentSense, (const uint8_t)BUCKET_RAIN_VAR_NUM,
                   (uint8_t)BUCKET_RAIN_RESOLUTION, "precipitation",
                   "millimeter", varCode, uuid) {}
    /**
     * @brief Construct a new RainCounterI2C_Depth object.
     *
     * @note This must be tied with a parent RainCounterI2C before it can be
     * used.
     */
    RainCounterI2C_Depth()
        : Variable((const uint8_t)BUCKET_RAIN_VAR_NUM,
                   (uint8_t)BUCKET_RAIN_RESOLUTION, "precipitation",
                   "millimeter", "RainCounterI2CVol") {}
    /**
     * @brief Destroy the RainCounterI2C_Depth object - no action needed.
     */
    ~RainCounterI2C_Depth() {}
};


#endif  // SRC_SENSORS_RAINCOUNTERI2C_H_
