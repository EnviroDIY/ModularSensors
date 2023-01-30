/**
 * @file MeterAtmos14.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the MeterAtmos14 subclass of the SDI12Sensors class along
 * with the variable subclasses MeterAtmos14_Cond, MeterAtmos14_Temp, and
 * MeterAtmos14_Depth.
 *
 * These are used for the Meter Atmos 14.
 *
 * This depends on the EnviroDIY SDI-12 library and the SDI12Sensors super
 * class.
 *
 * @note  This is identical to DecagonCTD.h in everything but the names.
 */
/* clang-format off */
/**
 * @defgroup sensor_atmos14 Meter Atmos 14
 * Classes for the Meter Atmos 14 conductivity, temperature, and depth sensor.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_atmos14_intro Introduction
 *
 * > A compact 3.4 cm diameter sensor that fits into tight spaces, the
 * > HYDROS 14 is a low-cost, durable, and easy-to-use tool for monitoring EC,
 * > temperature, and depth in both groundwater and surface water.
 *
 * @note Meter Environmental was formerly known as Decagon Devices and sold a
 * very similar sensor to the current Atmos 14 as the CTD-10.
 *
 * The Atmos 14 is implemented as a sub-classes of the SDI12Sensors class.
 * It requires a 3.5-12V power supply, which can be turned off between
 * measurements. While contrary to the manual, they will run with power as low
 * as 3.3V.
 *
 * @note  This is identical to Meter Atmos 14 in everything but the names.
 *
 * @warning Coming from the factory, METER sensors are set at SDI-12 address
 * '0'.  They also output a "DDI" serial protocol string on each power up.
 * This library *disables the DDI output string* on all newer METER sensors
 * that support disabling it.  After using a METER sensor with ModularSensors,
 * you will need to manually re-enable the DDI output if you wish to use it.
 *
 * @section sensor_atmos14_datasheet Sensor Datasheet
 * Documentation for the SDI-12 Protocol commands and responses for the Atmos
 * 14 can be found at: http://publications.metergroup.com/Integrator%20Guide/18414%20ATMOS%2014%20Gen2%20Integrator%20Guide.pdf
 *
 * @section sensor_atmos14_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_atmos14_ctor Sensor Constructor
 * {{ @ref MeterAtmos14::MeterAtmos14 }}
 *
 * ___
 * @section sensor_atmos14_examples Example Code
 * The Meter Atmos14 is used in the @menulink{meter_atmos14} example.
 *
 * @menusnip{meter_atmos14}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_METERATMOS14_H_
#define SRC_SENSORS_METERATMOS14_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

/** @ingroup sensor_atmos14 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Atmos 14 can report 3 values.
#define ATMOS14_NUM_VARIABLES 4
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define ATMOS14_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_atmos14_timing
 * @name Sensor Timing
 * The sensor timing for a Meter Atmos 14
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; maximum warm-up time in SDI-12 mode: 500ms
#define ATMOS14_WARM_UP_TIME_MS 500
/// @brief Sensor::_stabilizationTime_ms; the Atmos 14 is stable as soon as it
/// warms up (0ms stabilization).
#define ATMOS14_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; maximum measurement duration: 500ms.
#define ATMOS14_MEASUREMENT_TIME_MS 500
/// @brief Extra wake time required for an SDI-12 sensor between the "break"
/// and the time the command is sent.  The Atmos 14 requires no extra time.
#define ATMOS14_EXTRA_WAKE_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_atmos14_vp
 * @name Conductivity
 * The vapor pressure variable from a Meter Atmos 14
 * - Range is 0 – 47 kPa
 * - Accuracy is variable across a range of temperatures and RH
 *
 * {{ @ref MeterAtmos14_vp::MeterAtmos14_vp }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; vapor pressure should have 2.
 *
 * 0 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.01 kPa
 */
#define ATMOS14_VP_RESOLUTION 2
/// @brief Sensor variable number; vapor pressure is stored in sensorValues[1].
#define ATMOS14_VP_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "specificConductance"
#define ATMOS14_VP_VAR_NAME "vaporPressure"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "pascal"
#define ATMOS14_VP_UNIT_NAME "Millibar"
/// @brief Default variable short code; "Atmos14vp"
#define ATMOS14_VP_DEFAULT_CODE "Atmos14vp"
/**@}*/

/**
 * @anchor sensor_atmos14_temp
 * @name Temperature
 * The temperature variable from a Meter Atmos 14
 * - Range is -40°C to +80°C
 * - Accuracy is ±0.2°C
 *
 * {{ @ref MeterAtmos14_Temp::MeterAtmos14_Temp }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; temperature should have 2.
 *
 * 1 is reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging  - resolution is 0.1°C
 */
#define ATMOS14_ATEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define ATMOS14_ATEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define ATMOS14_ATEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define ATMOS14_ATEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Atmos14temp"
#define ATMOS14_ATEMP_DEFAULT_CODE "Atmos14temp"
/**@}*/

