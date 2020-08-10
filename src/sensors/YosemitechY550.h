/**
 * @file YosemitechY550.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY550 sensor subclass and the variable
 * subclasses YosemitechY550_COD, YosemitechY550_Temp, and
 * YosemitechY550_Turbidity.
 *
 * These are for the Yosemitech Y550 COD Sensor with Wiper.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 *
 * @defgroup y550_group Yosemitech Y550 UV245/COD %Sensor
 * Classes for the @ref y550_page
 *
 * @copydoc y550_page
 *
 * @ingroup yosemitech_group
 */
/* clang-format off */
/**
 * @page y550_page Yosemitech Y550 UV245/COD %Sensor
 *
 * @tableofcontents
 *
 * @section y550_datasheet Sensor Datasheet
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y550-COD-UV254-1.5_ModbusInstruction-en.pdf)
 *
 * @section y550_sensor The Yosemitech Y550 UV245/COD Sensor
 * @ctor_doc{YosemitechY550, byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage}
 * @subsection y550_timing Sensor Timing
 * - Time before sensor responds after power - 500ms
 * - Time between "StartMeasurement" command and stable reading - 22sec
 *
 * @section y550_cod COD Output
 * @variabledoc{YosemitechY550,COD}
 *   - Range is:
 *     - 0.75 to 370 mg/L COD (equiv. KHP)
 *     - 0.2 to 150 mg/L TOC (equiv. KHP)
 *   - Accuracy is not reported on sensor datasheet
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.01 mg/L COD
 *   - Reported as milligrams per liter (mg/L)
 *   - Default variable code is Y550COD
 *
 * @section y550_temp Temperature Output
 * @variabledoc{YosemitechY550,Temp}
 *   - Range is 5°C to + 45°C
 *   - Accuracy is ± 0.2°C
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.1 °C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is Y550Temp
 *
 * @section y550_turb Turbidity Output
 * @variabledoc{YosemitechY550,Turbidity}
 *   - Range is 0.1~1000 NTU
 *   - Accuracy is ＜5% or 0.3NTU
 *   - Result stored in sensorValues[2]
 *   - Resolution is 0.01 NTU
 *   - Reported as Nephelometric Turbidity Units (NTU)
 *   - Default variable code is Y550Turbidity
 *
 * The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 *
 * ___
 * @section y550_examples Example Code
 * The Yosemitech Y550 UV245/COD sensor is used in the @menulink{y550} example.
 *
 * @menusnip{y550}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY550_H_
#define SRC_SENSORS_YOSEMITECHY550_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the Y550 can report 2 values.
#define Y550_NUM_VARIABLES 2
/// Sensor::_warmUpTime_ms; the Y550 warms up in 1500ms.
#define Y550_WARM_UP_TIME_MS 1500
/// Sensor::_stabilizationTime_ms; the Y550 is stable after 2000ms.
#define Y550_STABILIZATION_TIME_MS 2000
/**
 * @brief Sensor::_measurementTime_ms; the Y550 takes 2000ms to complete a
 * measurement.
 */
#define Y550_MEASUREMENT_TIME_MS 2000

/// Decimals places in string representation; COD should have 2.
#define Y550_COD_RESOLUTION 2
/// Variable number; COD is stored in sensorValues[0].
#define Y550_COD_VAR_NUM 0

/// Decimals places in string representation; temperature should have 2.
#define Y550_TEMP_RESOLUTION 2
/// Variable number; temperature is stored in sensorValues[1].
#define Y550_TEMP_VAR_NUM 1

/// Decimals places in string representation; turbidity should have 2.
#define Y550_TURB_RESOLUTION 2
/// Variable number; turbidity is stored in sensorValues[2].
#define Y550_TURB_VAR_NUM 2

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y550-B UV254/COD sensor with wiper](@ref y550_page).
 *
 * @ingroup y550_group
 */
