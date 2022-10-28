/**
 * @file YosemitechY4000.h
 * @copyright 2017-2022 Stroud Water Research Center
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
 * @defgroup sensor_y4000 Yosemitech Y4000 Multi-Parameter Sonde
 * Classes for the Yosemitech Y4000 multi-parameter sonde.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_y4000_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y4000-Sonde_UserManual_v2.0.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y4000-Sonde-1.6-ModbusInstruction-en.pdf)
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y4000_ctor Sensor Constructor
 * {{ @ref YosemitechY4000::YosemitechY4000 }}
 *
 * ___
 * @section sensor_y4000_examples Example Code
 * The Y4000 Sonde is used in the @menulink{yosemitech_y4000} example.
 *
 * @menusnip{yosemitech_y4000}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY4000_H_
#define SRC_SENSORS_YOSEMITECHY4000_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

/** @ingroup sensor_y4000 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Y4000 can report 8 values.
#define Y4000_NUM_VARIABLES 8
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define Y4000_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_y4000_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y4000 multi-parameter sonde
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; time before sensor responds after power - 1.6
 * seconds (1600ms).
 *
 * This is the time for communication to begin.
 */
#define Y4000_WARM_UP_TIME_MS 1000
/**
 * @brief Sensor::_stabilizationTime_ms; the Y4000 is stable after 60000ms.
 *
 * Y4000 Modbus manual says 60s; we find Cond takes about 40s to respond.
 */
#define Y4000_STABILIZATION_TIME_MS 50000
/// @brief Sensor::_measurementTime_ms; the Y4000 takes ~4000ms to complete a
/// measurement.
#define Y4000_MEASUREMENT_TIME_MS 4000
/**@}*/

/**
 * @anchor sensor_y4000_domgl
 * @name Dissolved Oxygen Concentration
 * The dissolved oxygen concentration variable from a Yosemitch Y4000
 * multi-parameter sonde
 * - Range is 0-20mg/L or 0-200% Air Saturation
 * - Accuracy is ± 0.3 mg/L
 *
 * {{ @ref YosemitechY4000_DOmgL::YosemitechY4000_DOmgL }}
 */
/**@{*/
/// @brief Decimals places in string representation; dissolved oxygen
/// concentration should have 2 - resolution is 0.01 mg/L.
#define Y4000_DOMGL_RESOLUTION 2
/// @brief Sensor variable number; dissolved oxygen concentration is stored in
/// sensorValues[0]
#define Y4000_DOMGL_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "oxygenDissolved"
#define Y4000_DOMGL_VAR_NAME "oxygenDissolved"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "milligramPerLiter" (mg/L)
#define Y4000_DOMGL_UNIT_NAME "milligramPerLiter"
/// @brief Default variable short code; "Y4000DOmgL"
#define Y4000_DOMGL_DEFAULT_CODE "Y4000DOmgL"
/**@}*/

/**
 * @anchor sensor_y4000_turb
 * @name Turbidity
 * The turbidity variable from a Yosemitch Y4000 multi-parameter sonde
 * - Range is 0.1~1000 NTU
 * - Accuracy is ＜5% or 0.3NTU
 *
 * {{ @ref YosemitechY4000_Turbidity::YosemitechY4000_Turbidity }}
 */
/**@{*/
/// @brief Decimals places in string representation; turbidity should have 2 -
/// resolution is 0.01 NTU.
#define Y4000_TURB_RESOLUTION 2
/// @brief Sensor variable number; turbidity is stored in sensorValues[1].
#define Y4000_TURB_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "turbidity"
#define Y4000_TURB_VAR_NAME "turbidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "nephelometricTurbidityUnit" (NTU)
#define Y4000_TURB_UNIT_NAME "nephelometricTurbidityUnit"
/// @brief Default variable short code; "Y4000Turbidity"
#define Y4000_TURB_DEFAULT_CODE "Y4000Turbidity"
/**@}*/

