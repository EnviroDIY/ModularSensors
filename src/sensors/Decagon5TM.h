/**
 * @file Decagon5TM.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the Decagon5TM subclass of the SDI12Sensors class along with
 * the variable subclasses Decagon5TM_Ea, Decagon5TM_Temp, and Decagon5TM_VWC.
 *
 * These are used for the Meter ECH20 or the discontinued Decagon Devices 5TM
 * Soil Moisture probe.
 *
 * This depends on the EnviroDIY SDI-12 library and the SDI12Sensors super
 * class.
 */
/* clang-format off */
/**
 * @defgroup sensor_fivetm Meter ECH2O (5TM)
 * Classes for the Meter ECH2O (5TM) soil moisture sensor.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_fivetm_intro Introduction
 *
 * Meter Environmental makes two series of soil moisture sensors, the
 * [ECH2O series](https://www.metergroup.com/environment/products/?product_category=9525) and the
 * [Teros series](https://www.metergroup.com/environment/products/teros-12/).
 * __This page is for the ECH2O series.__
 *
 * @note Meter Environmental was formerly known as Decagon Devices and sold a
 * very similar sensor to the current EC-5 sensor as the 5TM.
 *
 * Both series of sensors operate as sub-classes of the SDI12Sensors class.
 * They require a 3.5-12V power supply, which can be turned off between
 * measurements. While contrary to the manual, they will run with power as low
 * as 3.3V. On the 5TM with a stereo cable, the power is connected to the tip,
 * data to the ring, and ground to the sleeve. On the bare-wire version, the
 * power is connected to the _white_ cable, data to _red_, and ground to the
 * unshielded cable.
 *
 * @section sensor_fivetm_datasheet Sensor Datasheet
 * [Datasheet](http://publications.metergroup.com/Manuals/20431_EC-5_Manual_Web.pdf)
 *
 * @section sensor_fivetm_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_fivetm_ctor Sensor Constructor
 * {{ @ref Decagon5TM::Decagon5TM }}
 *
 * ___
 * @section sensor_fivetm_examples Example Code
 * The Meter ECH2O (5TM) is used in the @menulink{decagon_5tm} example.
 *
 * @menusnip{decagon_5tm}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_DECAGON5TM_H_
#define SRC_SENSORS_DECAGON5TM_H_

// Debugging Statement
// #define MS_DECAGON5TM_DEBUG

#ifdef MS_DECAGON5TM_DEBUG
#define MS_DEBUGGING_STD "Decagon5TM"
#endif

#ifdef MS_SDI12SENSORS_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "SDI12Sensors"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "VariableBase.h"
#include "sensors/SDI12Sensors.h"

/** @ingroup sensor_fivetm */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the 5TM can report 3 values.
#define TM_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; volumetric water content is calculated from
/// the permittivity and the temperature.
#define TM_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_fivetm_timing
 * @name Sensor Timing
 * The sensor timing for a Meter ECH2O
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; maximum warm-up time in SDI-12 mode: 200ms
#define TM_WARM_UP_TIME_MS 200
/// @brief Sensor::_stabilizationTime_ms; the 5TM is stable as soon as it warms
/// up (0ms stabilization).
#define TM_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; maximum measurement duration: 200ms
#define TM_MEASUREMENT_TIME_MS 200
/// @brief Extra wake time required for an SDI-12 sensor between the "break" and
/// the time the command is sent.  The 5TM requires no extra time.
#define TM_EXTRA_WAKE_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_fivetm_ea
 * @name EA
 * The EA variable from a Meter ECH2O
 * - Range is 0 – 1 m3/m3 (0 – 100% VWC)
 * - Accuracy for generic calibration equation: ± 0.03 m3/m3 (± 3% VWC) typical
 * - Accuracy for medium-specific calibration: ± 0.02 m3/m3 (± 2% VWC)
 *
 * {{ @ref Decagon5TM_Ea::Decagon5TM_Ea }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; EA should have 5
 *
 * 4 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.0008 m3/m3 (0.08% VWC)
 * from 0 – 50% VWC.
 */
#define TM_EA_RESOLUTION 5
/// @brief Sensor variable number; EA is stored in sensorValues[0].
#define TM_EA_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "permittivity"
#define TM_EA_VAR_NAME "permittivity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "faradPerMeter" (F/m)
#define TM_EA_UNIT_NAME "faradPerMeter"
/// @brief Default variable short code; "SoilEa"
#define TM_EA_DEFAULT_CODE "SoilEa"
/**@}*/

/**
 * @anchor sensor_fivetm_temp
 * @name Temperature
 * The temperature variable from a Meter ECH2O
 * - Range is - 40°C to + 50°C
 * - Accuracy is ± 1°C
 *
 * {{ @ref Decagon5TM_Temp::Decagon5TM_Temp }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; temperature should have 2
 *
 * 1 is reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.1°C.
 */
#define TM_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define TM_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define TM_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define TM_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "SoilTemp"
#define TM_TEMP_DEFAULT_CODE "SoilTemp"
/**@}*/

/**
 * @anchor sensor_fivetm_vwc
 * @name Volumetric Water Content
 * The VWC variable from a Meter ECH2O
 * - Range is 0 – 1 m3/m3 (0 – 100% VWC)
 * - Accuracy for Generic calibration equation: ± 0.03 m3/m3 (± 3% VWC) typ
 * - Accuracy for Medium Specific Calibration: ± 0.02 m3/m3 (± 2% VWC)
 *
 * {{ @ref Decagon5TM_VWC::Decagon5TM_VWC }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; VWC should have 3
 *
 * 2 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.0008 m3/m3 (0.08% VWC)
 * from 0 – 50% VWC.
 */
#define TM_VWC_RESOLUTION 3
/// @brief Sensor variable number; VWC is stored in sensorValues[2].
#define TM_VWC_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "volumetricWaterContent"
#define TM_VWC_VAR_NAME "volumetricWaterContent"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "percent" -
/// volumetric percent water content (%, m3/100m3)
#define TM_VWC_UNIT_NAME "percent"
/// @brief Default variable short code; "SoilVWC"
#define TM_VWC_DEFAULT_CODE "SoilVWC"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Meter ECH2O soil moisture sensors](@ref sensor_fivetm).
 *
 * These were formerly sold as the Decagon 5TM.
 *
 * @ingroup sensor_fivetm
 */
/* clang-format on */
class Decagon5TM : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Decagon 5TM object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required for
     * the sensor constructor.  Optionally, you can include a number of distinct
     * readings to average.  The data pin must be a pin that supports pin-change
     * interrupts.
     *
     * @param SDI12address The SDI-12 address of the ECH2O; can be a char,
     * char*, or int.
     * @warning The SDI-12 address **must** be changed from the factory
     * programmed value of "0" before the ECH2O can be used with
     * ModularSensors!
     * @param powerPin The pin on the mcu controlling power to the ECH2O
     * Use -1 if it is continuously powered.
     * - The ECH2O requires a 3.5-12V power supply, which can be turned off
     * between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    Decagon5TM(char SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "Decagon5TM", TM_NUM_VARIABLES, TM_WARM_UP_TIME_MS,
                       TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS,
                       TM_EXTRA_WAKE_TIME_MS, TM_INC_CALC_VARIABLES) {}
    /**
     * @copydoc Decagon5TM::Decagon5TM
     */
    Decagon5TM(char* SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "Decagon5TM", TM_NUM_VARIABLES, TM_WARM_UP_TIME_MS,
                       TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS,
                       TM_EXTRA_WAKE_TIME_MS, TM_INC_CALC_VARIABLES) {}
    /**
     * @copydoc Decagon5TM::Decagon5TM
     */
    Decagon5TM(int SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "Decagon5TM", TM_NUM_VARIABLES, TM_WARM_UP_TIME_MS,
                       TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS,
                       TM_EXTRA_WAKE_TIME_MS, TM_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Decagon 5TM object
     */
    ~Decagon5TM() {}

    /**
     * @copydoc SDI12Sensors::getResults()
     */
    bool getResults(void) override;
};


// Defines the Ea/Matric Potential Variable
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [apparent dielectric permittivity (εa, matric potential) output](@ref sensor_fivetm_ea)
 * from a [Meter ECH20 or Decagon 5TM soil moisture probe](@ref sensor_fivetm).
 *
 * @ingroup sensor_fivetm
 */
/* clang-format on */
class Decagon5TM_Ea : public Variable {
 public:
    /**
     * @brief Construct a new Decagon5TM_Ea object.
     *
     * @param parentSense The parent Decagon5TM providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilEa".
     */
    explicit Decagon5TM_Ea(Decagon5TM* parentSense, const char* uuid = "",
                           const char* varCode = TM_EA_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)TM_EA_VAR_NUM,
                   (uint8_t)TM_EA_RESOLUTION, TM_EA_VAR_NAME, TM_EA_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new Decagon5TM_Ea object.
     *
     * @note This must be tied with a parent Decagon5TM before it can be used.
     */
    Decagon5TM_Ea()
        : Variable((const uint8_t)TM_EA_VAR_NUM, (uint8_t)TM_EA_RESOLUTION,
                   TM_EA_VAR_NAME, TM_EA_UNIT_NAME, TM_EA_DEFAULT_CODE) {}
    /**
     * @brief Destroy the Decagon5TM_Ea object - no action needed.
     */
    ~Decagon5TM_Ea() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_fivetm_temp) output from a
 * [Meter ECH20 or Decagon 5TM soil moisture probe](@ref sensor_fivetm).
 *
 * @ingroup sensor_fivetm
 */
/* clang-format on */
class Decagon5TM_Temp : public Variable {
 public:
    /**
     * @brief Construct a new Decagon5TM_Temp object.
     *
     * @param parentSense The parent Decagon5TM providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilTemp".
     */
    explicit Decagon5TM_Temp(Decagon5TM* parentSense, const char* uuid = "",
                             const char* varCode = TM_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)TM_TEMP_VAR_NUM,
                   (uint8_t)TM_TEMP_RESOLUTION, TM_TEMP_VAR_NAME,
                   TM_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new Decagon5TM_Temp object.
     *
     * @note This must be tied with a parent Decagon5TM before it can be used.
     */
    Decagon5TM_Temp()
        : Variable((const uint8_t)TM_TEMP_VAR_NUM, (uint8_t)TM_TEMP_RESOLUTION,
                   TM_TEMP_VAR_NAME, TM_TEMP_UNIT_NAME, TM_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the Decagon5TM_Temp object - no action needed.
     */
    ~Decagon5TM_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [volumetric water content](@ref sensor_fivetm_vwc) output from a
 * [Meter ECH20 or Decagon 5TM soil moisture probe](@ref sensor_fivetm).
 *
 * @ingroup sensor_fivetm
 */
/* clang-format on */
class Decagon5TM_VWC : public Variable {
 public:
    /**
     * @brief Construct a new Decagon5TM_VWC object.
     *
     * @param parentSense The parent Decagon5TM providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilVWC".
     */
    explicit Decagon5TM_VWC(Decagon5TM* parentSense, const char* uuid = "",
                            const char* varCode = TM_VWC_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)TM_VWC_VAR_NUM,
                   (uint8_t)TM_VWC_RESOLUTION, TM_VWC_VAR_NAME,
                   TM_VWC_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new Decagon5TM_VWC object.
     *
     * @note This must be tied with a parent Decagon5TM before it can be used.
     */
    Decagon5TM_VWC()
        : Variable((const uint8_t)TM_VWC_VAR_NUM, (uint8_t)TM_VWC_RESOLUTION,
                   TM_VWC_VAR_NAME, TM_VWC_UNIT_NAME, TM_VWC_DEFAULT_CODE) {}
    /**
     * @brief Destroy the Decagon5TM_VWC object - no action needed.
     */
    ~Decagon5TM_VWC() {}
};
/**@}*/
#endif  // SRC_SENSORS_DECAGON5TM_H_
