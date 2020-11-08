/**
 * @file Decagon5TM.h
 * @copyright 2020 Stroud Water Research Center
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
 * @defgroup fivetm_group Meter ECH2O (5TM)
 * Classes for the Meter ECH2O (5TM) soil moisture sensor.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section fivetm_intro Introduction
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
 * @section fivetm_datasheet Sensor Datasheet
 * [Datasheet](http://publications.metergroup.com/Manuals/20431_EC-5_Manual_Web.pdf)
 *
 * ___
 * @section fivetm_examples Example Code
 * The Meter ECH2O (5TM) is used in the @menulink{fivetm} example.
 *
 * @menusnip{fivetm}
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

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines
/** @ingroup fivetm_group */
/**@{*/

/// @brief Sensor::_numReturnedValues; the 5TM can report 3 values.
#define TM_NUM_VARIABLES 3

/**
 * @anchor fivetm_timing_defines
 * @name Sensor Timing
 * Defines for the sensor timing for a Meter ECH2O
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; maximum warm-up time in SDI-12 mode: 200ms
#define TM_WARM_UP_TIME_MS 200
/// @brief Sensor::_stabilizationTime_ms; the 5TM is stable as soon as it warms
/// up (0ms stabilization).
#define TM_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; maximum measurement duration: 200ms
#define TM_MEASUREMENT_TIME_MS 200
/**@}*/

/**
 * @anchor fivetm_ea_defines
 * @name EA
 * Defines for the EA variable from a Meter ECH2O
 * - Range is 0 – 1 m3/m3 (0 – 100% VWC)
 * - Accuracy for generic calibration equation: ± 0.03 m3/m3 (± 3% VWC) typical
 * - Accuracy for medium-specific calibration: ± 0.02 m3/m3 (± 2% VWC)
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
/// @brief Variable number; EA is stored in sensorValues[0].
#define TM_EA_VAR_NUM 0
/// @brief Variable name; "permittivity"
#define TM_EA_VAR_NAME "permittivity"
/// @brief Variable unit name; "faradPerMeter" (F/m)
#define TM_EA_UNIT_NAME "faradPerMeter"
/// @brief Default variable short code; "SoilEa"
#define TM_EA_DEFAULT_CODE "SoilEa"
/**@}*/

/**
 * @anchor fivetm_temp_defines
 * @name Temperature
 * Defines for the temperature variable from a Meter ECH2O
 * - Range is - 40°C to + 50°C
 * - Accuracy is ± 1°C
 */
/**
 * @brief Decimals places in string representation; temperature should have 2
 *
 * 1 is reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.1°C.
 */
#define TM_TEMP_RESOLUTION 2
/// @brief Variable number; temperature is stored in sensorValues[1].
#define TM_TEMP_VAR_NUM 1
/// @brief Variable name; "temperature"
#define TM_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name; "degreeCelsius"
#define TM_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "SoilTemp"
#define TM_TEMP_DEFAULT_CODE "SoilTemp"
/**@}*/

/**
 * @anchor fivetm_vwc_defines
 * @name Volumetric Water Content
 * Defines for the VWC variable from a Meter ECH2O
 * - Range is 0 – 1 m3/m3 (0 – 100% VWC)
 * - Accuracy for Generic calibration equation: ± 0.03 m3/m3 (± 3% VWC) typ
 * - Accuracy for Medium Specific Calibration: ± 0.02 m3/m3 (± 2% VWC)
 */
/**
 * @brief Decimals places in string representation; VWC should have 3
 *
 * 2 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.0008 m3/m3 (0.08% VWC)
 * from 0 – 50% VWC.
 */
#define TM_VWC_RESOLUTION 3
/// @brief Variable number; VWC is stored in sensorValues[2].
#define TM_VWC_VAR_NUM 2
/// @brief Variable name; "volumetricWaterContent"
#define TM_VWC_VAR_NAME "volumetricWaterContent"
/// @brief Variable unit name; "percent" - volumetric percent water content (%,
/// m3/100m3)
#define TM_VWC_UNIT_NAME "percent"
/// @brief Default variable short code; "SoilVWC"
#define TM_VWC_DEFAULT_CODE "SoilVWC"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Meter ECH2O soil moisture sensors](@ref fivetm_group).
 *
 * These were formerly sold as the Decagon 5TM.
 *
 * @ingroup fivetm_group
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
     * @ingroup fivetm_group
     * @param SDI12address The SDI-12 address of the ECH2O.
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
                       TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc Decagon5TM::Decagon5TM
     * @ingroup fivetm_group
     */
    Decagon5TM(char* SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "Decagon5TM", TM_NUM_VARIABLES, TM_WARM_UP_TIME_MS,
                       TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc Decagon5TM::Decagon5TM
     * @ingroup fivetm_group
     */
    Decagon5TM(int SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "Decagon5TM", TM_NUM_VARIABLES, TM_WARM_UP_TIME_MS,
                       TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Decagon 5TM object
     */
    ~Decagon5TM() {}

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;
};


// Defines the Ea/Matric Potential Variable
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [apparent dielectric permittivity (εa, matric potential) output](@ref fivetm_ea)
 * from a [Meter ECH20 or Decagon 5TM soil moisture probe](@ref fivetm_group).
 *
 * @ingroup fivetm_group
 */
/* clang-format on */
class Decagon5TM_Ea : public Variable {
 public:
    /**
     * @brief Construct a new Decagon5TM_Ea object.
     * @ingroup fivetm_group
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
 * [temperature output](@ref fivetm_temp) output from a
 * [Meter ECH20 or Decagon 5TM soil moisture probe](@ref fivetm_group).
 *
 * @ingroup fivetm_group
 */
/* clang-format on */
class Decagon5TM_Temp : public Variable {
 public:
    /**
     * @brief Construct a new Decagon5TM_Temp object.
     * @ingroup fivetm_group
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
 * [volumetric water content](@ref fivetm_vwc) output from a
 * [Meter ECH20 or Decagon 5TM soil moisture probe](@ref fivetm_group).
 *
 * @ingroup fivetm_group
 */
/* clang-format on */
class Decagon5TM_VWC : public Variable {
 public:
    /**
     * @brief Construct a new Decagon5TM_VWC object.
     * @ingroup fivetm_group
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
