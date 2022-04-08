/**
 * @file TurnerCyclops.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the TurnerCyclops sensor subclass and the variable subclasses
 * TurnerCyclops_Turbidity and TurnerCyclops_Voltage.
 *
 * These are used for the Turner Scientific Cyclops-7F.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 */
/* clang-format off */
/**
 * @defgroup sensor_cyclops Turner Cyclops-7F
 * Classes for the Turner Cyclops-7F analog turbidity sensor.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_cyclops_intro Introduction
 *
 * > Cyclops-7F Sensors are high performance, compact submersible sensors
 * > designed for integration into any platform that provides power and
 * > datalogging.
 *
 * The Cyclops sensors come pre-configured for one of 13 fluorophores and can
 * also be ordered with custom wavelength configurations.  The standard
 * configurations are:
 *
 * | ID    | Variable                     | Application                                              |
 * | ----- | ---------------------------- | -------------------------------------------------------- |
 * | **U** | TurnerCyclops_CDOM           | CDOM/FDOM                                                |
 * | **C** | TurnerCyclops_Chlorophyll    | Chl in vivo (Blue Excitation)                            |
 * | **D** | TurnerCyclops_RedChlorophyll | Chl in vivo (Red Excitation)                             |
 * | **F** | TurnerCyclops_Fluorescein    | Fluorescein Dye                                          |
 * | **O** | TurnerCyclops_CrudeOil       | Oil - Crude                                              |
 * | **G** | TurnerCyclops_BTEX           | Oil - Fine (Refined Fuels)                               |
 * | **B** | TurnerCyclops_Brighteners    | Optical Brighteners for Wastewater Monitoring            |
 * | **P** | TurnerCyclops_Phycocyanin    | Phycocyanin (Freshwater Cyanobacteria)                   |
 * | **E** | TurnerCyclops_Phycoerythrin  | Phycoerythrin (Marine Cyanobacteria)                     |
 * | **A** | TurnerCyclops_PTSA           | PTSA (1,3,6,8-Pyrenetetrasulfonic Acid Tetrasodium Salt) |
 * | **R** | TurnerCyclops_Rhodamine      | Rhodamine Dye                                            |
 * | **L** | TurnerCyclops_Tryptophan     | Tryptophan for Wastewater Monitoring                     |
 * | **T** | TurnerCyclops_Turbidity      | Turbidity                                                |
 *
 * The detection limits, ranges, and wavelengths of each configuration are:
 * | ID    | MDL       | Linear Range | LED (CWL) | Excitation | Emission   | Power @ 12V |
 * | ----- | --------- | ------------ | --------- | ---------- | ---------- | ----------- |
 * | **U** | 0.1 ppb¹  | 0-1,500 ppb¹ | 365 nm    | 325/120 nm | 470/60 nm  | 240 mW      |
 * | ^     | 0.5 ppb²  | 0-3,000 ppb² | 365 nm    | 325/120 nm | 470/60 nm  | ^           |
 * | **C** | 0.03 μg/L | 0-500 μg/L   | 460 nm    | 465/170 nm | 696/44 nm  | 240 mW      |
 * | **D** | 0.3 μg/L  | 0-500 μg/L   | 635 nm    | ≤ 635 nm   | > 695 nm   | 240         |
 * | **F** | 0.01 ppb  | 0-500 ppb    | 460 nm    | 400/150 nm | 545/28 nm  | 145 mW      |
 * | **O** | 0.2 ppb²  | 0-1,500 ppb² | 365 nm    | 325/120 nm | 410-600 nm | 250 mW      |
 * | **G** | 0.4ppm³   | 0-20 ppm³    | 255 nm    | ≤ 290 nm   | 350/50 nm  | 530 mW      |
 * | **B** | 0.6 ppb²  | 0-2,500 ppb² | 365 nm    | 325/120 nm | 445/15 nm  | 200 mW      |
 * | **P** | 2 ppb⁴    | 0-4,500 ppb⁴ | 590 nm    | 590/30 nm  | ≥ 645 nm   | 160 mW      |
 * | **E** | 0.1 ppb⁵  | 0-750 ppb⁵   | 525 nm    | 515-547 nm | ≥ 590 nm   | 270 mW      |
 * | **A** | 0.1 ppb²  | 0-650 ppb²   | 365 nm    | 325/120 nm | 405/10 nm  | 320 mW      |
 * | **R** | 0.01 ppb  | 0-1,000 ppb  | 530 nm    | 535/60 nm  | 590-715 nm | 175 mW      |
 * | **L** | 3 ppb     | 0-5,000 ppb  | 275 nm    | -          | 350/55 nm  | 540 mW      |
 * | **T** | 0.05 NTU  | 0-1,500 NTU  | 850 nm    | 850 nm     | 850 nm     | 120 mW      |
 *
 * ¹ Quinine Sulfate
 *
 * ² PTSA (1,3,6,8-Pyrenetetrasulfonic Acid Tetrasodium Salt)
 *
 * ³ BTEX (Benzene, Toluene, Ethylbenzene, Xylenes)
 *
 * ⁴ Phycocyanin pigment from Prozyme diluted in Deionized water
 *
 * ⁵ Phycoerythrin pigment from Prozyme diluted in Deionized water
 *
 * @note **The Cyclops configuration is set at the time of the sensor
 * manufacturing and cannot be changed.**  Each individual sensor is only capable
 * of measuring the single parameter it is configured for.  A Cyclops that is
 * configured to measure chlorophyll *cannot* be used to measure turbidity.
 *
 * @section sensor_cyclops_calib Calibration
 *
 * The Cyclops does *not* come pre-calibrated and must be calibrated by the
 * user after purchase.  Turner recommends using a simple 1-point calibration,
 * which is what this library supports.  Full calibration instructions are in
 * the instrument manual.
 *
 * The Cyclops-7F puts out a simple analog signal between 0 and 5V.  It has
 * three possible gain settings, 1x, 10x and 100x.  The gain setting is
 * selected by grounding the appropriate gain wire.  Because the output
 * signal can range up to 5V, if using an ADS1115 or ADS1015 powered at
 * only 3.3V, the gain must be reduced or a voltage devider used to
 * ensure the output signal does not exceed 3.6V.  This library does *not*
 * support variable gain or any type of auto-gaining for the Cyclops
 * sensors.
 *
 * @note The Cyclops should be calibrated in as close to field conditions as
 * possible.  All gain settings and voltage dividers should be in place for
 * the calibration.
 *
 * Before applying any calibration, the analog output from the Cyclops-7F
 * must be converted into a high resolution digital signal.  See the
 * [ADS1115 page](@ref analog_group) for details on the conversion.
 *
 * @section sensor_cyclops_datasheet Sensor Datasheet
 * - [Main Information Page](https://www.turnerdesigns.com/cyclops-7f-submersible-fluorometer)
 * - [Optical Specification Guide](http://docs.turnerdesigns.com/t2/doc/spec-guides/998-2181.pdf)
 * - [Manual](http://docs.turnerdesigns.com/t2/doc/manuals/998-2100.pdf)
 *
 * @section sensor_cyclops_flags Build flags
 * - ```-D MS_USE_ADS1015```
 *      - switches from the 16-bit ADS1115 to the 12 bit ADS1015
 *
 * @section sensor_cyclops_ctor Sensor Constructor
 * {{ @ref TurnerCyclops::TurnerCyclops }}
 *
 * ___
 * @section sensor_cyclops_examples Example Code
 * The Turner Cyclops-7F is used in the @menulink{turner_cyclops} example.
 *
 * @menusnip{turner_cyclops}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_TURNERCYCLOPS_H_
#define SRC_SENSORS_TURNERCYCLOPS_H_

// Debugging Statement
// #define MS_TURNERCYCLOPS_DEBUG

#ifdef MS_TURNERCYCLOPS_DEBUG
#define MS_DEBUGGING_STD "TurnerCyclops"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
/** @ingroup sensor_cyclops */
/**@{*/
/**
 * @brief Sensor::_numReturnedValues; the Cyclops can report 2 values.
 *
 * @note Although the Cyclops can come pre-configured for one of up to 13
 * standard parameters and additional custom parameters, each individual sensor
 * is only capable of measuring the single parameter it is configured for.  A
 * Cyclops that is configured to measure blue-green algae *cannot* be used to
 * measure turbidity.  The raw voltage value can be recorded for all variants of
 * the Cyclops.
 */