/**
 * @anchor sensor_atmos14_RH
 * @name Relative Humidity
 * The relative humidity variable from a Meter Atmos 14
 * - Range is 0 to 100 %
 * - Accuracy is is variable across a range of RH. Refer to Figure 2.
 *
 * {{ @ref MeterAtmos14_RH::MeterAtmos14_RH }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; rh should have 1.
 *
 * 0 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 2 mm
 */
#define ATMOS14_RH_RESOLUTION 2
/// @brief Sensor variable number; depth is stored in sensorValues[0].
#define ATMOS14_RH_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "waterDepth"
#define ATMOS14_RH_VAR_NAME "relativeHumidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "percent"
#define ATMOS14_RH_UNIT_NAME "percent"
/// @brief Default variable short code; "Atmos14rh"
#define ATMOS14_RH_DEFAULT_CODE "Atmos14rh"
/**@}*/


/**
 * @anchor sensor_atmos14_aPress
 * @name Atmospheric Pressure
 * The atmospheric pressure variable from a Meter Atmos 14
 * - Range is 1 to 120 kPa
 * - Accuracy +/- 0.05 kPa at 25C.
 *
 * {{ @ref MeterAtmos14_AtmsPress::MeterAtmos14_aPress }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; rh should have 1.
 *
 * 0 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 2 mm
 */
#define ATMOS14_APRESS_RESOLUTION 2
/// @brief Sensor variable number; depth is stored in sensorValues[0].
#define ATMOS14_APRESS_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "waterDepth"
#define ATMOS14_APRESS_VAR_NAME "atmsPressure"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "kilopascal"
#define ATMOS14_APRESS_UNIT_NAME "Millibar"
/// @brief Default variable short code; "Atmos14press"
#define ATMOS14_APRESS_DEFAULT_CODE "Atmos14press"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Meter Atmos 14 conductivity, temperature, and depth sensor](@ref sensor_atmos14)
 *
 * @ingroup sensor_atmos14
 */
/* clang-format on */
class MeterAtmos14 : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Meter Atmos 14 object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required for
     * the sensor constructor.  Optionally, you can include a number of distinct
     * readings to average.  The data pin must be a pin that supports pin-change
     * interrupts.
     *
     * @param SDI12address The SDI-12 address of the Atmos 14; can be a char,
     * char*, or int.
     * @warning The SDI-12 address **must** be changed from the factory
     * programmed value of "0" before the Atmos 14 can be used with
     * ModularSensors!
     * @param powerPin The pin on the mcu controlling power to the Atmos 14.
     * Use -1 if it is continuously powered.
     * - The Atmos 14 requires a 3.5-12V power supply, which can be turned off
     * between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    MeterAtmos14(char SDI12address, int8_t powerPin, int8_t dataPin,
                  uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "MeterAtmos14", ATMOS14_NUM_VARIABLES, ATMOS14_WARM_UP_TIME_MS,
              ATMOS14_STABILIZATION_TIME_MS, ATMOS14_MEASUREMENT_TIME_MS,
              ATMOS14_EXTRA_WAKE_TIME_MS, ATMOS14_INC_CALC_VARIABLES) {}
    /**
     * @copydoc MeterAtmos14::MeterAtmos14
     */
    MeterAtmos14(char* SDI12address, int8_t powerPin, int8_t dataPin,
                  uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "MeterAtmos14", ATMOS14_NUM_VARIABLES, ATMOS14_WARM_UP_TIME_MS,
              ATMOS14_STABILIZATION_TIME_MS, ATMOS14_MEASUREMENT_TIME_MS,
              ATMOS14_EXTRA_WAKE_TIME_MS, ATMOS14_INC_CALC_VARIABLES) {}
    /**
     * @copydoc MeterAtmos14::MeterAtmos14
     */
    MeterAtmos14(int SDI12address, int8_t powerPin, int8_t dataPin,
                  uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "MeterAtmos14", ATMOS14_NUM_VARIABLES, ATMOS14_WARM_UP_TIME_MS,
              ATMOS14_STABILIZATION_TIME_MS, ATMOS14_MEASUREMENT_TIME_MS,
              ATMOS14_EXTRA_WAKE_TIME_MS, ATMOS14_INC_CALC_VARIABLES) {}

    /**
     * @brief Destroy the Meter Atmos 14 object
     */
    ~MeterAtmos14() {}

    virtual bool getResults(void) override;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [conductivity output](@ref sensor_atmos14_cond) from a
 * [Meter Atmos 14.](@ref sensor_atmos14)
 *
 * @ingroup sensor_atmos14
 */
