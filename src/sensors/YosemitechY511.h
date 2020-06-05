/**
 * @file YosemitechY511.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY511 sensor subclass and the variable
 * subclasses YosemitechY511_Turbidity and YosemitechY511_Temp.
 *
 * These are for the Yosemitech Y511 Turbidity Sensor with Wiper.
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
#ifndef SRC_SENSORS_YOSEMITECHY511_H_
#define SRC_SENSORS_YOSEMITECHY511_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the Y511 can report 2 values.
#define Y511_NUM_VARIABLES 2
/**
 * @brief Sensor::_warmUpTime_ms; Y511 warms up in 8000ms.
 *
 * 500 ms to receive commands, but if activating brush warmup+stabilization must
 * >20s
 */
#define Y511_WARM_UP_TIME_MS 8000
/**
 * @brief Sensor::_stabilizationTime_ms; Y511 is stable after 40s.
 *
 * warmup+stabilization > 48 s for consecutive readings to give different
 * results
 */
#define Y511_STABILIZATION_TIME_MS 40000
/**
 * @brief Sensor::_measurementTime_ms; Y511 take 4s to complete a measurement.
 *
 * Could potentially be lower with a longer stabilization time; more testing
 * needed.
 */
#define Y511_MEASUREMENT_TIME_MS 4000

/// Decimals places in string representation; turbidity should have 2.
#define Y511_TURB_RESOLUTION 2
/// Variable number; turbidity is stored in sensorValues[0].
#define Y511_TURB_VAR_NUM 0

/// Decimals places in string representation; temperature should have 1.
#define Y511_TEMP_RESOLUTION 1
/// Variable number; temperature is stored in sensorValues[1].
#define Y511_TEMP_VAR_NUM 1

// The main class for the Decagon Y511
class YosemitechY511 : public YosemitechParent {
 public:
    // Constructors with overloads
    YosemitechY511(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y511,
                           "YosemitechY511", Y511_NUM_VARIABLES,
                           Y511_WARM_UP_TIME_MS, Y511_STABILIZATION_TIME_MS,
                           Y511_MEASUREMENT_TIME_MS) {}
    YosemitechY511(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y511,
                           "YosemitechY511", Y511_NUM_VARIABLES,
                           Y511_WARM_UP_TIME_MS, Y511_STABILIZATION_TIME_MS,
                           Y511_MEASUREMENT_TIME_MS) {}
    ~YosemitechY511() {}
};


// Defines the Turbidity
class YosemitechY511_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY511_Turbidity object.
     *
     * @param parentSense The parent YosemitechY511 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is Y511Turbidity
     */
    explicit YosemitechY511_Turbidity(YosemitechY511* parentSense,
                                      const char*     uuid    = "",
                                      const char*     varCode = "Y511Turbidity")
        : Variable(parentSense, (const uint8_t)Y511_TURB_VAR_NUM,
                   (uint8_t)Y511_TURB_RESOLUTION, "turbidity",
                   "nephelometricTurbidityUnit", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY511_Turbidity object.
     *
     * @note This must be tied with a parent YosemitechY511 before it can be
     * used.
     */
    YosemitechY511_Turbidity()
        : Variable((const uint8_t)Y511_TURB_VAR_NUM,
                   (uint8_t)Y511_TURB_RESOLUTION, "turbidity",
                   "nephelometricTurbidityUnit", "Y511Turbidity") {}
    /**
     * @brief Destroy the YosemitechY511_Turbidity object - no action needed.
     */
    ~YosemitechY511_Turbidity() {}
};


// Defines the Temperature Variable
class YosemitechY511_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY511_Temp object.
     *
     * @param parentSense The parent YosemitechY511 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is Y511Temp
     */
    explicit YosemitechY511_Temp(YosemitechY511* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y511Temp")
        : Variable(parentSense, (const uint8_t)Y511_TEMP_VAR_NUM,
                   (uint8_t)Y511_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY511_Temp object.
     *
     * @note This must be tied with a parent YosemitechY511 before it can be
     * used.
     */
    YosemitechY511_Temp()
        : Variable((const uint8_t)Y511_TEMP_VAR_NUM,
                   (uint8_t)Y511_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y511Temp") {}
    /**
     * @brief Destroy the YosemitechY511_Temp object - no action needed.
     */
    ~YosemitechY511_Temp() {}
};

#endif  // SRC_SENSORS_YOSEMITECHY511_H_