#define CYCLOPS_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; the raw voltage is reported, the other
/// parameter is calculated using the input calibration equation.
#define CYCLOPS_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_cyclops_timing
 * @name Sensor Timing
 * The sensor timing for an Cyclops-7F
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the ADS1115 warms up in 2ms.
#define CYCLOPS_WARM_UP_TIME_MS 2
/**
 * @brief Sensor::_stabilizationTime_ms; minimum stabilization time for the
 * Cyclops-7F is 1s (1000ms).
 *
 * @note The original Cyclops sensor (Cyclops-7) manufactured prior to 2017 had
 * a 5s warm-up time.  In the current model (Cyclops-7**F**) the warm-up time is
 * reduced to 1s.
 */
#define CYCLOPS_STABILIZATION_TIME_MS 1000
/// @brief Sensor::_measurementTime_ms; Cyclops takes 100ms to complete a
/// measurement - Maximum data rate = 10Hz (100ms/sample).
#define CYCLOPS_MEASUREMENT_TIME_MS 100
/**@}*/

/* clang-format off */
/**
 * @anchor sensor_cyclops_output
 * @name Calibrated Parameter Output
 * The primary output variable from an Cyclops-7F
 *
 * See the [optical specification guide](http://docs.turnerdesigns.com/t2/doc/spec-guides/998-2181.pdf)
 * for the minimum detection limit, accuracy, and range of the various Cyclops
 * parameter configurations.
 */
