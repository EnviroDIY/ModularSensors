/**
 * @file YosemitechY514.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY514 sensor subclass and the variable
 * subclasses YosemitechY514_Chlorophyll and YosemitechY514_Temp.
 *
 * These are for the Yosemitech Y514 chlorophyll sensor with wiper.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup y514_group Yosemitech Y514
 * Classes for the Yosemitech Y514 chlorophyll sensor with wiper.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section y514_datasheet Sensor Datasheet
 * - [Y514 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y514-Chlorophyl+Wiper_UserManual-v1.0.pdf)
 * - [Y515 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y515-Chlorophyll_UserManual-v1.0_en.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y514-Chlorophyl+Wiper-v1.6_ModbusInstructions.pdf)
 *
 * @section y514_sensor The y514 Sensor
 * @ctor_doc{YosemitechY514, byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage}
 * @subsection y514_timing Sensor Timing
 * - Time before sensor responds after power - 1.2 seconds
 * - Time between "StartMeasurement" command and stable reading - 8sec
 *
 * @section y514_chloro Chlorophyll Output
 *   - Range is 0 to 400 µg/L or 0 to 100 RFU
 *   - Accuracy is ± 1 %
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.1 µg/L / 0.1 RFU
 *   - Reported as micrograms per liter (µg/L)
 *   - Default variable code is Y514Chloro
 * @variabledoc{y514_chloro,YosemitechY514,Chlorophyll,Y514Chloro}
 *
 * @section y514_temp Temperature Output
 *   - Range is 0°C to + 50°C
 *   - Accuracy is ± 0.2°C
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.1 °C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is Y514Temp
 * @variabledoc{y514_temp,YosemitechY514,Temp,Y514Temp}
 *
 * The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 *
 * ___
 * @section y514_examples Example Code
 * The Yosemitech Y514 chlorophyll sensor is used in the @menulink{y514}
 * example.
 *
 * @menusnip{y514}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY514_H_
#define SRC_SENSORS_YOSEMITECHY514_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the Y514 can report 2 values.
#define Y514_NUM_VARIABLES 2
/// Sensor::_warmUpTime_ms; the Y514 warms up in 1300ms.
#define Y514_WARM_UP_TIME_MS 1300
/// Sensor::_stabilizationTime_ms; the Y514 is stable after 8000ms.
#define Y514_STABILIZATION_TIME_MS 8000
/**
 * @brief Sensor::_measurementTime_ms; the Y514 takes 2000ms to complete a
 * measurement.
 */
#define Y514_MEASUREMENT_TIME_MS 2000

/**
 * @brief Decimals places in string representation; chlorophyll concentration
 * should have 1.
 */
#define Y514_CHLORO_RESOLUTION 1
/// Variable number; chlorophyll concentration is stored in sensorValues[0].
#define Y514_CHLORO_VAR_NUM 0

/// Decimals places in string representation; temperature should have 1.
#define Y514_TEMP_RESOLUTION 1
/// Variable number; temperature is stored in sensorValues[1].
#define Y514_TEMP_VAR_NUM 1

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y514 sensor](@ref y514_group).
 *
 * @ingroup y514_group
 */
/* clang-format on */
class YosemitechY514 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y514 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)
     * for more information on what streams can be used.
     * @param powerPin The pin on the mcu controlling power to the Y514.
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
    YosemitechY514(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y514,
                           "YosemitechY514", Y514_NUM_VARIABLES,
                           Y514_WARM_UP_TIME_MS, Y514_STABILIZATION_TIME_MS,
                           Y514_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc YosemitechY514::YosemitechY514
     */
    YosemitechY514(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y514,
                           "YosemitechY514", Y514_NUM_VARIABLES,
                           Y514_WARM_UP_TIME_MS, Y514_STABILIZATION_TIME_MS,
                           Y514_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Yosemitech Y514 object
     */
    ~YosemitechY514() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [chlorophyll concentration output](@ref y514_chloro) from a
 * [Yosemitech Y514-A chlorophyll sensor with wiper](@ref y514_group).
 *
 * @ingroup y514_group
 */
/* clang-format on */
class YosemitechY514_Chlorophyll : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY514_Chlorophyll object.
     *
     * @param parentSense The parent YosemitechY514 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y514Chloro".
     */
    explicit YosemitechY514_Chlorophyll(YosemitechY514* parentSense,
                                        const char*     uuid    = "",
                                        const char*     varCode = "Y514Chloro")
        : Variable(parentSense, (const uint8_t)Y514_CHLORO_VAR_NUM,
                   (uint8_t)Y514_CHLORO_RESOLUTION, "chlorophyllFluorescence",
                   "microgramPerLiter", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY514_Chlorophyll object.
     *
     * @note This must be tied with a parent YosemitechY514 before it can be
     * used.
     */
    YosemitechY514_Chlorophyll()
        : Variable((const uint8_t)Y514_CHLORO_VAR_NUM,
                   (uint8_t)Y514_CHLORO_RESOLUTION, "chlorophyllFluorescence",
                   "microgramPerLiter", "Y514Chloro") {}
    /**
     * @brief Destroy the YosemitechY514_Chlorophyll() object - no action
     * needed.
     */
    ~YosemitechY514_Chlorophyll() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref y514_temp) from a
 * [Yosemitech Y514-A chlorophyll sensor with wiper](@ref y514_group).
 *
 * @ingroup y514_group
 */
/* clang-format on */
class YosemitechY514_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY514_Temp object.
     *
     * @param parentSense The parent YosemitechY514 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of Y514Temp
     */
    explicit YosemitechY514_Temp(YosemitechY514* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y514Temp")
        : Variable(parentSense, (const uint8_t)Y514_TEMP_VAR_NUM,
                   (uint8_t)Y514_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY514_Temp object.
     *
     * @note This must be tied with a parent YosemitechY514 before it can be
     * used.
     */
    YosemitechY514_Temp()
        : Variable((const uint8_t)Y514_TEMP_VAR_NUM,
                   (uint8_t)Y514_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y514Temp") {}
    /**
     * @brief Destroy the YosemitechY514_Temp object - no action needed.
     */
    ~YosemitechY514_Temp() {}
};
#endif  // SRC_SENSORS_YOSEMITECHY514_H_