/**
 * @anchor sensor_y4000_cond
 * @name Conductivity
 * The conductivity variable from a Yosemitch Y4000 multi-parameter sonde
 * - Range is 1 µS/cm to 200 mS/cm
 * - Accuracy is ± 1 % Full Scale
 *
 * {{ @ref YosemitechY4000_Cond::YosemitechY4000_Cond }}
 */
/**@{*/
/// @brief Decimals places in string representation; conductivity should have 1
/// - resolution is 0.1 µS/cm.
#define Y4000_COND_RESOLUTION 1
/// @brief Sensor variable number; conductivity is stored in sensorValues[2].
#define Y4000_COND_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "specificConductance"
#define Y4000_COND_VAR_NAME "specificConductance"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "microsiemenPerCentimeter" (µS/cm)
#define Y4000_COND_UNIT_NAME "microsiemenPerCentimeter"
/// @brief Default variable short code; "Y4000Cond"
#define Y4000_COND_DEFAULT_CODE "Y4000Cond"
/**@}*/

/**
 * @anchor sensor_y4000_ph
 * @name pH
 * The pH variable from a Yosemitch Y4000 multi-parameter sonde
 * - Range is 2 to 12 pH units
 * - Accuracy is ± 0.1 pH units
 *
 * {{ @ref YosemitechY4000_pH::YosemitechY4000_pH }}
 */
/**@{*/
/// @brief Decimals places in string representation; ph should have 2 -
/// resolution is 0.01 pH units.
#define Y4000_PH_RESOLUTION 2
/// @brief Sensor variable number; pH is stored in sensorValues[3].
#define Y4000_PH_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/); "pH"
#define Y4000_PH_VAR_NAME "pH"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "pH"
/// (dimensionless pH units)
#define Y4000_PH_UNIT_NAME "pH"
/// @brief Default variable short code; "Y4000pH"
#define Y4000_PH_DEFAULT_CODE "Y4000pH"
/**@}*/

/**
 * @anchor sensor_y4000_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y4000 multi-parameter sonde
 * - Range is 0°C to + 50°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY4000_Temp::YosemitechY4000_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define Y4000_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[4].
#define Y4000_TEMP_VAR_NUM 4
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y4000_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y4000_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y4000Temp"
#define Y4000_TEMP_DEFAULT_CODE "Y4000Temp"
/**@}*/

/**
 * @anchor sensor_y4000_orp
 * @name ORP
 * The ORP variable from a Yosemitch Y4000 multi-parameter sonde
 * - Range is -999 ~ 999 mV
 * - Accuracy is ± 20 mV
 *
 * {{ @ref YosemitechY4000_ORP::YosemitechY4000_ORP }}
 */
/**@{*/
/// @brief Decimals places in string representation; orp should have 0 -
/// resolution is 1 mV.
#define Y4000_ORP_RESOLUTION 0
/// @brief Sensor variable number; ORP is stored in sensorValues[5].
#define Y4000_ORP_VAR_NUM 5
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "ORP"
#define Y4000_ORP_VAR_NAME "ORP"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "millivolt"
/// (mV)
#define Y4000_ORP_UNIT_NAME "millivolt"
/// @brief Default variable short code; "Y4000Potential"
#define Y4000_ORP_DEFAULT_CODE "Y4000Potential"
/**@}*/

/**
 * @anchor sensor_y4000_chloro
 * @name Chlorophyll Concentration
 * The chlorophyll concentration variable from a Yosemitch Y4000 multi-parameter
 * sonde
 * - Range is 0 to 400 µg/L or 0 to 100 RFU
 * - Accuracy is ± 1 %
 *
 * {{ @ref YosemitechY4000_Chlorophyll::YosemitechY4000_Chlorophyll }}
 */
/**@{*/
/// @brief Decimals places in string representation; chlorophyll concentration
/// should have 1 - resolution is 0.1 µg/L / 0.1 RFU.
#define Y4000_CHLORO_RESOLUTION 1
/// @brief Sensor variable number; chlorophyll concentration is stored in
/// sensorValues[6].
#define Y4000_CHLORO_VAR_NUM 6
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "chlorophyll_a_b_c"
#define Y4000_CHLORO_VAR_NAME "chlorophyll_a_b_c"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "microgramPerLiter" (µg/L)
#define Y4000_CHLORO_UNIT_NAME "microgramPerLiter"
/// @brief Default variable short code; "Y4000Chloro"
#define Y4000_CHLORO_DEFAULT_CODE "Y4000Chloro"
/**@}*/