/**@{*/
/* clang-format on */
/// Variable number; the primary variable is stored in sensorValues[0].
#define CYCLOPS_VAR_NUM 0
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; 1.
#define CYCLOPS_RESOLUTION 1
#else
/// @brief Decimals places in string representation; 5.
#define CYCLOPS_RESOLUTION 5
#endif
/**@}*/

/**
 * @anchor sensor_cyclops_voltage
 * @name Voltage
 * The voltage variable from an Cyclops-7F
 * - Range is 0 to 3.6V when using an ADS1x15 powered at 3.3V
 *     - Full sensor range is 0-5V
 * - Accuracy:
 *     - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset errror)
 *       - @m_span{m-dim}(@ref #CYCLOPS_VOLTAGE_RESOLUTION = 4)@m_endspan
 *     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15%
 * (gain error), <3 LSB (offset errror)
 *       - @m_span{m-dim}(@ref #CYCLOPS_VOLTAGE_RESOLUTION = 1)@m_endspan
 *
 * {{ @ref TurnerCyclops_Voltage::TurnerCyclops_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[1].
#define CYCLOPS_VOLTAGE_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "voltage"
#define CYCLOPS_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt"
#define CYCLOPS_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "CyclopsVoltage"
#define CYCLOPS_VOLTAGE_DEFAULT_CODE "CyclopsVoltage"

#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1.
///  - Resolution:
///     - 16-bit ADC (ADS1115): 0.125 mV
#define CYCLOPS_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4.
///  - Resolution:
///     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 2 mV
#define CYCLOPS_VOLTAGE_RESOLUTION 4
#endif
/**@}*/

/// @brief The assumed address of the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Turner Cyclops-7F submersible fluorometer](@ref sensor_cyclops).
 *
 * @ingroup sensor_cyclops
 */
