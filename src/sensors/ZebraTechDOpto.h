/**
 * @file ZebraTechDOpto.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ZebraTechDOpto sensor subclass and the variable
 * subclasses ZebraTechDOpto_Temp, ZebraTechDOpto_DOpct, and
 * ZebraTechDOpto_DOmgL.
 *
 * These are for the ZebraTech D-Opto digital dissolved oxygen sensor.
 *
 * This depends on the SDI12Sensors parent class.
 */
/* clang-format off */
/**
 * @defgroup dopto_group Zebra-Tech D-Opto
 * Classes for the Zebra-Tech D-Opto digital dissolved oxygen sensor.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section dopto_intro Introduction
 *
 * The [Zebra-Tech D-Opto](http://www.zebra-tech.co.nz/d-opto-sensor/) sensor is implemented as a sub-classes of the SDI12Sensors class.
 * It require an 8-12V power supply, which can be turned off between measurements.
 * The connection between the logger and the Arduino board is made by way of a white interface module provided by Zebra-Tech.
 * You will need a voltage booster or a separate power supply to give the D-Opto sufficient voltage to run.
 * At the Stroud Center, we use [Pololu 9V Step-Up Voltage Regulators](https://www.pololu.com/product/2116).
 *
 * @section dopto_datasheet Sensor Datasheet
 * The manual for this sensor is available at:
 * www.zebra-tech.co.nz/wp-content/uploads/2014/04/D-Opto-Sensor-manual-A4-ver-2.pdf
 *
 * ___
 * @section dopto_examples Example Code
 * The Zebra-Tech D-Opto is used in the @menulink{dopto} example.
 *
 * @menusnip{dopto}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_ZEBRATECHDOPTO_H_
#define SRC_SENSORS_ZEBRATECHDOPTO_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines
/** @ingroup dopto_group */
/**@{*/

/// @brief Sensor::_numReturnedValues; the D-Opto can report 3 values.
#define DOPTO_NUM_VARIABLES 3

/**
 * @anchor dopto_timing_defines
 * @name Sensor Timing
 * Defines for the sensor timing for an ZebraTech D-Opto
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the D-Opto warms up in 275ms.  Maximum
/// warm-up time in SDI-12 mode: ~250ms, give a bit of extra for safety.
#define DOPTO_WARM_UP_TIME_MS 275
/// @brief Sensor::_stabilizationTime_ms; the D-Opto is stable as soon as it
/// warms up (0ms stabilization).
#define DOPTO_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the D-Opto takes 5335ms to complete a
 * measurement.
 * - Excitiation time before measurement: ~5225ms
 * - Maximum measurement duration: ~110ms
 *
 * Obnoxiously, the sensor will not take a "concurrent" measurement and leave
 * the sensor powered on, so we must wait entire ~5200ms exitation time and the
 * 110ms measurement time each time.
 * There is the ability to do a non-concurrent measurement and leave the
 * sensor powered on, in which case the re-measurement takes ~110ms, but doing
 * it that way the sensor would send an interruption when it was finished,
 * possibly colliding with and confusing other sensor results.
 */
#define DOPTO_MEASUREMENT_TIME_MS 5335
/**@}*/

/**
 * @anchor dopto_temp_defines
 * @name Temperature
 * Defines for the temperature variable from an ZebraTech D-Opto
 * - Range is not specified in sensor datasheet
 * - Accuracy is ± 0.1°C
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is 0.01°C.
#define DOPTO_TEMP_RESOLUTION 2
/// @brief Variable number; temperature is stored in sensorValues[0].
#define DOPTO_TEMP_VAR_NUM 0
/// @brief Variable name; "temperature"
#define DOPTO_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name; "degreeCelsius" (°C)
#define DOPTO_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "DOtempC"
#define DOPTO_TEMP_DEFAULT_CODE "DOtempC"
/**@}*/

/**
 * @anchor dopto_temp_defines
 * @name Dissolved Oxygen Percent Saturation
 * Defines for the percent saturation variable from an ZebraTech D-Opto
 * - Range is not specified in sensor datasheet
 * - Accuracy is 1 % of reading or 0.02PPM, whichever is greater
 */
/// @brief Decimals places in string representation; dissolved oxygen percent
/// should have 2.
#define DOPTO_DOPCT_RESOLUTION 2
/// @brief Variable number; dissolved oxygen percent is stored in
/// sensorValues[1]
#define DOPTO_DOPCT_VAR_NUM 1
/// @brief Variable number; dissolved oxygen percent is stored in
/// sensorValues[1]
#define DOPTO_DOPCT_VAR_NAME "oxygenDissolvedPercentOfSaturation"
/// @brief Variable unit name; "percent" (% saturation)
#define DOPTO_DOPCT_UNIT_NAME "percent"
/// @brief Default variable short code; "DOpercent"
#define DOPTO_DOPCT_DEFAULT_CODE "DOpercent"
/**@}*/

/**
 * @anchor dopto_doconc_defines
 * @name Dissolved Oxygen Concentration
 * Defines for the DO concentration variable from an ZebraTech D-Opto
 * - Range is not specified in sensor datasheet
 * - Accuracy is 1 % of reading or 0.02PPM, whichever is greater
 */
/// @brief Decimals places in string representation; dissolved oxygen
/// concentration should have 3 - resolution is 0.01 % / 0.001 PPM.
#define DOPTO_DOMGL_RESOLUTION 3
/// @brief Variable number; dissolved oxygen concentration is stored in
/// sensorValues[2]
#define DOPTO_DOMGL_VAR_NUM 2
/// @brief Variable number; dissolved oxygen concentration is stored in
/// sensorValues[2]
#define DOPTO_DOMGL_VAR_NAME "oxygenDissolved"
/// @brief Variable unit name; "milligramPerLiter" (mg/L)
#define DOPTO_DOMGL_UNIT_NAME "milligramPerLiter"
/// @brief Default variable short code; "DOppm"
#define DOPTO_DOMGL_DEFAULT_CODE "DOppm"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Zebra-Tech D-Opto dissolved oxygen sensor](@ref dopto_group).
 *
 * @ingroup dopto_group
 */