/**
 * @anchor sensor_y4000_bga
 * @name Blue Green Algae Concentration
 * The BGA variable from a Yosemitch Y4000 multi-parameter sonde
 * - Range is 0 to 100 µg/L or 0 to 100 RFU
 * - Accuracy is ±  0.04ug/L PC
 *
 * {{ @ref YosemitechY4000_BGA::YosemitechY4000_BGA }}
 */
/**@{*/
/// @brief Decimals places in string representation; bga should have 2 -
/// resolution is 0.01 µg/L / 0.01 RFU.
#define Y4000_BGA_RESOLUTION 2
/// @brief Sensor variable number; BGA is stored in sensorValues[7].
#define Y4000_BGA_VAR_NUM 7
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "blueGreenAlgaeCyanobacteriaPhycocyanin"
#define Y4000_BGA_VAR_NAME "blueGreenAlgaeCyanobacteriaPhycocyanin"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "microgramPerLiter" (µg/L)
#define Y4000_BGA_UNIT_NAME "microgramPerLiter"
/// @brief Default variable short code; "Y4000BGA"
#define Y4000_BGA_DEFAULT_CODE "Y4000BGA"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y4000 multiparameter sonde](@ref sensor_y4000).
 *
 * @ingroup sensor_y4000
 */
/* clang-format on */
class YosemitechY4000 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y4000 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
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
                           Y4000_MEASUREMENT_TIME_MS,
                           Y4000_INC_CALC_VARIABLES) {}
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
                           Y4000_MEASUREMENT_TIME_MS,
                           Y4000_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Yosemitech Y4000 object
     */
    ~YosemitechY4000() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen concentration output](@ref sensor_y4000_domgl) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref sensor_y4000).
 *
 * @ingroup sensor_y4000
 */
/* clang-format on */
class YosemitechY4000_DOmgL : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_DOmgL object.
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
 * [turbidity output](@ref sensor_y4000_turb) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref sensor_y4000).
 *
 * @ingroup sensor_y4000
 */
/* clang-format on */
class YosemitechY4000_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Turbidity object.
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
 * [conductivity output](@ref sensor_y4000_cond) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref sensor_y4000).
 *
 * @ingroup sensor_y4000
 */
/* clang-format on */
class YosemitechY4000_Cond : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Cond object.
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
 * [pH output](@ref sensor_y4000_ph)
 * from a [Yosemitech Y4000 multiparameter sonde](@ref sensor_y4000).
 *
 * @ingroup sensor_y4000
 */
/* clang-format on */
class YosemitechY4000_pH : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_pH object.
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
 * [temperature output](@ref sensor_y4000_temp) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref sensor_y4000).
 *
 * @ingroup sensor_y4000
 */
/* clang-format on */
class YosemitechY4000_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Temp object.
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
 * [electrode electrical potential output](@ref sensor_y4000_orp) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref sensor_y4000).
 *
 * @ingroup sensor_y4000
 */
/* clang-format on */
class YosemitechY4000_ORP : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_ORP object.
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
 * [chlorophyll concentration output](@ref sensor_y4000_chloro) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref sensor_y4000)
 *
 * @ingroup sensor_y4000
 */
/* clang-format on */
class YosemitechY4000_Chlorophyll : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_Chlorophyll object.
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
 * [blue green algae (BGA) concentration output](@ref sensor_y4000_bga) from a
 * [Yosemitech Y4000 multiparameter sonde](@ref sensor_y4000).
 *
 * @ingroup sensor_y4000
 */
/* clang-format on */
class YosemitechY4000_BGA : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY4000_BGA object.
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
/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY4000_H_