/* clang-format on */
class TurnerCyclops : public Sensor {
 public:
    // The constructor - need the power pin, the ADS1X15 data channel, and the
    // calibration info
    /* clang-format off */
    /**
     * @brief Construct a new Turner Cyclops object - need the power pin, the
     * ADS1X15 data channel, and the calibration info.
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core.  Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The pin on the mcu controlling power to the Cyclops-7F
     * Use -1 if it is continuously powered.
     * - The ADS1x15 requires an input voltage of 2.0-5.5V, but this library
     * assumes the ADS is powered with 3.3V.
     * - The Cyclops-7F itself requires a 3-15V power supply, which can be
     * turned off between measurements.
     * @param adsChannel The analog data channel _on the TI ADS1115_ that the
     * Cyclops is connected to (0-3).
     * @param conc_std The concentration of the standard used for a 1-point
     * sensor calibration.  The concentration units should be the same as the
     * final measuring units.
     * | ID  | Variable                     | Units                               |
     * | --- | ---------------------------- | ----------------------------------- |
     * | C   | TurnerCyclops_Chlorophyll    | micrograms per Liter (µg/L)         |
     * | R   | TurnerCyclops_Rhodamine      | parts per billion (ppb)             |
     * | F   | TurnerCyclops_Fluorescein    | parts per billion (ppb)             |
     * | P   | TurnerCyclops_Phycocyanin    | parts per billion (ppb)             |
     * | E   | TurnerCyclops_Phycoerythrin  | parts per billion (ppb)             |
     * | U   | TurnerCyclops_CDOM           | parts per billion (ppb)             |
     * | O   | TurnerCyclops_CrudeOil       | parts per billion (ppb)             |
     * | B   | TurnerCyclops_Brighteners    | parts per billion (ppb)             |
     * | T   | TurnerCyclops_Turbidity      | nephelometric turbidity units (NTU) |
     * | A   | TurnerCyclops_PTSA           | parts per billion (ppb)             |
     * | G   | TurnerCyclops_BTEX           | parts per million (ppm)             |
     * | L   | TurnerCyclops_Tryptophan     | parts per billion (ppb)             |
     * | D   | TurnerCyclops_RedChlorophyll | micrograms per Liter (µg/L)         |
     * @param volt_std The voltage (in volts) measured for the conc_std.  This
     * voltage should be the final voltage *after* accounting for any voltage
     * dividers or gain settings.
     * @param volt_blank The voltage (in volts) measured for a blank.  This
     * voltage should be the final voltage *after* accounting for any voltage
     * dividers or gain settings.
     * @param i2cAddress The I2C address of the ADS 1x15, default is 0x48 (ADDR
     * = GND)
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    /* clang-format on */
    TurnerCyclops(int8_t powerPin, uint8_t adsChannel, float conc_std,
                  float volt_std, float volt_blank,
                  uint8_t i2cAddress            = ADS1115_ADDRESS,
                  uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the Turner Cyclops object
     */
    ~TurnerCyclops();

    /**
     * @copydoc Sensor::getSensorLocation()
     */
    String getSensorLocation(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    uint8_t _adsChannel;
    float   _conc_std, _volt_std, _volt_blank;
    uint8_t _i2cAddress;
};


// Also returning raw voltage
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_cyclops_voltage) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * This could be helpful if the calibration equation was typed incorrectly or if
 * it is suspected to have changed over time.
 *
 * The raw voltage output can be measured by all variants of the Cyclops sensor.
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_Voltage object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsVoltage".
     */
    explicit TurnerCyclops_Voltage(
        TurnerCyclops* parentSense, const char* uuid = "",
        const char* varCode = CYCLOPS_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)CYCLOPS_VOLTAGE_VAR_NUM,
                   (uint8_t)CYCLOPS_VOLTAGE_RESOLUTION,
                   CYCLOPS_VOLTAGE_VAR_NAME, CYCLOPS_VOLTAGE_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Voltage object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Voltage()
        : Variable((const uint8_t)CYCLOPS_VOLTAGE_VAR_NUM,
                   (uint8_t)CYCLOPS_VOLTAGE_RESOLUTION,
                   CYCLOPS_VOLTAGE_VAR_NAME, CYCLOPS_VOLTAGE_UNIT_NAME,
                   CYCLOPS_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the TurnerCyclops_Voltage object - no action needed.
     */
    ~TurnerCyclops_Voltage() {}
};


/**
 * @brief The Variable sub-class used for the
 * [chlorophyll output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for the
 * blue excitiation of chlorophyll!**_  Chlorophyll (blue excitation) models
 * will be marked with a **“C”** at the top of the sensor housing near the cable
 * connections.
 *
 * Chlorophyll concentration is measured (and should be calibrated) in
 * micrograms per Liter (µ/L).
 *
 * - Miminum detection limit:  0.03 µg/L
 * - Linear range:  0-500 µg/L
 * - LED (CWL):  460 nm
 * - Excitiation wavelength:  465/170 nm
 * - Emission wavelength:  696/44 nm
 * - Power required (mW @12V):  240
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_Chlorophyll : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_Chlorophyll object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsChlorophyll".
     */
    explicit TurnerCyclops_Chlorophyll(
        TurnerCyclops* parentSense, const char* uuid = "",
        const char* varCode = "CyclopsChlorophyll")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "chlorophyllFluorescence",
                   "microgramPerLiter", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Chlorophyll object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Chlorophyll()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "chlorophyllFluorescence", "microgramPerLiter",
                   "CyclopsChlorophyll") {}
    ~TurnerCyclops_Chlorophyll() {}
};