/* clang-format on */
class YosemitechY550 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y550 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication
     * @param powerPin A pin on the mcu controlling power to the sensor.
     * Defaults to -1.
     * @param powerPin2 A pin on the mcu controlling power to the RS485 adapter.
     * Defaults to -1.
     * @param enablePin A pin on the mcu controlling the direction enable on the RS485
     * adapter.  Defaults to -1.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor.  Defaults to 1.
     */
    YosemitechY550(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin,
                           measurementsToAverage, Y550, "YosemitechY550",
                           Y550_NUM_VARIABLES, Y550_WARM_UP_TIME_MS,
                           Y550_STABILIZATION_TIME_MS, Y550_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc YosemitechY550::YosemitechY550
     */
    YosemitechY550(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin,
                           measurementsToAverage, Y550, "YosemitechY550",
                           Y550_NUM_VARIABLES, Y550_WARM_UP_TIME_MS,
                           Y550_STABILIZATION_TIME_MS, Y550_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Yosemitech Y550 object
     */
    ~YosemitechY550() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [carbon oxygen demand (COD) output](@ref y550_cod) from a
 * [Yosemitech Y550-B UV254/COD sensor with wiper](@ref y550_page).
 *
 * @ingroup y550_group
 */
/* clang-format on */
class YosemitechY550_COD : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY550_COD object.
     *
     * @param parentSense The parent YosemitechY550 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y550COD".
     */
    explicit YosemitechY550_COD(YosemitechY550* parentSense, const char* uuid = "",
                                const char* varCode = "Y550COD")
        : Variable(parentSense, (const uint8_t)Y550_COD_VAR_NUM,
                   (uint8_t)Y550_COD_RESOLUTION, "COD", "milligramPerLiter", varCode,
                   uuid) {}
    /**
     * @brief Construct a new YosemitechY550_COD object.
     *
     * @note This must be tied with a parent YosemitechY550 before it can be
     * used.
     */
    YosemitechY550_COD()
        : Variable((const uint8_t)Y550_COD_VAR_NUM, (uint8_t)Y550_COD_RESOLUTION, "COD",
                   "milligramPerLiter", "Y550COD") {}
    /**
     * @brief Destroy the YosemitechY550_COD object - no action needed.
     */
    ~YosemitechY550_COD() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref y550_temp) from a
 * [Yosemitech Y550-B UV254/COD sensor with wiper](@ref y550_page).
 *
 * @ingroup y550_group
 */
/* clang-format on */
class YosemitechY550_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY550_Temp object.
     *
     * @param parentSense The parent YosemitechY550 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y550Temp".
     */
    explicit YosemitechY550_Temp(YosemitechY550* parentSense, const char* uuid = "",
                                 const char* varCode = "Y550Temp")
        : Variable(parentSense, (const uint8_t)Y550_TEMP_VAR_NUM,
                   (uint8_t)Y550_TEMP_RESOLUTION, "temperature", "degreeCelsius",
                   varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY550_Temp object.
     *
     * @note This must be tied with a parent YosemitechY550 before it can be
     * used.
     */
    YosemitechY550_Temp()
        : Variable((const uint8_t)Y550_TEMP_VAR_NUM, (uint8_t)Y550_TEMP_RESOLUTION,
                   "temperature", "degreeCelsius", "Y550Temp") {}
    /**
     * @brief Destroy the YosemitechY550_Temp object - no action needed.
     */
    ~YosemitechY550_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [turbidity output](@ref y550_turb) from a
 * [Yosemitech Y550-B UV254/COD sensor with wiper](@ref y550_page).
 *
 * @ingroup y550_group
 */
/* clang-format on */
class YosemitechY550_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY550_Turbidity object.
     *
     * @param parentSense The parent YosemitechY550 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y550Turbidity".
     */
    explicit YosemitechY550_Turbidity(YosemitechY550* parentSense,
                                      const char*     uuid    = "",
                                      const char*     varCode = "Y550Turbidity")
        : Variable(parentSense, (const uint8_t)Y550_TURB_VAR_NUM,
                   (uint8_t)Y550_TURB_RESOLUTION, "turbidity",
                   "nephelometricTurbidityUnit", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY550_Turbidity object.
     *
     * @note This must be tied with a parent YosemitechY550 before it can be
     * used.
     */
    YosemitechY550_Turbidity()
        : Variable((const uint8_t)Y550_TURB_VAR_NUM, (uint8_t)Y550_TURB_RESOLUTION,
                   "turbidity", "nephelometricTurbidityUnit", "Y550Turbidity") {}
    /**
     * @brief Destroy the YosemitechY550_Turbidity object - no action needed.
     */
    ~YosemitechY550_Turbidity() {}
};

#endif  // SRC_SENSORS_YOSEMITECHY550_H_
