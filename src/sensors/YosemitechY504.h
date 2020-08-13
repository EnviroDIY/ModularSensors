/**
 * @file YosemitechY504.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY504 sensor subclass and the variable
 * subclasses YosemitechY504_DOpct, YosemitechY504_Temp, and
 * YosemitechY504_DOmgL.
 *
 * These are for the Yosemitech Y502-A or Y504-A optical Dissolved Oxygen
 * sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup y504_group Yosemitech Y504 Dissolved Oxygen
 * Classes for the @ref y504_page
 *
 * @ingroup yosemitech_group
 *
 * @copydoc y504_page
 */
/* clang-format on */
/* clang-format off */
/**
 * @page y504_page Yosemitech Y504 Dissolved Oxygen
 *
 * @tableofcontents
 *
 * @section y504_datasheet Sensor Datasheet
 * - [Y504 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y504-DO_UserManual-v1.1.pdf)
 * - [Y504 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y505-DO_UserManual-v1.2.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y504-DO-v6.2_ModbusInstructions.pdf)
 *
 * @section y504_sensor The Yosemitech Y504 Dissolved Oxygen Sensor
 * @ctor_doc{YosemitechY504, byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage}
 * @subsection y504_timing Sensor Timing
 * - Time before sensor responds after power - 275-300ms (use 350 for safety)
 * - Time between "StartMeasurement" command and stable reading - 8sec
 *
 * @section y504_dopercent Dissolved Oxygen Percent Saturation Output
 * @variabledoc{YosemitechY504,DOpct}
 *   - Range is 0-20mg/L or 0-200% Saturation
 *   - Accuracy is ± 1 %
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.1%
 *   - Reported as percent saturation (%)
 *   - Default variable code is Y504DOpct
 *
 * @section y504_temp Temperature Output
 * @variabledoc{YosemitechY504,Temp}
 *   - Range is 0°C to + 50°C
 *   - Accuracy is ± 0.2°C
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.1 °C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is Y504Temp
 *
 * @section y504_domgl Dissolved Oxygen Concentration Output
 * @variabledoc{YosemitechY504,DOmgL}
 *   - Range is 0-20mg/L or 0-200% Saturation
 *   - Accuracy is ± 1 %
 *   - Result stored in sensorValues[2]
 *   - Resolution is 0.01 mg/L
 *   - Reported as milligrams per liter (mg/L)
 *   - Default variable code is Y504DOmgL
 *
 * The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 *
 * ___
 * @section y504_examples Example Code
 * The Yosemitech Y504 Dissolved Oxygen is used in the @menulink{y504} example.
 *
 * @menusnip{y504}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY504_H_
#define SRC_SENSORS_YOSEMITECHY504_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the Y504 can report 3 values.
#define Y504_NUM_VARIABLES 3
/// Sensor::_warmUpTime_ms; the Y504 warms up in 375ms.
#define Y504_WARM_UP_TIME_MS 375
/// Sensor::_stabilizationTime_ms; the Y504 is stable after 8000ms.
#define Y504_STABILIZATION_TIME_MS 8000
/**
 * @brief Sensor::_measurementTime_ms; the Y504 takes 1700ms to complete a
 * measurement.
 */
#define Y504_MEASUREMENT_TIME_MS 1700

/**
 * @brief Decimals places in string representation; dissolved oxygen percent
 * should have 1.
 */
#define Y504_DOPCT_RESOLUTION 1
/// Variable number; dissolved oxygen percent is stored in sensorValues[0]
#define Y504_DOPCT_VAR_NUM 0

/// Decimals places in string representation; temperature should have 1.
#define Y504_TEMP_RESOLUTION 1
/// Variable number; temperature is stored in sensorValues[1].
#define Y504_TEMP_VAR_NUM 1

/**
 * @brief Decimals places in string representation; dissolved oxygen
 * concentration should have 2.
 */
#define Y504_DOMGL_RESOLUTION 2
/// Variable number; dissolved oxygen concentration is stored in sensorValues[2]
#define Y504_DOMGL_VAR_NUM 2

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y502-A or Y504-A optical dissolved oxygen sensors](@ref y504_page).
 *
 * @ingroup y504_group
 */