/**
 * @brief The Variable sub-class used for the
 * [Rhodamine output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for
 * Rhodamine WT!**_  Rhodamine WT models will be marked with a **“R”** at the
 * top of the sensor housing near the cable connections.
 *
 * Rhodamine WT concentration is measured (and should be calibrated) in parts
 * per billion (ppb).
 *
 * - Miminum detection limit:  0.01 ppb
 * - Linear range:  0-1000 ppb
 * - LED (CWL):  530 nm
 * - Excitiation wavelength:  535/60 nm
 * - Emission wavelength:  590-715 nm
 * - Power required (mW @12V):  175
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_Rhodamine : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_Rhodamine object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsRhodamine".
     */
    explicit TurnerCyclops_Rhodamine(TurnerCyclops* parentSense,
                                     const char*    uuid = "",
                                     const char* varCode = "CyclopsRhodamine")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "RhodamineFluorescence",
                   "partPerBillion", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Rhodamine object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Rhodamine()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "RhodamineFluorescence", "partPerBillion",
                   "CyclopsRhodamine") {}
    ~TurnerCyclops_Rhodamine() {}
};


/**
 * @brief The Variable sub-class used for the
 * [fluorescein output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for
 * fluorescein dye!**_  Fluorescein models will be marked with an **“F”** at the
 * top of the sensor housing near the cable connections.
 *
 * Fluorescein concentration is measured (and should be calibrated) in parts per
 * billion (ppb).
 *
 * - Miminum detection limit:  0.01 ppb
 * - Linear range:  0-500 ppb
 * - LED (CWL): 460 nm
 * - Excitiation wavelength:  400/150 nm
 * - Emission wavelength:  545/28 nm
 * - Power required (mW @12V):  145
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_Fluorescein : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_Fluorescein object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsFluorescein".
     */
    explicit TurnerCyclops_Fluorescein(
        TurnerCyclops* parentSense, const char* uuid = "",
        const char* varCode = "CyclopsFluorescein")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "fluorescein", "partPerBillion",
                   varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Fluorescein object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Fluorescein()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "fluorescein", "partPerBillion", "CyclopsFluorescein") {}
    ~TurnerCyclops_Fluorescein() {}
};


