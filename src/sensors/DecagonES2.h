/**
 * @file DecagonES2.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the DecagonES2 subclass of the SDI12Sensors class along with
 * the variable subclasses DecagonES2_Cond and DecagonES2_Temp.
 *
 * These are used for the discontinued Decagon Devices ES-2 Electrical
 * Conductivity Sensor.
 *
 * This depends on the EnviroDIY SDI-12 library and the SDI12Sensors super
 * class.
 *
 * For Specific Conductance:
 *  Resolution is 0.001 mS/cm = 1 µS/cm
 *  Accuracy is ±0.01mS/cm or ±10% (whichever is greater)
 *  Range is 0 – 120 mS/cm (bulk)
 *
 * For Temperature:
 *  Resolution is 0.1°C
 *  Accuracy is ±1°C
 *  Range is -40°C to +50°C
 *
 * Maximum warm-up time in SDI-12 mode: 250ms, assume stability at warm-up
 * Maximum measurement duration: 250ms
 */

// Header Guards
#ifndef SRC_SENSORS_DECAGONES2_H_
#define SRC_SENSORS_DECAGONES2_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the ES2 can report 2 values.
#define ES2_NUM_VARIABLES 2
/// Sensor::_warmUpTime_ms; the ES2 warms up in 250ms.
#define ES2_WARM_UP_TIME_MS 250
/// Sensor::_stabilizationTime_ms; the ES2 is stable after 0ms.
#define ES2_STABILIZATION_TIME_MS 0
/// Sensor::_measurementTime_ms; the ES2 takes 250ms to complete a measurement.
#define ES2_MEASUREMENT_TIME_MS 250

/// Decimals places in string representation; conductivity should have 1.
#define ES2_COND_RESOLUTION 1
// adding extra digit to resolution for averaging
/// Variable number; conductivity is stored in sensorValues[0].
#define ES2_COND_VAR_NUM 0

/// Decimals places in string representation; temperature should have 2.
#define ES2_TEMP_RESOLUTION 2
// adding extra digit to resolution for averaging
/// Variable number; temperature is stored in sensorValues[1].
#define ES2_TEMP_VAR_NUM 1

// The main class for the Decagon ES-2
class DecagonES2 : public SDI12Sensors {
 public:
    // Constructors with overloads
    DecagonES2(char SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "DecagonES2", ES2_NUM_VARIABLES, ES2_WARM_UP_TIME_MS,
                       ES2_STABILIZATION_TIME_MS, ES2_MEASUREMENT_TIME_MS) {}
    DecagonES2(char* SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "DecagonES2", ES2_NUM_VARIABLES, ES2_WARM_UP_TIME_MS,
                       ES2_STABILIZATION_TIME_MS, ES2_MEASUREMENT_TIME_MS) {}
    DecagonES2(int SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "DecagonES2", ES2_NUM_VARIABLES, ES2_WARM_UP_TIME_MS,
                       ES2_STABILIZATION_TIME_MS, ES2_MEASUREMENT_TIME_MS) {}
    // Destructor
    ~DecagonES2() {}
};


// Defines the Conductivity Variable
class DecagonES2_Cond : public Variable {
 public:
    /**
     * @brief Construct a new DecagonES2_Cond object.
     *
     * @param parentSense The parent DecagonES2 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is ES2Cond.
     */
    explicit DecagonES2_Cond(DecagonES2* parentSense, const char* uuid = "",
                             const char* varCode = "ES2Cond")
        : Variable(parentSense, (const uint8_t)ES2_COND_VAR_NUM,
                   (uint8_t)ES2_COND_RESOLUTION, "specificConductance",
                   "microsiemenPerCentimeter", varCode, uuid) {}
    /**
     * @brief Construct a new DecagonES2_Cond object.
     *
     * @note This must be tied with a parent DecagonES2 before it can be used.
     */
    DecagonES2_Cond()
        : Variable((const uint8_t)ES2_COND_VAR_NUM,
                   (uint8_t)ES2_COND_RESOLUTION, "specificConductance",
                   "microsiemenPerCentimeter", "ES2Cond") {}
    /**
     * @brief Destroy the DecagonES2_Cond object - no action needed.
     */
    ~DecagonES2_Cond() {}
};

// Defines the Temperature Variable
class DecagonES2_Temp : public Variable {
 public:
    /**
     * @brief Construct a new DecagonES2_Temp object.
     *
     * @param parentSense The parent DecagonES2 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is ES2Temp.
     */
    explicit DecagonES2_Temp(DecagonES2* parentSense, const char* uuid = "",
                             const char* varCode = "ES2Temp")
        : Variable(parentSense, (const uint8_t)ES2_TEMP_VAR_NUM,
                   (uint8_t)ES2_TEMP_RESOLUTION, "temperature", "degreeCelsius",
                   varCode, uuid) {}
    /**
     * @brief Construct a new DecagonES2_Temp object.
     *
     * @note This must be tied with a parent DecagonES2 before it can be used.
     */
    DecagonES2_Temp()
        : Variable((const uint8_t)ES2_TEMP_VAR_NUM,
                   (uint8_t)ES2_TEMP_RESOLUTION, "temperature", "degreeCelsius",
                   "ES2Temp") {}
    /**
     * @brief Destroy the DecagonES2_Temp object - no action needed.
     */
    ~DecagonES2_Temp() {}
};

#endif  // SRC_SENSORS_DECAGONES2_H_
