/**
 * @file YosemitechY520.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY520 sensor subclass and the variable
 * subclasses YosemitechY520_Cond and YosemitechY520_Temp.
 *
 * These are for the Yosemitech Y520 4-pole Conductivity sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup y520_group Yosemitech Y520 Conductivity %Sensor
 * Classes for the @ref y520_page
 *
 * @ingroup yosemitech_group
 *
 * @copydoc y520_page
 */
/* clang-format on */
/* clang-format off */
/**
 * @page y520_page Yosemitech Y520 Conductivity %Sensor
 *
 * @tableofcontents
 *
 * @section y520_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y520-Conductivity_UserManual-v1.1.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y520-Conductivity-v1.8_ModbusInstructions.pdf)
 *
 * @section y520_sensor The y520 Sensor
 * @ctor_doc{YosemitechY520, byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage}
 * @subsection y520_timing Sensor Timing
 * - Time before sensor responds after power - 1200 ms
 * - Time between "StartMeasurement" command and stable reading - 10sec
 *
 * @section y520_cond Conductivity Output
 * @variabledoc{YosemitechY520,Cond}
 *   - Range is 1 µS/cm to 200 mS/cm
 *   - Accuracy is ± 1 % Full Scale
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.1 µS/cm
 *   - Reported as microsiemens per centimeter (µS/cm)
 *   - Default variable code is Y520Cond
 *
 * @section y520_temp Temperature Output
 * @variabledoc{YosemitechY520,Temp}
 *   - Range is 0°C to + 50°C
 *   - Accuracy is ± 0.2°C
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.1 °C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is Y520Temp
 *
 * The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 *
 * ___
 * @section y520_examples Example Code
 * The Yosemitech Y520 conductivity sensor is used in the @menulink{y520}
 * example.
 *
 * @menusnip{y520}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY520_H_
#define SRC_SENSORS_YOSEMITECHY520_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the Y520 can report 2 values.
#define Y520_NUM_VARIABLES 2
/// Sensor::_warmUpTime_ms; the Y520 warms up in 1600ms.
#define Y520_WARM_UP_TIME_MS 1600
/// Sensor::_stabilizationTime_ms; the Y520 is stable after 10000ms.
#define Y520_STABILIZATION_TIME_MS 10000
/**
 * @brief Sensor::_measurementTime_ms; the Y520 takes 2700ms to complete a
 * measurement.
 */
#define Y520_MEASUREMENT_TIME_MS 2700

/// Decimals places in string representation; conductivity should have 1.
#define Y520_COND_RESOLUTION 1
/// Variable number; conductivity is stored in sensorValues[0].
#define Y520_COND_VAR_NUM 0

/// Decimals places in string representation; temperature should have 1.
#define Y520_TEMP_RESOLUTION 1
/// Variable number; temperature is stored in sensorValues[1].
#define Y520_TEMP_VAR_NUM 1

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y520-A 4-electrode conductivity sensor](@ref y520_page).
 *
 * @ingroup y520_group
 */
/* clang-format on */
class YosemitechY520 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y520 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)
     * for more information on what streams can be used.
     * @param powerPin The pin on the mcu controlling power to the Y520.  Use -1
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
    YosemitechY520(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y520,
                           "YosemitechY520", Y520_NUM_VARIABLES,
                           Y520_WARM_UP_TIME_MS, Y520_STABILIZATION_TIME_MS,
                           Y520_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc YosemitechY520::YosemitechY520
     */
    YosemitechY520(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y520,
                           "YosemitechY520", Y520_NUM_VARIABLES,
                           Y520_WARM_UP_TIME_MS, Y520_STABILIZATION_TIME_MS,
                           Y520_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Yosemitech Y520 object
     */
    ~YosemitechY520() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [conductivity output](@ref y520_cond) from a
 * [Yosemitech Y520-A 4-electrode conductivity sensor](@ref y520_page).
 *
 * @ingroup y520_group
 */
/* clang-format on */
class YosemitechY520_Cond : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY520_Cond object.
     *
     * @param parentSense The parent YosemitechY520 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of Y520Cond
     */
    explicit YosemitechY520_Cond(YosemitechY520* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y520Cond")
        : Variable(parentSense, (const uint8_t)Y520_COND_VAR_NUM,
                   (uint8_t)Y520_COND_RESOLUTION, "specificConductance",
                   "microsiemenPerCentimeter", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY520_Cond object.
     *
     * @note This must be tied with a parent YosemitechY520 before it can be
     * used.
     */
    YosemitechY520_Cond()
        : Variable((const uint8_t)Y520_COND_VAR_NUM,
                   (uint8_t)Y520_COND_RESOLUTION, "specificConductance",
                   "microsiemenPerCentimeter", "Y520Cond") {}
    /**
     * @brief Destroy the YosemitechY520_Cond object - no action needed.
     */
    ~YosemitechY520_Cond() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref y520_temp) from a
 * [Yosemitech Y520-A 4-electrode conductivity sensor](@ref y520_page).
 *
 * @ingroup y520_group
 */
/* clang-format on */
class YosemitechY520_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY520_Temp object.
     *
     * @param parentSense The parent YosemitechY520 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of Y520Temp
     */
    explicit YosemitechY520_Temp(YosemitechY520* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y520Temp")
        : Variable(parentSense, (const uint8_t)Y520_TEMP_VAR_NUM,
                   (uint8_t)Y520_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY520_Temp object.
     *
     * @note This must be tied with a parent YosemitechY520 before it can be
     * used.
     */
    YosemitechY520_Temp()
        : Variable((const uint8_t)Y520_TEMP_VAR_NUM,
                   (uint8_t)Y520_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y520Temp") {}
    /**
     * @brief Destroy the YosemitechY520_Temp object - no action needed.
     */
    ~YosemitechY520_Temp() {}
};

#endif  // SRC_SENSORS_YOSEMITECHY520_H_