/**
 * @brief The Variable sub-class used for the
 * [phycocyanin output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for
 * phycocyanin (freshwater cyanobacteria)!**_  Phycocyanin models will be marked
 * with a **“P”** at the top of the sensor housing near the cable connections.
 *
 * Phycocyanin concentration is measured (and should be calibrated) in parts per
 * billion (ppb).
 *
 * - Miminum detection limit:  2 ppb (Phycocyanin pigment from Prozyme diluted
 * in Deionized water)
 * - Linear range:  0-4,500 ppb
 * - LED (CWL):  590 nm
 * - Excitiation wavelength:  590/30 nm
 * - Emission wavelength:  >= 645 nm
 * - Power required (mW @12V):  160
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_Phycocyanin : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_Phycocyanin object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsPhycocyanin".
     */
    explicit TurnerCyclops_Phycocyanin(
        TurnerCyclops* parentSense, const char* uuid = "",
        const char* varCode = "CyclopsPhycocyanin")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION,
                   "blue_GreenAlgae_Cyanobacteria_Phycocyanin",
                   "partPerBillion", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Phycocyanin object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Phycocyanin()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "blue_GreenAlgae_Cyanobacteria_Phycocyanin",
                   "partPerBillion", "CyclopsPhycocyanin") {}
    ~TurnerCyclops_Phycocyanin() {}
};


/**
 * @brief The Variable sub-class used for the
 * [phycoerythrin output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for
 * phycoerythrin (marine cyanobacteria)!**_  Phycoerythrin models will be marked
 * with an **“E”** at the top of the sensor housing near the cable connections.
 *
 * Phycoerythrin concentration is measured (and should be calibrated) in parts
 * per billion (ppb).
 *
 * - Miminum detection limit:  0.1 ppb (Phycoerythrin pigment from Prozyme
 * diluted in Deionized water)
 * - Linear range:  0-750 ppb
 * - LED (CWL):  525 nm
 * - Excitiation wavelength:  515-547 nm
 * - Emission wavelength:  >=590 nm
 * - Power required (mW @12V):  270
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_Phycoerythrin : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_Phycoerythrin object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsPhycoerythrin".
     */
    explicit TurnerCyclops_Phycoerythrin(
        TurnerCyclops* parentSense, const char* uuid = "",
        const char* varCode = "CyclopsPhycoerythrin")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "phycoerythrin",
                   "partPerBillion", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Phycoerythrin object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Phycoerythrin()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "phycoerythrin", "partPerBillion", "CyclopsPhycoerythrin") {}
    ~TurnerCyclops_Phycoerythrin() {}
};


/**
 * @brief The Variable sub-class used for the
 * [CDOM output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for CDOM /
 * fDOM!**_  CDOM models will be marked with a **“U”** at the top of the sensor
 * housing near the cable connections.
 *
 * CDOM/fDOM concentration is measured (and should be calibrated) in parts per
 * billion (ppb).
 *
 * - Miminum detection limit:
 *   - 0.1 ppb Quinine Sulfate
 *   - 0.5 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid Tetrasodium Salt)
 * - Linear range:
 *   - 0-1,500 ppb Quinine Sulfate
 *   - 0-3,000 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid Tetrasodium Salt)
 * - LED (CWL):  365nm
 * - Excitiation wavelength:  325/120 nm
 * - Emission wavelength:  470/60 nm
 * - Power required (mW @12V):  240
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_CDOM : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_CDOM object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsCDOM".
     */
    explicit TurnerCyclops_CDOM(TurnerCyclops* parentSense,
                                const char*    uuid    = "",
                                const char*    varCode = "CyclopsCDOM")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION,
                   "fluorescenceDissolvedOrganicMatter", "partPerBillion",
                   varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_CDOM object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_CDOM()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "fluorescenceDissolvedOrganicMatter", "partPerBillion",
                   "CyclopsCDOM") {}
    ~TurnerCyclops_CDOM() {}
};


