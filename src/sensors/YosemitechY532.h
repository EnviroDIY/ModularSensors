/**
 * @file YosemitechY532.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY532 sensor subclass and the variable
 * subclasses YosemitechY532_pH, YosemitechY532_Temp, and
 * YosemitechY532_Voltage.
 *
 * These are for the Yosemitech Y532 pH sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup y532_group Yosemitech Y532
 * Classes for the Yosemitech Y532 pH sensor.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section y532_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH_UserManual-v1.0.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH-ORP-v1.7_ModbusInstructions.pdf)
 *
 * @section y532_sensor The y532 Sensor
 * @ctor_doc{YosemitechY532, byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage}
 * @subsection y532_timing Sensor Timing
 * - Time before sensor responds after power - 500ms
 * - Time between "StartMeasurement" command and stable reading - 4.5sec
 *
 * @section y532_ph pH Output
 *   - Range is 2 to 12 pH units
 *   - Accuracy is ± 0.1 pH units
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.01 pH units
 *   - Reported as dimensionless pH units
 *   - Default variable code is Y532pH
 * @variabledoc{y532_ph,YosemitechY532,pH,Y532pH}
 *
 * @section y532_temp Temperature Output
 *   - Range is 0°C to + 50°C
 *   - Accuracy is ± 0.2°C
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.1 °C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is Y532Temp
 * @variabledoc{y532_temp,YosemitechY532,Temp,Y532Temp}
 *
 * @section y532_volt Voltage Output
 *   - Range is -999 ~ 999 mV
 *   - Accuracy is ± 20 mV
 *   - Result stored in sensorValues[2]
 *   - Resolution is 1mV
 *   - Reported as millivolts (mV)
 *   - Default variable code is Y532Potential
 * @variabledoc{y532_volt,YosemitechY532,Voltage,Y532Potential}
 *
 * The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 *
 * ___
 * @section y532_examples Example Code
 * The Yosemitech Y532 pH sensor is used in the @menulink{y532} example.
 *
 * @menusnip{y532}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY532_H_
#define SRC_SENSORS_YOSEMITECHY532_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the Y532 can report 3 values.
#define Y532_NUM_VARIABLES 3
/// Sensor::_warmUpTime_ms; the Y532 warms up in 500ms.
#define Y532_WARM_UP_TIME_MS 500
/// Sensor::_stabilizationTime_ms; the Y532 is stable after 4500ms.
#define Y532_STABILIZATION_TIME_MS 4500
/**
 * @brief Sensor::_measurementTime_ms; the Y532 takes 1800ms to complete a
 * measurement.
 */
#define Y532_MEASUREMENT_TIME_MS 1800

/// Decimals places in string representation; pH should have 2.
#define Y532_PH_RESOLUTION 2
/// Variable number; pH is stored in sensorValues[0].
#define Y532_PH_VAR_NUM 0

/// Decimals places in string representation; temperature should have 1.
#define Y532_TEMP_RESOLUTION 1
/// Variable number; temperature is stored in sensorValues[1].
#define Y532_TEMP_VAR_NUM 1

/// Decimals places in string representation; voltage should have 0.
#define Y532_VOLT_RESOLUTION 0
/// Variable number; voltage is stored in sensorValues[2].
#define Y532_VOLT_VAR_NUM 2

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y532-A digital pH sensor](@ref y532_group).
 *
 * @ingroup y532_group
 */
