/**
 * @file YosemitechY4000.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY4000 sensor subclass and the variable
 * subclasses YosemitechY4000_DOmgL, YosemitechY4000_Turbidity,
 * YosemitechY4000_Cond, YosemitechY4000_pH, YosemitechY4000_Temp,
 * YosemitechY4000_ORP, YosemitechY4000_Chlorophyll, and YosemitechY4000_BGA.
 *
 * These are for the Yosemitech Y4000 multi-parameter sonde.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 *
 * More detailed infromation on each variable can be found in the documentation
 * for the individual sensor probes
 */
/* clang-format off */
/**
 * @defgroup y4000_group Yosemitech Y4000 Sonde
 * Classes for the Yosemitech Y4000 multi-parameter sonde.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section y4000_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y4000-Sonde_UserManual_v2.0.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y4000-Sonde-1.6-ModbusInstruction-en.pdf)
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * ___
 * @section y4000_examples Example Code
 * The Y4000 Sonde is used in the @menulink{y4000} example.
 *
 * @menusnip{y4000}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY4000_H_
#define SRC_SENSORS_YOSEMITECHY4000_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
/** @ingroup y4000_group */
/**@{*/

/// @brief Sensor::_numReturnedValues; the Y4000 can report 8 values.
#define Y4000_NUM_VARIABLES 8

/**
 * @anchor y4000_timing_defines
 * @name Sensor Timing
 * Defines for the sensor timing for a Yosemitch Y4000 multi-parameter sonde
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; time before sensor responds after power - 1.6
 * seconds (1600ms).
 *
 * This is the time for communication to begin.
 */
#define Y4000_WARM_UP_TIME_MS 1600
/**
 * @brief  Sensor::_stabilizationTime_ms; the Y4000 is stable after 60000ms.
 *
 * Y4000 Modbus manual says 60s; we find Cond takes about that long to respond.
 */
#define Y4000_STABILIZATION_TIME_MS 60000
/// @brief Sensor::_measurementTime_ms; the Y4000 takes ~3000ms to complete a
/// measurement.
#define Y4000_MEASUREMENT_TIME_MS 3000
/**@}*/

/**
 * @anchor y4000_docond_defines
 * @name Dissolved Oxygen Concentration
 * Defines for the dissolved oxygen concentration variable from a Yosemitch
 * Y4000 multi-parameter sonde
 * - Range is 0-20mg/L or 0-200% Air Saturation
 * - Accuracy is ± 0.3 mg/L
 */
/// @brief Decimals places in string representation; dissolved oxygen
/// concentration should have 2 - resolution is 0.01 mg/L.
#define Y4000_DOMGL_RESOLUTION 2
/// @brief Variable number; dissolved oxygen concentration is stored in
/// sensorValues[0]
#define Y4000_DOMGL_VAR_NUM 0
/// @brief Variable number; dissolved oxygen concentration is stored in
/// sensorValues[0]
#define Y4000_DOMGL_VAR_NAME "oxygenDissolved"
/// @brief Variable unit name; "milligramPerLiter" (mg/L)
#define Y4000_DOMGL_UNIT_NAME "milligramPerLiter"
/// @brief Default variable short code; "Y4000DOmgL"
#define Y4000_DOMGL_DEFAULT_CODE "Y4000DOmgL"
/**@}*/

/**
 * @anchor y4000_turb_defines
 * @name Turbidity
 * Defines for the turbidity variable from a Yosemitch Y4000 multi-parameter
 * sonde
 * - Range is 0.1~1000 NTU
 * - Accuracy is ＜5% or 0.3NTU
 */
/**@{*/
/// @brief Decimals places in string representation; turbidity should have 2 -
/// resolution is 0.01 NTU.
#define Y4000_TURB_RESOLUTION 2
/// @brief Variable number; turbidity is stored in sensorValues[1].
#define Y4000_TURB_VAR_NUM 1
/// @brief Variable name; "turbidity"
#define Y4000_TURB_VAR_NAME "turbidity"
/// @brief Variable unit name; "nephelometricTurbidityUnit" (NTU)
#define Y4000_TURB_UNIT_NAME "nephelometricTurbidityUnit"
/// @brief Default variable short code; "Y4000Turbidity"
#define Y4000_TURB_DEFAULT_CODE "Y4000Turbidity"
/**@}*/

/**
 * @anchor y4000_cond_defines
 * @name Conductivity
 * Defines for the conductivity variable from a Yosemitch Y4000 multi-parameter
 * sonde
 * - Range is 1 µS/cm to 200 mS/cm
 * - Accuracy is ± 1 % Full Scale
 */
