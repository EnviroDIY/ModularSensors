/**
 * @file YosemitechY510.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY510 sensor subclass and the variable
 * subclasses YosemitechY510_Turbidity and YosemitechY510_Temp.
 *
 * These are for Yosemitech Y510 Turbidity Sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 *
 * These devices output very high "resolution" (32bits) so the resolutions are
 * based on their accuracy, not the resolution of the sensor.
 *
 * For Turbidity:
 *     Accuracy is ± 5 % or 0.3 NTU
 *     Range is 0.1 to 1000 NTU
 *
 * For Temperature:
 *     Resolution is 0.1 °C
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 500ms
 * Time between "StartMeasurement" command and stable reading - 22sec
 */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY510_H_
#define SRC_SENSORS_YOSEMITECHY510_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the Y510 can report 2 values.
#define Y510_NUM_VARIABLES 2
/// Sensor::_warmUpTime_ms; the Y510 warms up in 500ms.
#define Y510_WARM_UP_TIME_MS 500
/// Sensor::_stabilizationTime_ms; the Y510 is stable after 22000ms.
#define Y510_STABILIZATION_TIME_MS 22000
/// Sensor::_measurementTime_ms; the Y510 takes 1700ms to complete a
/// measurement.
#define Y510_MEASUREMENT_TIME_MS 1700

/// Decimals places in string representation; turbidity should have 2.
#define Y510_TURB_RESOLUTION 2
/// Variable number; turbidity is stored in sensorValues[0].
#define Y510_TURB_VAR_NUM 0

/// Decimals places in string representation; temperature should have 1.
#define Y510_TEMP_RESOLUTION 1
/// Variable number; temperature is stored in sensorValues[1].
#define Y510_TEMP_VAR_NUM 1

// The main class for the Decagon Y510
class YosemitechY510 : public YosemitechParent {
 public:
    // Constructors with overloads
    YosemitechY510(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y510,
                           "YosemitechY510", Y510_NUM_VARIABLES,
                           Y510_WARM_UP_TIME_MS, Y510_STABILIZATION_TIME_MS,
                           Y510_MEASUREMENT_TIME_MS) {}
    YosemitechY510(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y510,
                           "YosemitechY510", Y510_NUM_VARIABLES,
                           Y510_WARM_UP_TIME_MS, Y510_STABILIZATION_TIME_MS,
                           Y510_MEASUREMENT_TIME_MS) {}
    ~YosemitechY510() {}
};


// Defines the Turbidity
class YosemitechY510_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY510_Turbidity object.
     *
     * @param parentSense The parent YosemitechY510 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is Y510Turbidity
     */
    explicit YosemitechY510_Turbidity(YosemitechY510* parentSense,
                                      const char*     uuid    = "",
                                      const char*     varCode = "Y510Turbidity")
        : Variable(parentSense, (const uint8_t)Y510_TURB_VAR_NUM,
                   (uint8_t)Y510_TURB_RESOLUTION, "turbidity",
                   "nephelometricTurbidityUnit", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY510_Turbidity object.
     *
     * @note This must be tied with a parent YosemitechY510 before it can be
     * used.
     */
    YosemitechY510_Turbidity()
        : Variable((const uint8_t)Y510_TURB_VAR_NUM,
                   (uint8_t)Y510_TURB_RESOLUTION, "turbidity",
                   "nephelometricTurbidityUnit", "Y510Turbidity") {}
    /**
     * @brief Destroy the YosemitechY510_Turbidity object - no action needed.
     */
    ~YosemitechY510_Turbidity() {}
};


// Defines the Temperature Variable
class YosemitechY510_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY510_Temp object.
     *
     * @param parentSense The parent YosemitechY510 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is Y510Temp
     */
    explicit YosemitechY510_Temp(YosemitechY510* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y510Temp")
        : Variable(parentSense, (const uint8_t)Y510_TEMP_VAR_NUM,
                   (uint8_t)Y510_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY510_Temp object.
     *
     * @note This must be tied with a parent YosemitechY510 before it can be
     * used.
     */
    YosemitechY510_Temp()
        : Variable((const uint8_t)Y510_TEMP_VAR_NUM,
                   (uint8_t)Y510_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y510Temp") {}
    /**
     * @brief Destroy the YosemitechY510_Temp object - no action needed.
     */
    ~YosemitechY510_Temp() {}
};

#endif  // SRC_SENSORS_YOSEMITECHY510_H_
