/**
 * @file MeterTeros11.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the MeterTeros11 sensor subclass and the variable subclasses
 * MeterTeros11_Ea, MeterTeros11_Temp, and MeterTeros11_VWC.
 *
 * These are for the Meter Teros 11 Advanced Soil Moisture probe.
 *
 * This depends on the EnviroDIY SDI-12 library and the SDI12Sensors super
 * class.
 */
/* clang-format off */
/**
 * @defgroup teros_group Meter Teros 11
 * Classes for the Meter Teros 11 soil moisture probe.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section teros_intro Introduction
 *
 * Meter Environmental makes two series of soil moisture sensors, the
 * [ECH2O series](https://www.metergroup.com/environment/products/?product_category=9525) and the
 * [Teros series](https://www.metergroup.com/environment/products/teros-12/).
 * __This page is for the Teros series.__
 *
 * Both series of sensors operate as sub-classes of the SDI12Sensors class.
 * They require a 3.5-12V power supply, which can be turned off between
 * measurements. While contrary to the manual, they will run with power as low
 * as 3.3V. On the 5TM with a stereo cable, the power is connected to the tip,
 * data to the ring, and ground to the sleeve. On the bare-wire version, the
 * power is connected to the _white_ cable, data to _red_, and ground to the
 * unshielded cable.
 *
 * @section teros_datasheet Sensor Datasheet
 * Documentation for the SDI-12 Protocol commands and responses for the Meter
 * Teros 11 can be found at:
 * http://publications.metergroup.com/Manuals/20587_TEROS11-12_Manual_Web.pdf
 *
 * @section teros_sensor The Teros Sensor
 * @ctor_doc{MeterTeros11, char SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage}
 * @subsection teros_timing Sensor Timing
 * - Warm-up time in SDI-12 mode: 245ms typical, assume stability at warm-up
 * - Measurement duration: 25 ms to 50 ms
 * @subsection teros_voltages Voltage Ranges
 * - Supply Voltage (VCC to GND), 4.0 to 15.0 VDC
 * - Digital Input Voltage (logic high), 2.8 to 3.9 V (3.6 typical)
 * - Digital Output Voltage (logic high), 3.6 typical
 *
 * @section teros11_ea Ea Output
 *   - Range is 1 (air) to 80 (water)
 *   - Accuracy is:
 *     - 1–40 (soil range) , ±1 εa (unitless)
 *     - 40–80, 15% of measurement
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.00001
 *   - Reported as farads per meter (F/m)
 *   - Default variable code is TerosSoilEa
 * @variabledoc{teros11_ea,MeterTeros11,Ea,TerosSoilEa}
 *
 * @section teros11_temp Temperature Output
 *   - Range is -40°C to 60°C
 *   - Accuracy is:
 *     - ± 1°C, from -40°C to 0°C
 *     - ± 0.5°C, from 0°C to + 60°C
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.1°C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is TerosSoilTemp
 * @variabledoc{teros11_temp,MeterTeros11,Temp,TerosSoilTemp}
 *
 * @section teros11_vwc VWC Output
 *   - Range is:
 *     - Mineral soil calibration: 0.00–0.70 m3/m3 (0 – 70% VWC)
 *     - Soilless media calibration: 0.0–1.0 m3/m3 (0 – 100% VWC)
 *   - Accuracy is:
 *     - Generic calibration: ±0.03 m3/m3 (± 3% VWC) typical in mineral soils
 * that have solution electrical conductivity <8 dS/m
 *     - Medium specific calibration: ±0.01–0.02 m3/m3 (± 1-2% VWC)in any porous
 * medium
 *   - Result stored in sensorValues[2]
 *   - Resolution is 0.001 m3/m3 (0.1% VWC) from 0 – 70% VWC
 *   - Reported as volumetric percent water content (%, m3/100m3)
 *   - Default variable code is TerosSoilVWC
 * @variabledoc{teros11_vwc,MeterTeros11,VWC,TerosSoilVWC}
 *
 * ___
 * @section teros_examples Example Code
 * The Meter Teros is used in the @menulink{teros} example.
 *
 * @menusnip{teros}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_METERTEROS11_H_
#define SRC_SENSORS_METERTEROS11_H_

// Debugging Statement
// #define MS_MeterTeros11_DEBUG

#ifdef MS_METERTEROS11_DEBUG
#define MS_DEBUGGING_STD "MeterTeros11"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the Teros 11 can report 3 values.
#define TEROS11_NUM_VARIABLES 3
/// Sensor::_warmUpTime_ms; the Teros 11 warms up in 250ms.
#define TEROS11_WARM_UP_TIME_MS 250
/// Sensor::_stabilizationTime_ms; the Teros 11 is stable after 50ms.
#define TEROS11_STABILIZATION_TIME_MS 50
/**
 * @brief Sensor::_measurementTime_ms; the Teros 11 takes 50ms to complete a
 * measurement.
 */
#define TEROS11_MEASUREMENT_TIME_MS 50

/**
 * @brief Decimals places in string representation; EA should have 5.
 *
 * 4 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging
 */
#define TEROS11_EA_RESOLUTION 5
/// Variable number; EA is stored in sensorValues[0].
#define TEROS11_EA_VAR_NUM 0

/**
 * @brief Decimals places in string representation; temperature should have 2.
 *
 * 1 is reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging
 */
#define TEROS11_TEMP_RESOLUTION 2
/// Variable number; temperature is stored in sensorValues[1].
#define TEROS11_TEMP_VAR_NUM 1

/**
 * @brief Decimals places in string representation; VWC should have 3.
 *
 * 2 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging
 */