/**
 * @brief The Variable sub-class used for the
 * [crude oil output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for crude
 * oil!**_  Crude oil / petrolium models will be marked with an **“O”** at the
 * top of the sensor housing near the cable connections.
 *
 * Crude oil / petrolium concentration is measured (and should be calibrated) in
 * parts per billion (ppb).
 *
 * - Miminum detection limit:  0.2 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid
 * Tetrasodium Salt)
 * - Linear range:  0-1,500 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid
 * Tetrasodium Salt)
 * - LED (CWL):  365 nm
 * - Excitiation wavelength:  325/120 nm
 * - Emission wavelength:  410-600 nm
 * - Power required (mW @12V):  250
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_CrudeOil : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_CrudeOil object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsCrudeOil".
     */
    explicit TurnerCyclops_CrudeOil(TurnerCyclops* parentSense,
                                    const char*    uuid    = "",
                                    const char*    varCode = "CyclopsCrudeOil")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "petroleumHydrocarbonTotal",
                   "partPerBillion", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_CrudeOil object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_CrudeOil()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "petroleumHydrocarbonTotal", "partPerBillion",
                   "CyclopsCrudeOil") {}
    ~TurnerCyclops_CrudeOil() {}
};


/**
 * @brief The Variable sub-class used for the
 * [opticalBrighteners output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for
 * optical brighteners for wastewater monitoring!**_  Optical brighteners models
 * will be marked with a **“B”** at the top of the sensor housing near the cable
 * connections.
 *
 * Optical brightener concentration is measured (and should be calibrated) in
 * parts per billion (ppb).
 *
 * - Miminum detection limit:  0.6 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid
 * Tetrasodium Salt)
 * - Linear range:  0-2,500 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid
 * Tetrasodium Salt)
 * - LED (CWL):  365 nm
 * - Excitiation wavelength:  325/120 nm
 * - Emission wavelength:  445/15 nm
 * - Power required (mW @12V):  200
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_Brighteners : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_Brighteners object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsOpticalBrighteners".
     */
    explicit TurnerCyclops_Brighteners(
        TurnerCyclops* parentSense, const char* uuid = "",
        const char* varCode = "CyclopsOpticalBrighteners")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "opticalBrighteners",
                   "partPerBillion", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Brighteners object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Brighteners()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "opticalBrighteners", "partPerBillion",
                   "CyclopsOpticalBrighteners") {}
    ~TurnerCyclops_Brighteners() {}
};


/**
 * @brief The Variable sub-class used for the
 * [turbidity output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for
 * turbidity!**_  Turbidity models will be marked with a **“T”** at the top of
 * the sensor housing near the cable connections.
 *
 * Turbidity is measured (and should be calibrated) in nephelometric turbidity
 * units (NTU).
 *
 * - Miminum detection limit:  0.05 NTU
 * - Linear range:  0-1,500 NTU
 * - LED (CWL):  850 nm
 * - Excitiation wavelength:  850 nm
 * - Emission wavelength:  850 nm
 * - Power required (mW @12V):  120
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_Turbidity object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsTurbidity".
     */
    explicit TurnerCyclops_Turbidity(TurnerCyclops* parentSense,
                                     const char*    uuid = "",
                                     const char* varCode = "CyclopsTurbidity")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "Turbidity",
                   "nephelometricTurbidityUnit", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Turbidity object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Turbidity()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "Turbidity", "nephelometricTurbidityUnit",
                   "CyclopsTurbidity") {}
    ~TurnerCyclops_Turbidity() {}
};


/**
 * @brief The Variable sub-class used for the
 * [PTSA output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for PTSA
 * (1,3,6,8-Pyrenetetrasulfonic Acid Tetrasodium Salt)!**_  PTSA models will be
 * marked with an **“A”** at the top of the sensor housing near the cable
 * connections.
 *
 * PTSA concentration is measured (and should be calibrated) in parts per
 * billion (ppb).
 *
 * - Miminum detection limit:  0.1 ppb
 * - Linear range:  0-650 ppb
 * - LED (CWL):  365
 * - Excitiation wavelength:  325/120 nm
 * - Emission wavelength:  405/10 nm
 * - Power required (mW @12V):  320
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_PTSA : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_PTSA object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsPTSA".
     */
    explicit TurnerCyclops_PTSA(TurnerCyclops* parentSense,
                                const char*    uuid    = "",
                                const char*    varCode = "CyclopsPTSA")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "ptsa", "partPerBillion",
                   varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_PTSA object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_PTSA()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "ptsa", "partPerBillion", "CyclopsPTSA") {}
    ~TurnerCyclops_PTSA() {}
};