/* clang-format on */
class YosemitechY532 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y532 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)
     * for more information on what streams can be used.
     * @param powerPin The pin on the mcu controlling power to the Y532.  Use -1
     * if it is continuously powered.
     * @param powerPin2 The pin on the mcu controlling power to the RS485
     * adapter, if it is different from that used to power the sensor.  Use -1
     * or omit if not applicable.
     * @param enablePin The pin on the mcu controlling the direction enable on
     * the RS485 adapter, if necessary; use -1 or omit if not applicable.  An
     * RS485 adapter with integrated flow control is strongly recommended.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    YosemitechY532(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y532,
                           "YosemitechY532", Y532_NUM_VARIABLES,
                           Y532_WARM_UP_TIME_MS, Y532_STABILIZATION_TIME_MS,
                           Y532_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc YosemitechY532::YosemitechY532
     */
    YosemitechY532(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y532,
                           "YosemitechY532", Y532_NUM_VARIABLES,
                           Y532_WARM_UP_TIME_MS, Y532_STABILIZATION_TIME_MS,
                           Y532_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Yosemitech Y532 object
     */
    ~YosemitechY532() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [pH output](@ref y532_ph)
 * from a  [Yosemitech Y532-A digital pH sensor](@ref y532_group).
 *
 * @ingroup y532_group
 */
/* clang-format on */
class YosemitechY532_pH : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY532_pH object.
     *
     * @param parentSense The parent YosemitechY532 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of Y532pH
     */
    explicit YosemitechY532_pH(YosemitechY532* parentSense,
                               const char*     uuid    = "",
                               const char*     varCode = "Y532pH")
        : Variable(parentSense, (const uint8_t)Y532_PH_VAR_NUM,
                   (uint8_t)Y532_PH_RESOLUTION, "pH", "pH", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY532_pH object.
     *
     * @note This must be tied with a parent YosemitechY532 before it can be
     * used.
     */
    YosemitechY532_pH()
        : Variable((const uint8_t)Y532_PH_VAR_NUM, (uint8_t)Y532_PH_RESOLUTION,
                   "pH", "pH", "Y532pH") {}
    /**
     * @brief Destroy the YosemitechY532_pH object - no action needed.
     */
    ~YosemitechY532_pH() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref y532_temp) from a
 * [Yosemitech Y532-A digital pH sensor](@ref y532_group).
 *
 * @ingroup y532_group
 */
/* clang-format on */
class YosemitechY532_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY532_Temp object.
     *
     * @param parentSense The parent YosemitechY532 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y532Temp".
     */
    explicit YosemitechY532_Temp(YosemitechY532* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y532Temp")
        : Variable(parentSense, (const uint8_t)Y532_TEMP_VAR_NUM,
                   (uint8_t)Y532_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY532_Temp object.
     *
     * @note This must be tied with a parent YosemitechY532 before it can be
     * used.
     */
    YosemitechY532_Temp()
        : Variable((const uint8_t)Y532_TEMP_VAR_NUM,
                   (uint8_t)Y532_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y532Temp") {}
    /**
     * @brief Destroy the YosemitechY532_Temp object - no action needed.
     */
    ~YosemitechY532_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [electrode electrical potential output](@ref y532_volt) from a
 * [Yosemitech Y532-A digital pH sensor](@ref y532_group).
 *
 * @ingroup y532_group
 */
/* clang-format on */
class YosemitechY532_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY532_Voltage object.
     *
     * @param parentSense The parent YosemitechY532 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y532Potential".
     */
    explicit YosemitechY532_Voltage(YosemitechY532* parentSense,
                                    const char*     uuid    = "",
                                    const char*     varCode = "Y532Potential")
        : Variable(parentSense, (const uint8_t)Y532_VOLT_VAR_NUM,
                   (uint8_t)Y532_VOLT_RESOLUTION, "voltage", "millivolt",
                   varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY532_Voltage object.
     *
     * @note This must be tied with a parent YosemitechY532 before it can be
     * used.
     */
    YosemitechY532_Voltage()
        : Variable((const uint8_t)Y532_VOLT_VAR_NUM,
                   (uint8_t)Y532_VOLT_RESOLUTION, "voltage", "millivolt",
                   "Y532Potential") {}
    /**
     * @brief Destroy the YosemitechY532_Voltage object - no action needed.
     */
    ~YosemitechY532_Voltage() {}
};

#endif  // SRC_SENSORS_YOSEMITECHY532_H_