/**@{*/
/// @brief Decimals places in string representation; conductivity should have 1
/// - resolution is 0.1 µS/cm.
#define Y4000_COND_RESOLUTION 1
/// @brief Variable number; conductivity is stored in sensorValues[2].
#define Y4000_COND_VAR_NUM 2
/// @brief Variable name; "specificConductance"
#define Y4000_COND_VAR_NAME "specificConductance"
/// @brief Variable unit name; "microsiemenPerCentimeter" (µS/cm)
#define Y4000_COND_UNIT_NAME "microsiemenPerCentimeter"
/// @brief Default variable short code; "Y4000Cond"
#define Y4000_COND_DEFAULT_CODE "Y4000Cond"
/**@}*/

/**
 * @anchor y4000_ph_defines
 * @name pH
 * Defines for the pH variable from a Yosemitch Y4000 multi-parameter sonde
 * - Range is 2 to 12 pH units
 * - Accuracy is ± 0.1 pH units
 */
/**@{*/
/// @brief Decimals places in string representation; ph should have 2 -
/// resolution is 0.01 pH units.
#define Y4000_PH_RESOLUTION 2
/// @brief Variable number; pH is stored in sensorValues[3].
#define Y4000_PH_VAR_NUM 3
/// @brief Variable name; "pH"
#define Y4000_PH_VAR_NAME "pH"
/// @brief Variable unit name; "pH" (dimensionless pH units)
#define Y4000_PH_UNIT_NAME "pH"
/// @brief Default variable short code; "Y4000pH"
#define Y4000_PH_DEFAULT_CODE "Y4000pH"
/**@}*/

/**
 * @anchor y4000_temp_defines
 * @name Temperature
 * Defines for the temperature variable from a Yosemitch Y4000 multi-parameter
 * sonde
 * - Range is 0°C to + 50°C
 * - Accuracy is ± 0.2°C
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define Y4000_TEMP_RESOLUTION 1
/// @brief Variable number; temperature is stored in sensorValues[4].
#define Y4000_TEMP_VAR_NUM 4
/// @brief Variable name; "temperature"
#define Y4000_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name; "degreeCelsius" (°C)
#define Y4000_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y4000Temp"
#define Y4000_TEMP_DEFAULT_CODE "Y4000Temp"
/**@}*/

/**
 * @anchor y4000_orp_defines
 * @name ORP
 * Defines for the ORP variable from a Yosemitch Y4000 multi-parameter sonde
 * - Range is -999 ~ 999 mV
 * - Accuracy is ± 20 mV
 */
/**@{*/
/// @brief Decimals places in string representation; orp should have 0 -
/// resolution is 1 mV.
#define Y4000_ORP_RESOLUTION 0
/// @brief Variable number; ORP is stored in sensorValues[5].
#define Y4000_ORP_VAR_NUM 5
/// @brief Variable name; "ORP"
#define Y4000_ORP_VAR_NAME "ORP"
/// @brief Variable unit name; "millivolt" (mV)
#define Y4000_ORP_UNIT_NAME "millivolt"
/// @brief Default variable short code; "Y4000Potential"
#define Y4000_ORP_DEFAULT_CODE "Y4000Potential"
/**@}*/

/**
 * @anchor y4000_chloro_defines
 * @name Chlorophyll Concentration
 * Defines for the chlorophyll concentration variable from a Yosemitch Y4000
 * multi-parameter sonde
 * - Range is 0 to 400 µg/L or 0 to 100 RFU
 * - Accuracy is ± 1 %
 */
/**@{*/
/// @brief Decimals places in string representation; chlorophyll concentration
/// should have 1 - resolution is 0.1 µg/L / 0.1 RFU.
#define Y4000_CHLORO_RESOLUTION 1
/// @brief Variable number; chlorophyll concentration is stored in
/// sensorValues[6].
#define Y4000_CHLORO_VAR_NUM 6
/// @brief Variable name; "chlorophyllFluorescence"
#define Y4000_CHLORO_VAR_NAME "chlorophyllFluorescence"
/// @brief Variable unit name; "microgramPerLiter" (µg/L)
#define Y4000_CHLORO_UNIT_NAME "microgramPerLiter"
/// @brief Default variable short code; "Y4000Chloro"
#define Y4000_CHLORO_DEFAULT_CODE "Y4000Chloro"
/**@}*/

/**
 * @anchor y4000_bga_defines
 * @name Blue Green Algae Concentration
 * Defines for the BGA variable from a Yosemitch Y4000 multi-parameter sonde
 * - Range is 0 to 100 µg/L or 0 to 100 RFU
 * - Accuracy is ±  0.04ug/L PC
 */