/**
 * @brief The Variable sub-class used for the
 * [refined fuels (BTEX) output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for fine
 * oil / refined fuels / BTEX!**_  Fine oil models will be marked with a **“G”**
 * at the top of the sensor housing near the cable connections.  Fine oil is
 * measured as BTEX (benzene, toluene, ethylbenzene, xylenes).
 *
 * BTEX concentration is measured (and should be calibrated) in parts per
 * million (ppm).
 *
 * - Miminum detection limit:  0.4 ppm
 * - Linear range:  0-20 ppm
 * - LED (CWL):  255 nm
 * - Excitiation wavelength:  <=290 nm
 * - Emission wavelength:  350/50 nm
 * - Power required (mW @12V):  530
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_BTEX : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_BTEX object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsBTEX".
     */
    explicit TurnerCyclops_BTEX(TurnerCyclops* parentSense,
                                const char*    uuid    = "",
                                const char*    varCode = "CyclopsBTEX")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "btex", "partPerMillion",
                   varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_BTEX object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_BTEX()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "btex", "partPerMillion", "CyclopsBTEX") {}
    ~TurnerCyclops_BTEX() {}
};


/**
 * @brief The Variable sub-class used for the
 * [tryptophan output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for
 * tryptophan!**_  Tryptophan models will be marked with an **“L”** at the top
 * of the sensor housing near the cable connections.
 *
 * Tryptophan concentration is measured (and should be calibrated) in parts per
 * billion (ppb).
 *
 * - Miminum detection limit:  3 ppb
 * - Linear range:  0-5,000 ppb
 * - LED (CWL):  275 nm
 * - Excitiation wavelength:  -
 * - Emission wavelength:  350/55
 * - Power required (mW @12V):  540
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_Tryptophan : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_Tryptophan object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsTryptophan".
     */
    explicit TurnerCyclops_Tryptophan(TurnerCyclops* parentSense,
                                      const char*    uuid = "",
                                      const char* varCode = "CyclopsTryptophan")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "tryptophan", "partPerBillion",
                   varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Tryptophan object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Tryptophan()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "tryptophan", "partPerBillion", "CyclopsTryptophan") {}
    ~TurnerCyclops_Tryptophan() {}
};


/**
 * @brief The Variable sub-class used for the
 * [chlorophyll output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for the
 * red excitiation of chlorophyll!**_  Chlorophyll (red excitation) models will
 * be marked with a **“D”** at the top of the sensor housing near the cable
 * connections.
 *
 * Chlorophyll concentration is measured (and should be calibrated) in
 * micrograms per Liter (µ/L).
 *
 * - Miminum detection limit:  0.3 µg/L
 * - Linear range:  0-500 µg/L
 * - LED (CWL):  635 nm
 * - Excitiation wavelength:  <=635 nm
 * - Emission wavelength:  >=695 nm
 * - Power required (mW @12V):  240
 *
 * @ingroup sensor_cyclops
 */
class TurnerCyclops_RedChlorophyll : public Variable {
 public:
    /**
     * @brief Construct a new TurnerCyclops_RedChlorophyll object.
     *
     * @param parentSense The parent TurnerCyclops providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "CyclopsChlorophyll".
     */
    explicit TurnerCyclops_RedChlorophyll(
        TurnerCyclops* parentSense, const char* uuid = "",
        const char* varCode = "CyclopsRedChlorophyll")
        : Variable(parentSense, (const uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "chlorophyllFluorescence",
                   "microgramPerLiter", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_RedChlorophyll object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_RedChlorophyll()
        : Variable((const uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "chlorophyllFluorescence", "microgramPerLiter",
                   "CyclopsRedChlorophyll") {}
    ~TurnerCyclops_RedChlorophyll() {}
};
/**@}*/
#endif  // SRC_SENSORS_TURNERCYCLOPS_H_