#define TEROS11_VWC_RESOLUTION 3
/// Variable number; VWC is stored in sensorValues[2].
#define TEROS11_VWC_VAR_NUM 2

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Meter Teros 11 sensor](@ref teros_group)
 *
 * @ingroup teros_group
 */
/* clang-format on */
class MeterTeros11 : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Meter Teros 11 object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required for
     * the sensor constructor.  Optionally, you can include a number of distinct
     * readings to average.  The data pin must be a pin that supports pin-change
     * interrupts.
     *
     * @param SDI12address The SDI-12 address of the sensor
     * @warning The SDI-12 address **must** be changed from the factory
     * programmed value of "0" before the sensor can be used with
     * ModularSensors!
     * @param powerPin The pin on the mcu controlling power to the Teros 11. Use
     * -1 if it is continuously powered.
     * - The Teros 11 requires a 3.5-12V power supply, which can be turned off
     * between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    MeterTeros11(char SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterTeros11", TEROS11_NUM_VARIABLES,
                       TEROS11_WARM_UP_TIME_MS, TEROS11_STABILIZATION_TIME_MS,
                       TEROS11_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc MeterTeros11::MeterTeros11
     */
    MeterTeros11(char* SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterTeros11", TEROS11_NUM_VARIABLES,
                       TEROS11_WARM_UP_TIME_MS, TEROS11_STABILIZATION_TIME_MS,
                       TEROS11_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc MeterTeros11::MeterTeros11
     */
    MeterTeros11(int SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterTeros11", TEROS11_NUM_VARIABLES,
                       TEROS11_WARM_UP_TIME_MS, TEROS11_STABILIZATION_TIME_MS,
                       TEROS11_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Meter Teros 11 object
     */
    ~MeterTeros11() {}

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;
};


// Defines the Ea/Matric Potential Variable
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [apparent dielectric permittivity (εa, matric potential)](@ref teros11_ea)
 * from a [Meter Teros soil moisture/water content sensor](@ref teros_group).
 *
 * @ingroup teros_group
 */
/* clang-format on */
class MeterTeros11_Ea : public Variable {
 public:
    /**
     * @brief Construct a new MeterTeros11_Ea object.
     *
     * @param parentSense The parent MeterTeros11 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilEa".
     */
    explicit MeterTeros11_Ea(MeterTeros11* parentSense, const char* uuid = "",
                             const char* varCode = "TerosSoilEa")
        : Variable(parentSense, (const uint8_t)TEROS11_EA_VAR_NUM,
                   (uint8_t)TEROS11_EA_RESOLUTION, "permittivity",
                   "faradPerMeter", varCode, uuid) {}
    /**
     * @brief Construct a new MeterTeros11_Ea object.
     *
     * @note This must be tied with a parent MeterTeros11 before it can be used.
     */
    MeterTeros11_Ea()
        : Variable((const uint8_t)TEROS11_EA_VAR_NUM,
                   (uint8_t)TEROS11_EA_RESOLUTION, "permittivity",
                   "faradPerMeter", "SoilEa") {}
    /**
     * @brief Destroy the MeterTeros11_Ea object - no action needed.
     */
    ~MeterTeros11_Ea() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref teros11_temp) output from a
 * [Teros soil moisture/water content sensor](@ref teros_group).
 *
 * @ingroup teros_group
 */
/* clang-format on */
class MeterTeros11_Temp : public Variable {
 public:
    /**
     * @brief Construct a new MeterTeros11_Temp object.
     *
     * @param parentSense The parent MeterTeros11 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilTemp".
     */
    explicit MeterTeros11_Temp(MeterTeros11* parentSense, const char* uuid = "",
                               const char* varCode = "TerosSoilTemp")
        : Variable(parentSense, (const uint8_t)TEROS11_TEMP_VAR_NUM,
                   (uint8_t)TEROS11_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new MeterTeros11_Temp object.
     *
     * @note This must be tied with a parent MeterTeros11 before it can be used.
     */
    MeterTeros11_Temp()
        : Variable((const uint8_t)TEROS11_TEMP_VAR_NUM,
                   (uint8_t)TEROS11_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "SoilTemp") {}
    /**
     * @brief Destroy the MeterTeros11_Temp object - no action needed.
     */
    ~MeterTeros11_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [volumetric water content](@ref teros11_vwc) output from a
 * [Teros soil moisture/water content sensor](@ref teros_group).
 *
 * @ingroup teros_group
 */
/* clang-format on */
class MeterTeros11_VWC : public Variable {
 public:
    /**
     * @brief Construct a new MeterTeros11_VWC object.
     *
     * @param parentSense The parent MeterTeros11 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilVWC".
     */
    explicit MeterTeros11_VWC(MeterTeros11* parentSense, const char* uuid = "",
                              const char* varCode = "TerosSoilVWC")
        : Variable(parentSense, (const uint8_t)TEROS11_VWC_VAR_NUM,
                   (uint8_t)TEROS11_VWC_RESOLUTION, "volumetricWaterContent",
                   "percent", varCode, uuid) {}
    /**
     * @brief Construct a new MeterTeros11_VWC object.
     *
     * @note This must be tied with a parent MeterTeros11 before it can be used.
     */
    MeterTeros11_VWC()
        : Variable((const uint8_t)TEROS11_VWC_VAR_NUM,
                   (uint8_t)TEROS11_VWC_RESOLUTION, "volumetricWaterContent",
                   "percent", "SoilVWC") {}
    /**
     * @brief Destroy the MeterTeros11_VWC object - no action needed.
     */
    ~MeterTeros11_VWC() {}
};

#endif  // SRC_SENSORS_METERTEROS11_H_