/* clang-format on */
class MeterAtmos14_Vpress : public Variable {
 public:
    /**
     * @brief Construct a new MeterAtmos14_Vpress object.
     *
     * @param parentSense The parent MeterAtmos14 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Atmos14Vpress".
     */
    explicit MeterAtmos14_Vpress(
        MeterAtmos14* parentSense, const char* uuid = "",
        const char* varCode = ATMOS14_VP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS14_VP_VAR_NUM,
                   (uint8_t)ATMOS14_VP_RESOLUTION, ATMOS14_VP_VAR_NAME,
                   ATMOS14_VP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterAtmos14_Vpress object.
     *
     * @note This must be tied with a parent MeterAtmos14 before it can be
     * used.
     */
    MeterAtmos14_Vpress()
        : Variable((const uint8_t)ATMOS14_VP_VAR_NUM,
                   (uint8_t)ATMOS14_VP_RESOLUTION, ATMOS14_VP_VAR_NAME,
                   ATMOS14_VP_UNIT_NAME, ATMOS14_VP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterAtmos14_Vpress object - no action needed.
     */
    ~MeterAtmos14_Vpress() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_atmos14_temp) from a
 * [Meter Atmos 14.](@ref sensor_atmos14)
 *
 * @ingroup sensor_atmos14
 */
/* clang-format on */
class MeterAtmos14_aTemp : public Variable {
 public:
    /**
     * @brief Construct a new MeterAtmos14_Temp object.
     *
     * @param parentSense The parent MeterAtmos14 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Atmos14temp".
     */
    explicit MeterAtmos14_aTemp(
        MeterAtmos14* parentSense, const char* uuid = "",
        const char* varCode = ATMOS14_ATEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS14_ATEMP_VAR_NUM,
                   (uint8_t)ATMOS14_ATEMP_RESOLUTION, ATMOS14_ATEMP_VAR_NAME,
                   ATMOS14_ATEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterAtmos14_Temp object.
     *
     * @note This must be tied with a parent MeterAtmos14 before it can be
     * used.
     */
    MeterAtmos14_aTemp()
        : Variable((const uint8_t)ATMOS14_ATEMP_VAR_NUM,
                   (uint8_t)ATMOS14_ATEMP_RESOLUTION, ATMOS14_ATEMP_VAR_NAME,
                   ATMOS14_ATEMP_UNIT_NAME, ATMOS14_ATEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterAtmos14_Temp object - no action needed.
     */
    ~MeterAtmos14_aTemp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [depth output](@ref sensor_atmos14_depth) from a
 * [Meter Atmos 14.](@ref sensor_atmos14)
 *
 * @ingroup sensor_atmos14
 */
/* clang-format on */
class MeterAtmos14_aPress : public Variable {
 public:
    /**
     * @brief Construct a new MeterAtmos14_aPress object.
     *
     * @param parentSense The parent MeterAtmos14 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Atmos14aPress".
     */
    explicit MeterAtmos14_aPress(
        MeterAtmos14* parentSense, const char* uuid = "",
        const char* varCode = ATMOS14_APRESS_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS14_APRESS_VAR_NUM,
                   (uint8_t)ATMOS14_APRESS_RESOLUTION, ATMOS14_APRESS_VAR_NAME,
                   ATMOS14_APRESS_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterAtmos14_aPress object.
     *
     * @note This must be tied with a parent MeterAtmos14 before it can be
     * used.
     */
    MeterAtmos14_aPress()
        : Variable((const uint8_t)ATMOS14_APRESS_VAR_NUM,
                   (uint8_t)ATMOS14_APRESS_RESOLUTION, ATMOS14_APRESS_VAR_NAME,
                   ATMOS14_APRESS_UNIT_NAME, ATMOS14_APRESS_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterAtmos14_aPress object - no action needed.
     */
    ~MeterAtmos14_aPress() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [depth output](@ref sensor_atmos14_depth) from a
 * [Meter Atmos 14 sensor.](@ref sensor_atmos14)
 *
 * @ingroup sensor_atmos14
 */
/* clang-format on */
class MeterAtmos14_RH : public Variable {
 public:
    /**
     * @brief Construct a new MeterAtmos14_RH object.
     *
     * @param parentSense The parent MeterAtmos14 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Atmos14RH ".
     */
    explicit MeterAtmos14_RH(
        MeterAtmos14* parentSense, const char* uuid = "",
        const char* varCode = ATMOS14_RH_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS14_RH_VAR_NUM,
                   (uint8_t)ATMOS14_RH_RESOLUTION, ATMOS14_RH_VAR_NAME,
                   ATMOS14_RH_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterAtmos14_RH object.
     *
     * @note This must be tied with a parent MeterAtmos14 before it can be
     * used.
     */
    MeterAtmos14_RH()
        : Variable((const uint8_t)ATMOS14_RH_VAR_NUM,
                   (uint8_t)ATMOS14_RH_RESOLUTION, ATMOS14_RH_VAR_NAME,
                   ATMOS14_RH_UNIT_NAME, ATMOS14_RH_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterAtmos14_RH  object - no action needed.
     */
    ~MeterAtmos14_RH() {}
};
/**@}*/
#endif  // SRC_SENSORS_METERATMOS14_H_
