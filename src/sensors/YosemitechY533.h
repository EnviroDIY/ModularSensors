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
 */
/* clang-format off */
/**
 * @defgroup y533_group Yosemitech Y533
 * Classes for the Yosemitech Y533 oxidation/reduction potential (ORP) sensor.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section y533_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH_UserManual-v1.0.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH-ORP-v1.7_ModbusInstructions.pdf)
 *
 * @section y533_sensor The y533 Sensor
 * @ctor_doc{YosemitechY533, byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage}
 * @subsection y533_timing Sensor Timing
 * - Time before sensor responds after power - 500ms
 * - Time between "StartMeasurement" command and stable reading - 4.5sec
 *
 * @section y533_ph pH Output
 *   - Range is 2-12 pH
 *   - Accuracy is ±0.1 pH
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.01 pH units
 *   - Reported as dimensionless pH units
 *   - Default variable code is Y533pH
 * @variabledoc{y533_ph,YosemitechY533,pH,Y533pH}
 *
 * @section y533_temp Temperature Output
 *   - Range is 0°C to + 50°C
 *   - Accuracy is ± 0.2°C
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.1 °C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is Y533Temp
 * @variabledoc{y533_temp,YosemitechY533,Temp,Y533Temp}
 *
 * @section y533_volt Voltage Output
 *   - Range is -999 ~ 999 mV
 *   - Accuracy is ± 20 mV
 *   - Result stored in sensorValues[2]
 *   - Resolution is 1 mV
 *   - Reported as millivolts (mV)
 *   - Default variable code is Y533Potential
 * @variabledoc{y533_volt,YosemitechY533,Voltage,Y533Potential}
 *
 * The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 *
 * ___
 * @section y533_examples Example Code
 * The Yosemitech Y533 ORP sensor is used in the @menulink{y533} example.
 *
 * @menusnip{y533}
 */
/* clang-format on */

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
/**
 * @brief Sensor::_measurementTime_ms; the Y533 takes 1800ms to complete a
 * measurement.
 */
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

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y533 ORP sensor](@ref y533_group).
 *
 * @ingroup y533_group
 */
/* clang-format on */
class YosemitechY533 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y533 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)
     * for more information on what streams can be used.
     * @param powerPin The pin on the mcu controlling power to the Y533.
     * Use -1 if it is continuously powered.
     * @param powerPin2 The pin on the mcu controlling power to the RS485
     * adapter, if it is different from that used to power the sensor.  Use -1
     * or omit if not applicable.
     * @param enablePin The pin on the mcu controlling the direction enable on
     * the RS485 adapter, if necessary; use -1 or omit if not applicable.
     * @note An RS485 adapter with integrated flow control is strongly
     * recommended.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    YosemitechY533(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y533,
                           "YosemitechY533", Y533_NUM_VARIABLES,
                           Y533_WARM_UP_TIME_MS, Y533_STABILIZATION_TIME_MS,
                           Y533_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc YosemitechY533::YosemitechY533
     */
    YosemitechY533(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y533,
                           "YosemitechY533", Y533_NUM_VARIABLES,
                           Y533_WARM_UP_TIME_MS, Y533_STABILIZATION_TIME_MS,
                           Y533_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Yosemitech Y533 object
     */
    ~YosemitechY533() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [pH output](@ref y533_ph)
 * from a [Yosemitech Y533 ORP sensor](@ref y533_group).
 *
 * @ingroup y533_group
 */
/* clang-format on */
class YosemitechY533_pH : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY533_pH object.
     *
     * @param parentSense The parent YosemitechY533 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y533pH".
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


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref y533_temp) from a
 * [Yosemitech Y533 ORP sensor](@ref y533_group).
 *
 * @ingroup y533_group
 */
/* clang-format on */
class YosemitechY533_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY533_Temp object.
     *
     * @param parentSense The parent YosemitechY533 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y533Temp".
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


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [electrode electrical potential output](@ref y533_volt) from a
 * [Yosemitech Y533 ORP sensor](@ref y533_group).
 *
 * @ingroup y533_group
 */
/* clang-format on */
class YosemitechY533_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY533_Voltage object.
     *
     * @param parentSense The parent YosemitechY533 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y533Potential".
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