/* clang-format on */
class ZebraTechDOpto : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Zebra-Tech DOpto object.
     * @ingroup dopto_group
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required for
     * the sensor constructor.  Optionally, you can include a number of distinct
     * readings to average.  The data pin must be a pin that supports pin-change
     * interrupts.
     *
     * @param SDI12address The SDI-12 address of the DOpto.
     * @param powerPin The pin on the mcu controlling power to the D-Opto
     * Use -1 if it is continuously powered.
     * - The D-Opto requires a 8-12V power supply, which can be turned off
     * between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    ZebraTechDOpto(char SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                       DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS,
                       DOPTO_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc ZebraTechDOpto::ZebraTechDOpto
     */
    ZebraTechDOpto(char* SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                       DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS,
                       DOPTO_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc ZebraTechDOpto::ZebraTechDOpto
     */
    ZebraTechDOpto(int SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                       DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS,
                       DOPTO_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Zebra-Tech DOpto object
     */
    ~ZebraTechDOpto() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref dopto_temp) from a
 * [Zebra-Tech D-Opto dissolved oxygen sensor](@ref dopto_group).
 *
 * @ingroup dopto_group
 */
/* clang-format on */
class ZebraTechDOpto_Temp : public Variable {
 public:
    /**
     * @brief Construct a new ZebraTechDOpto_Temp object.
     * @ingroup dopto_group
     *
     * @param parentSense The parent ZebraTechDOpto providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "DOtempC".
     */
    explicit ZebraTechDOpto_Temp(ZebraTechDOpto* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = DOPTO_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)DOPTO_TEMP_VAR_NUM,
                   (uint8_t)DOPTO_TEMP_RESOLUTION, DOPTO_TEMP_VAR_NAME,
                   DOPTO_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ZebraTechDOpto_Temp object.
     *
     * @note This must be tied with a parent ZebraTechDOpto before it can be
     * used.
     */
    ZebraTechDOpto_Temp()
        : Variable((const uint8_t)DOPTO_TEMP_VAR_NUM,
                   (uint8_t)DOPTO_TEMP_RESOLUTION, DOPTO_TEMP_VAR_NAME,
                   DOPTO_TEMP_UNIT_NAME, DOPTO_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ZebraTechDOpto_Temp object - no action needed.
     */
    ~ZebraTechDOpto_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen percent saturation output](@ref dopto_dopercent)
 * from a [Zebra-Tech D-Opto dissolved oxygen sensor](@ref dopto_group).
 *
 * @ingroup dopto_group
 */
/* clang-format on */
class ZebraTechDOpto_DOpct : public Variable {
 public:
    /**
     * @brief Construct a new ZebraTechDOpto_DOpct object.
     * @ingroup dopto_group
     *
     * @param parentSense The parent ZebraTechDOpto providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "DOpercent".
     */
    explicit ZebraTechDOpto_DOpct(
        ZebraTechDOpto* parentSense, const char* uuid = "",
        const char* varCode = DOPTO_DOPCT_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)DOPTO_DOPCT_VAR_NUM,
                   (uint8_t)DOPTO_DOPCT_RESOLUTION, DOPTO_DOPCT_VAR_NAME,
                   DOPTO_DOPCT_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ZebraTechDOpto_DOpct object.
     *
     * @note This must be tied with a parent ZebraTechDOpto before it can be
     * used.
     */
    ZebraTechDOpto_DOpct()
        : Variable((const uint8_t)DOPTO_DOPCT_VAR_NUM,
                   (uint8_t)DOPTO_DOPCT_RESOLUTION, DOPTO_DOPCT_VAR_NAME,
                   DOPTO_DOPCT_UNIT_NAME, DOPTO_DOPCT_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ZebraTechDOpto_DOpct object - no action needed.
     */
    ~ZebraTechDOpto_DOpct() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen concentration output](@ref dopto_domgl) from a
 * [Zebra-Tech D-Opto dissolved oxygen sensor](@ref dopto_group).
 *
 * @ingroup dopto_group
 */
/* clang-format on */
class ZebraTechDOpto_DOmgL : public Variable {
 public:
    /**
     * @brief Construct a new ZebraTechDOpto_DOmgL object.
     * @ingroup dopto_group
     *
     * @param parentSense The parent ZebraTechDOpto providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "DOppm".
     */
    explicit ZebraTechDOpto_DOmgL(
        ZebraTechDOpto* parentSense, const char* uuid = "",
        const char* varCode = DOPTO_DOMGL_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)DOPTO_DOMGL_VAR_NUM,
                   (uint8_t)DOPTO_DOMGL_RESOLUTION, DOPTO_DOMGL_VAR_NAME,
                   DOPTO_DOMGL_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ZebraTechDOpto_DOmgL object.
     *
     * @note This must be tied with a parent ZebraTechDOpto before it can be
     * used.
     */
    ZebraTechDOpto_DOmgL()
        : Variable((const uint8_t)DOPTO_DOMGL_VAR_NUM,
                   (uint8_t)DOPTO_DOMGL_RESOLUTION, DOPTO_DOMGL_VAR_NAME,
                   DOPTO_DOMGL_UNIT_NAME, DOPTO_DOMGL_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ZebraTechDOpto_DOmgL object - no action needed.
     */
    ~ZebraTechDOpto_DOmgL() {}
};
/**@}*/
#endif  // SRC_SENSORS_ZEBRATECHDOPTO_H_
