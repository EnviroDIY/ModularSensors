/**
 * @file YosemitechY533.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY533 sensor subclass and the variable
 * subclasses YosemitechY533_pH, YosemitechY533_Temp, and
 * YosemitechY533_Voltage.
 *
 * These are for the Yosemitech Y533 ORP sensor.
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
 * For pH:
 *     Resolution is 0.01 pH units
 *     Accuracy is ±0.1 pH
 *     Range is 2-12 pH
 *
 * For Temperature:
 *     Resolution is 0.1 °C
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * For ORP/Voltage:
 *     Resolution is 1 mV
 *     Accuracy is ± 20 mV
 *     Range is -999 ~ 999 mV
 *
 * Time before sensor responds after power - 500ms
 * Time between "StartMeasurement" command and stable reading - 4.5sec
 */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY533_H_
#define SRC_SENSORS_YOSEMITECHY533_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the Y533 can report 3 values.
#define Y533_NUM_VARIABLES 3
/// Sensor::_warmUpTime_ms; the Y533 warms up in 500ms.
#define Y533_WARM_UP_TIME_MS 500
/// Sensor::_stabilizationTime_ms; the Y533 is stable after 4500ms.
#define Y533_STABILIZATION_TIME_MS 4500
/// Sensor::_measurementTime_ms; the Y533 takes 1800ms to complete a
/// measurement.
#define Y533_MEASUREMENT_TIME_MS 1800

/// Decimals places in string representation; pH should have 2.
#define Y533_PH_RESOLUTION 2
/// Variable number; pH is stored in sensorValues[0].
#define Y533_PH_VAR_NUM 0

/// Decimals places in string representation; temperature should have 1.
#define Y533_TEMP_RESOLUTION 1
/// Variable number; temperature is stored in sensorValues[1].
#define Y533_TEMP_VAR_NUM 1

/// Decimals places in string representation; voltage should have 0.
#define Y533_VOLT_RESOLUTION 0
/// Variable number; voltage is stored in sensorValues[2].
#define Y533_VOLT_VAR_NUM 2

// The main class for the Decagon Y533
class YosemitechY533 : public YosemitechParent {
 public:
    // Constructors with overloads
    YosemitechY533(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y533,
                           "YosemitechY533", Y533_NUM_VARIABLES,
                           Y533_WARM_UP_TIME_MS, Y533_STABILIZATION_TIME_MS,
                           Y533_MEASUREMENT_TIME_MS) {}
    YosemitechY533(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y533,
                           "YosemitechY533", Y533_NUM_VARIABLES,
                           Y533_WARM_UP_TIME_MS, Y533_STABILIZATION_TIME_MS,
                           Y533_MEASUREMENT_TIME_MS) {}
    ~YosemitechY533() {}
};


// Defines the pH Variable
class YosemitechY533_pH : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY533_pH object.
     *
     * @param parentSense The parent YosemitechY533 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is Y533pH
     */
    explicit YosemitechY533_pH(YosemitechY533* parentSense,
                               const char*     uuid    = "",
                               const char*     varCode = "Y533pH")
        : Variable(parentSense, (const uint8_t)Y533_PH_VAR_NUM,
                   (uint8_t)Y533_PH_RESOLUTION, "pH", "pH", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY533_pH object.
     *
     * @note This must be tied with a parent YosemitechY533 before it can be
     * used.
     */
    YosemitechY533_pH()
        : Variable((const uint8_t)Y533_PH_VAR_NUM, (uint8_t)Y533_PH_RESOLUTION,
                   "pH", "pH", "Y533pH") {}
    /**
     * @brief Destroy the YosemitechY533_pH object - no action needed.
     */
    ~YosemitechY533_pH() {}
};


// Defines the Temperature Variable
class YosemitechY533_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY533_Temp object.
     *
     * @param parentSense The parent YosemitechY533 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is Y533Temp
     */
    explicit YosemitechY533_Temp(YosemitechY533* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y533Temp")
        : Variable(parentSense, (const uint8_t)Y533_TEMP_VAR_NUM,
                   (uint8_t)Y533_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY533_Temp object.
     *
     * @note This must be tied with a parent YosemitechY533 before it can be
     * used.
     */
    YosemitechY533_Temp()
        : Variable((const uint8_t)Y533_TEMP_VAR_NUM,
                   (uint8_t)Y533_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y533Temp") {}
    /**
     * @brief Destroy the YosemitechY533_Temp object - no action needed.
     */
    ~YosemitechY533_Temp() {}
};


// Defines the Electrode Electrical Potential
class YosemitechY533_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY533_Voltage object.
     *
     * @param parentSense The parent YosemitechY533 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is Y533Potential
     */
    explicit YosemitechY533_Voltage(YosemitechY533* parentSense,
                                    const char*     uuid    = "",
                                    const char*     varCode = "Y533Potential")
        : Variable(parentSense, (const uint8_t)Y533_VOLT_VAR_NUM,
                   (uint8_t)Y533_VOLT_RESOLUTION, "voltage", "millivolt",
                   varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY533_Voltage object.
     *
     * @note This must be tied with a parent YosemitechY533 before it can be
     * used.
     */
    YosemitechY533_Voltage()
        : Variable((const uint8_t)Y533_VOLT_VAR_NUM,
                   (uint8_t)Y533_VOLT_RESOLUTION, "voltage", "millivolt",
                   "Y533Potential") {}
    /**
     * @brief Destroy the YosemitechY533_Voltage object - no action needed.
     */
    ~YosemitechY533_Voltage() {}
};

#endif  // SRC_SENSORS_YOSEMITECHY533_H_