/**@{*/
/// @brief Decimals places in string representation; bga should have 2 -
/// resolution is 0.01 µg/L / 0.01 RFU.
#define Y4000_BGA_RESOLUTION 2
/// @brief Variable number; BGA is stored in sensorValues[7].
#define Y4000_BGA_VAR_NUM 7
/// @brief Variable name; "blueGreenAlgaeCyanobacteriaPhycocyanin"
#define Y4000_BGA_VAR_NAME "blueGreenAlgaeCyanobacteriaPhycocyanin"
/// @brief Variable unit name; "microgramPerLiter" (µg/L)
#define Y4000_BGA_UNIT_NAME "microgramPerLiter"
/// @brief Default variable short code; "Y4000BGA"
#define Y4000_BGA_DEFAULT_CODE "Y4000BGA"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y4000 object.
     * @ingroup y4000_group
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)
     * for more information on what streams can be used.
     * @param powerPin The pin on the mcu controlling power to the Y4000.
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
    YosemitechY4000(byte modbusAddress, Stream* stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y4000,
                           "YosemitechY4000", Y4000_NUM_VARIABLES,
                           Y4000_WARM_UP_TIME_MS, Y4000_STABILIZATION_TIME_MS,
                           Y4000_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc YosemitechY4000::YosemitechY4000
     */
    YosemitechY4000(byte modbusAddress, Stream& stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y4000,
                           "YosemitechY4000", Y4000_NUM_VARIABLES,
                           Y4000_WARM_UP_TIME_MS, Y4000_STABILIZATION_TIME_MS,
                           Y4000_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Yosemitech Y4000 object
     */
    ~YosemitechY4000() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen concentration output](@ref y4000_domgl) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_DOmgL : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_DOmgL object.
     * @ingroup y4000_group
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000DOmgL".
     */
    explicit YosemitechY4000_DOmgL(
        YosemitechY4000* parentSense, const char* uuid = "",
        const char* varCode = Y4000_DOMGL_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y4000_DOMGL_VAR_NUM,
                   (uint8_t)Y4000_DOMGL_RESOLUTION, Y4000_DOMGL_VAR_NAME,
                   Y4000_DOMGL_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_DOmgL object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_DOmgL()
        : Variable((const uint8_t)Y4000_DOMGL_VAR_NUM,
                   (uint8_t)Y4000_DOMGL_RESOLUTION, Y4000_DOMGL_VAR_NAME,
                   Y4000_DOMGL_UNIT_NAME, Y4000_DOMGL_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY4000_DOmgL object - no action needed.
     */
    ~YosemitechY4000_DOmgL() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [turbidity output](@ref y4000_turb) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Turbidity object.
     * @ingroup y4000_group
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000Turbidity".
     */
    explicit YosemitechY4000_Turbidity(
        YosemitechY4000* parentSense, const char* uuid = "",
        const char* varCode = Y4000_TURB_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y4000_TURB_VAR_NUM,
                   (uint8_t)Y4000_TURB_RESOLUTION, Y4000_TURB_VAR_NAME,
                   Y4000_TURB_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_Turbidity object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_Turbidity()
        : Variable((const uint8_t)Y4000_TURB_VAR_NUM,
                   (uint8_t)Y4000_TURB_RESOLUTION, Y4000_TURB_VAR_NAME,
                   Y4000_TURB_UNIT_NAME, Y4000_TURB_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY4000_Turbidity object - no action needed.
     */
    ~YosemitechY4000_Turbidity() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [conductivity output](@ref y4000_cond) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_Cond : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Cond object.
     * @ingroup y4000_group
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000Cond".
     */
    explicit YosemitechY4000_Cond(YosemitechY4000* parentSense,
                                  const char*      uuid = "",
                                  const char* varCode = Y4000_COND_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y4000_COND_VAR_NUM,
                   (uint8_t)Y4000_COND_RESOLUTION, Y4000_COND_VAR_NAME,
                   Y4000_COND_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_Cond object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_Cond()
        : Variable((const uint8_t)Y4000_COND_VAR_NUM,
                   (uint8_t)Y4000_COND_RESOLUTION, Y4000_COND_VAR_NAME,
                   Y4000_COND_UNIT_NAME, Y4000_COND_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY4000_Cond object - no action needed.
     */
    ~YosemitechY4000_Cond() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [pH output](@ref y4000_ph)
 * from a [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_pH : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_pH object.
     * @ingroup y4000_group
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000pH".
     */
    explicit YosemitechY4000_pH(YosemitechY4000* parentSense,
                                const char*      uuid = "",
                                const char* varCode   = Y4000_PH_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y4000_PH_VAR_NUM,
                   (uint8_t)Y4000_PH_RESOLUTION, Y4000_PH_VAR_NAME,
                   Y4000_PH_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_pH object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_pH()
        : Variable((const uint8_t)Y4000_PH_VAR_NUM,
                   (uint8_t)Y4000_PH_RESOLUTION, Y4000_PH_VAR_NAME,
                   Y4000_PH_UNIT_NAME, Y4000_PH_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY4000_pH object - no action needed.
     */
    ~YosemitechY4000_pH() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref y4000_temp) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Temp object.
     * @ingroup y4000_group
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000Temp".
     */
    explicit YosemitechY4000_Temp(YosemitechY4000* parentSense,
                                  const char*      uuid = "",
                                  const char* varCode = Y4000_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y4000_TEMP_VAR_NUM,
                   (uint8_t)Y4000_TEMP_RESOLUTION, Y4000_TEMP_VAR_NAME,
                   Y4000_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_Temp object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_Temp()
        : Variable((const uint8_t)Y4000_TEMP_VAR_NUM,
                   (uint8_t)Y4000_TEMP_RESOLUTION, Y4000_TEMP_VAR_NAME,
                   Y4000_TEMP_UNIT_NAME, Y4000_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY4000_Temp object - no action needed.
     */
    ~YosemitechY4000_Temp() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [electrode electrical potential output](@ref y4000_orp) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_ORP : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_ORP object.
     * @ingroup y4000_group
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000Potential".
     */
    explicit YosemitechY4000_ORP(YosemitechY4000* parentSense,
                                 const char*      uuid = "",
                                 const char* varCode   = Y4000_ORP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y4000_ORP_VAR_NUM,
                   (uint8_t)Y4000_ORP_RESOLUTION, Y4000_ORP_VAR_NAME,
                   Y4000_ORP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_ORP object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_ORP()
        : Variable((const uint8_t)Y4000_ORP_VAR_NUM,
                   (uint8_t)Y4000_ORP_RESOLUTION, Y4000_ORP_VAR_NAME,
                   Y4000_ORP_UNIT_NAME, Y4000_ORP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY4000_ORP object - no action needed.
     */
    ~YosemitechY4000_ORP() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [chlorophyll concentration output](@ref y4000_chloro) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group)
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_Chlorophyll : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Chlorophyll object.
     * @ingroup y4000_group
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000Chloro".
     */
    explicit YosemitechY4000_Chlorophyll(
        YosemitechY4000* parentSense, const char* uuid = "",
        const char* varCode = Y4000_CHLORO_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y4000_CHLORO_VAR_NUM,
                   (uint8_t)Y4000_CHLORO_RESOLUTION, Y4000_CHLORO_VAR_NAME,
                   Y4000_CHLORO_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_Chlorophyll object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_Chlorophyll()
        : Variable((const uint8_t)Y4000_CHLORO_VAR_NUM,
                   (uint8_t)Y4000_CHLORO_RESOLUTION, Y4000_CHLORO_VAR_NAME,
                   Y4000_CHLORO_UNIT_NAME, Y4000_CHLORO_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY4000_Chlorophyll() object - no action
     * needed.
     */
    ~YosemitechY4000_Chlorophyll() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [blue green algae (BGA) concentration output](@ref y4000_bga) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref y4000_group).
 *
 * @ingroup y4000_group
 */
/* clang-format on */
class YosemitechY4000_BGA : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_BGA object.
     * @ingroup y4000_group
     *
     * @param parentSense The parent YosemitechY4000 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y4000BGA".
     */
    explicit YosemitechY4000_BGA(YosemitechY4000* parentSense,
                                 const char*      uuid = "",
                                 const char* varCode   = Y4000_BGA_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y4000_BGA_VAR_NUM,
                   (uint8_t)Y4000_BGA_RESOLUTION, Y4000_BGA_VAR_NAME,
                   Y4000_BGA_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY4000_BGA object.
     *
     * @note This must be tied with a parent YosemitechY4000 before it can be
     * used.
     */
    YosemitechY4000_BGA()
        : Variable((const uint8_t)Y4000_BGA_VAR_NUM,
                   (uint8_t)Y4000_BGA_RESOLUTION, Y4000_BGA_VAR_NAME,
                   Y4000_BGA_UNIT_NAME, Y4000_BGA_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY4000_BGA object - no action needed.
     */
    ~YosemitechY4000_BGA() {}
};


#endif  // SRC_SENSORS_YOSEMITECHY4000_H_