/* clang-format on */
class YosemitechY504 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y504 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)
     * for more information on what streams can be used.
     * @param powerPin The pin on the mcu controlling power to the Y504.  Use -1
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
    YosemitechY504(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y504,
                           "YosemitechY504", Y504_NUM_VARIABLES,
                           Y504_WARM_UP_TIME_MS, Y504_STABILIZATION_TIME_MS,
                           Y504_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc YosemitechY504::YosemitechY504
     */
    YosemitechY504(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y504,
                           "YosemitechY504", Y504_NUM_VARIABLES,
                           Y504_WARM_UP_TIME_MS, Y504_STABILIZATION_TIME_MS,
                           Y504_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Yosemitech Y504 object
     */
    ~YosemitechY504() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen percent saturation output](@ref y504_dopercent) from a
 * [Yosemitech Y502-A or Y504-A optical dissolved oxygen sensor](@ref y504_page).
 *
 * @ingroup y504_group
 */
/* clang-format on */
class YosemitechY504_DOpct : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY504_DOpct object.
     *
     * @param parentSense The parent YosemitechY504 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of Y504DOpct
     */
    explicit YosemitechY504_DOpct(YosemitechY504* parentSense,
                                  const char*     uuid    = "",
                                  const char*     varCode = "Y504DOpct")
        : Variable(parentSense, (const uint8_t)Y504_DOPCT_VAR_NUM,
                   (uint8_t)Y504_DOPCT_RESOLUTION,
                   "oxygenDissolvedPercentOfSaturation", "percent", varCode,
                   uuid) {}
    /**
     * @brief Construct a new YosemitechY504_DOpct object.
     *
     * @note This must be tied with a parent YosemitechY504 before it can be
     * used.
     */
    YosemitechY504_DOpct()
        : Variable(
              (const uint8_t)Y504_DOPCT_VAR_NUM, (uint8_t)Y504_DOPCT_RESOLUTION,
              "oxygenDissolvedPercentOfSaturation", "percent", "Y504DOpct") {}
    /**
     * @brief Destroy the YosemitechY504_DOpct object - no action needed.
     */
    ~YosemitechY504_DOpct() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref y504_temp) from a
 * [Yosemitech Y504 optical dissolved oxygen sensor](@ref y504_page).
 *
 * @ingroup y504_group
 */
/* clang-format on */
class YosemitechY504_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY504_Temp object.
     *
     * @param parentSense The parent YosemitechY504 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y504Temp".
     */
    explicit YosemitechY504_Temp(YosemitechY504* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y504Temp")
        : Variable(parentSense, (const uint8_t)Y504_TEMP_VAR_NUM,
                   (uint8_t)Y504_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY504_Temp object.
     *
     * @note This must be tied with a parent YosemitechY504 before it can be
     * used.
     */
    YosemitechY504_Temp()
        : Variable((const uint8_t)Y504_TEMP_VAR_NUM,
                   (uint8_t)Y504_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y504Temp") {}
    /**
     * @brief Destroy the YosemitechY504_Temp object - no action needed.
     */
    ~YosemitechY504_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen concentration output](@ref y504_domgl) from a
 * [Yosemitech Y502-A or Y504-A optical dissolved oxygen sensor](@ref y504_page).
 *
 * @ingroup y504_group
 */
/* clang-format on */
class YosemitechY504_DOmgL : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY504_DOmgL object.
     *
     * @param parentSense The parent YosemitechY504 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y504DOmgL".
     */
    explicit YosemitechY504_DOmgL(YosemitechY504* parentSense,
                                  const char*     uuid    = "",
                                  const char*     varCode = "Y504DOmgL")
        : Variable(parentSense, (const uint8_t)Y504_DOMGL_VAR_NUM,
                   (uint8_t)Y504_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY504_DOmgL object.
     *
     * @note This must be tied with a parent YosemitechY504 before it can be
     * used.
     */
    YosemitechY504_DOmgL()
        : Variable((const uint8_t)Y504_DOMGL_VAR_NUM,
                   (uint8_t)Y504_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", "Y504DOmgL") {}
    /**
     * @brief Destroy the YosemitechY504_DOmgL object - no action needed.
     */
    ~YosemitechY504_DOmgL() {}
};

#endif  // SRC_SENSORS_YOSEMITECHY504_H_
