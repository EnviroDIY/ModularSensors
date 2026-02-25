/**
 * @file TurnerCyclops.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the TurnerCyclops sensor subclass and the variable subclasses
 * TurnerCyclops_Turbidity and TurnerCyclops_Voltage.
 *
 * These are used for the Turner Scientific Cyclops-7F.
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
 * only 3.3V, the gain must be reduced or a voltage divider used to
 * ensure the output signal does not exceed 3.6V.  This library does *not*
 * support variable gain or any type of auto-gaining for the Cyclops
 * sensors.
 *
 * @note The Cyclops should be calibrated in as close to field conditions as
 * possible.  All gain settings and voltage dividers should be in place for
 * the calibration.
 *
 * The units to use for the calibration point depend on the parameter being measured,
 * as listed in the table below.
 *
 * | ID  | Variable                     | Units                               |
 * | --- | ---------------------------- | ----------------------------------- |
 * | U   | TurnerCyclops_CDOM           | parts per billion (ppb)             |
 * | C   | TurnerCyclops_Chlorophyll    | micrograms per Liter (µg/L)         |
 * | D   | TurnerCyclops_RedChlorophyll | micrograms per Liter (µg/L)         |
 * | F   | TurnerCyclops_Fluorescein    | parts per billion (ppb)             |
 * | O   | TurnerCyclops_CrudeOil       | parts per billion (ppb)             |
 * | G   | TurnerCyclops_BTEX           | parts per million (ppm)             |
 * | B   | TurnerCyclops_Brighteners    | parts per billion (ppb)             |
 * | P   | TurnerCyclops_Phycocyanin    | parts per billion (ppb)             |
 * | E   | TurnerCyclops_Phycoerythrin  | parts per billion (ppb)             |
 * | A   | TurnerCyclops_PTSA           | parts per billion (ppb)             |
 * | R   | TurnerCyclops_Rhodamine      | parts per billion (ppb)             |
 * | L   | TurnerCyclops_Tryptophan     | parts per billion (ppb)             |
 * | T   | TurnerCyclops_Turbidity      | nephelometric turbidity units (NTU) |
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
 * - `-D CYCLOPS_CALIBRATION_EPSILON=x.xf`
 *     - Sets the tolerance for validating the calibration values
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

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_TURNERCYCLOPS_DEBUG
#define MS_DEBUGGING_STD "TurnerCyclops"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include "VariableBase.h"
#include "SensorBase.h"
#include "AnalogVoltageBase.h"

// Sensor Specific Defines
/** @ingroup sensor_cyclops */
/**@{*/

/**
 * @anchor sensor_cyclops_config
 * @name Configuration Parameters
 * Configuration parameters for the Turner Cyclops-7F sensor
 */
/**@{*/
#if !defined(CYCLOPS_CALIBRATION_EPSILON) || defined(DOXYGEN)
/**
 * @brief Minimum voltage difference threshold for calibration validation
 *
 * This epsilon value is used to validate that the calibration standard voltage
 * and blank voltage are sufficiently different to provide a meaningful
 * calibration. If the absolute difference between these voltages is less than
 * this threshold, the calibration is considered invalid.
 *
 * @note This should be tuned to match the expected precision of the sensor
 * and ADC system. A value of 1e-4f (0.0001V or 0.1mV) is appropriate for
 * most high-precision ADC configurations.
 */
#define CYCLOPS_CALIBRATION_EPSILON 1e-4f
#endif  // !defined(CYCLOPS_CALIBRATION_EPSILON) || defined(DOXYGEN)
/**@}*/

/**
 * @anchor sensor_cyclops_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by Cyclops
 */
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
/**@}*/

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
 *     - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset error)
 *       - @m_span{m-dim}(@ref #CYCLOPS_VOLTAGE_RESOLUTION = 4)@m_endspan
 *     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15%
 * (gain error), <3 LSB (offset error)
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
    /**
     * @brief Construct a new Turner Cyclops object - need the power pin, the
     * analog data channel, and the calibration info.
     *
     * By default, this constructor will internally create a default
     * AnalogVoltageBase implementation for voltage readings, but a pointer to
     * a custom AnalogVoltageBase object can be passed in if desired.
     *
     * @param powerPin The pin on the mcu controlling power to the Cyclops-7F
     * Use -1 if it is continuously powered.
     * - The Cyclops-7F itself requires a 3-15V power supply, which can be
     * turned off between measurements.
     * @param analogChannel The analog data channel or processor pin for voltage
     * measurements.  The significance of the channel number depends on the
     * specific AnalogVoltageBase implementation used for voltage readings. For
     * example, with the TI ADS1x15, this would be the ADC channel (0-3) that
     * the sensor is connected to.  Negative or invalid channel numbers are not
     * clamped and will cause the reading to fail and emit a warning.
     * @param conc_std The concentration of the standard used for a 1-point
     * sensor calibration.  The concentration units should be the same as the
     * final measuring units.
     * @param volt_std The voltage (in volts) measured for the conc_std.  This
     * voltage should be the final voltage *after* accounting for any voltage
     * dividers or gain settings.
     * @param volt_blank The voltage (in volts) measured for a blank.  This
     * voltage should be the final voltage *after* accounting for any voltage
     * dividers or gain settings.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param analogVoltageReader Pointer to an AnalogVoltageBase object for
     * voltage measurements.  Pass nullptr (the default) to have the constructor
     * internally create and own a TIADS1x15Base instance.  If a non-null
     * pointer is supplied, the caller retains ownership and must ensure its
     * lifetime exceeds that of this object.
     */
    TurnerCyclops(int8_t powerPin, int8_t analogChannel, float conc_std,
                  float volt_std, float volt_blank,
                  uint8_t            measurementsToAverage = 1,
                  AnalogVoltageBase* analogVoltageReader   = nullptr);
    /**
     * @brief Destroy the Turner Cyclops object
     */
    ~TurnerCyclops();

    // Delete copy constructor and copy assignment operator to prevent shallow
    // copies
    TurnerCyclops(const TurnerCyclops&)            = delete;
    TurnerCyclops& operator=(const TurnerCyclops&) = delete;

    // Delete move constructor and move assignment operator
    TurnerCyclops(TurnerCyclops&&)            = delete;
    TurnerCyclops& operator=(TurnerCyclops&&) = delete;

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    /**
     * @brief The concentration of the standard used for a 1-point sensor
     * calibration.  The concentration units should be the same as the final
     * measuring units.
     */
    float _conc_std;
    /**
     * @brief The voltage (in volts) measured for the conc_std.  This voltage
     * should be the final voltage *after* accounting for any voltage dividers
     * or gain settings.
     */
    float _volt_std;
    /**
     * @brief The voltage (in volts) measured for a blank.  This voltage should
     * be the final voltage *after* accounting for any voltage dividers or gain
     * settings.
     */
    float _volt_blank;
    /// @brief Pointer to analog voltage reader
    AnalogVoltageBase* _analogVoltageReader = nullptr;
    /// @brief Flag to track if this object owns the analog voltage reader and
    /// should delete it in the destructor
    bool _ownsAnalogVoltageReader = false;
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VOLTAGE_VAR_NUM,
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
        : Variable((uint8_t)CYCLOPS_VOLTAGE_VAR_NUM,
                   (uint8_t)CYCLOPS_VOLTAGE_RESOLUTION,
                   CYCLOPS_VOLTAGE_VAR_NAME, CYCLOPS_VOLTAGE_UNIT_NAME,
                   CYCLOPS_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the Turner Cyclops Voltage object - no action needed.
     */
    ~TurnerCyclops_Voltage() {}
};


/**
 * @brief The Variable sub-class used for the
 * [chlorophyll output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for the
 * blue excitation of chlorophyll!**_  Chlorophyll (blue excitation) models
 * will be marked with a **“C”** at the top of the sensor housing near the cable
 * connections.
 *
 * Chlorophyll concentration is measured (and should be calibrated) in
 * micrograms per Liter (µ/L).
 *
 * - Minimum detection limit:  0.03 µg/L
 * - Linear range:  0-500 µg/L
 * - LED (CWL):  460 nm
 * - Excitation wavelength:  465/170 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "chlorophyllFluorescence",
                   "microgramPerLiter", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Chlorophyll object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Chlorophyll()
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "chlorophyllFluorescence", "microgramPerLiter",
                   "CyclopsChlorophyll") {}
    /**
     * @brief Destroy the Turner Cyclops Chlorophyll variable object - no action
     * needed.
     */
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
 * - Minimum detection limit:  0.01 ppb
 * - Linear range:  0-1000 ppb
 * - LED (CWL):  530 nm
 * - Excitation wavelength:  535/60 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "RhodamineFluorescence",
                   "partPerBillion", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Rhodamine object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Rhodamine()
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "RhodamineFluorescence", "partPerBillion",
                   "CyclopsRhodamine") {}
    /**
     * @brief Destroy the Turner Cyclops Rhodamine variable object - no action
     * needed.
     */
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
 * - Minimum detection limit:  0.01 ppb
 * - Linear range:  0-500 ppb
 * - LED (CWL): 460 nm
 * - Excitation wavelength:  400/150 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "fluorescein", "partPerBillion",
                   varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Fluorescein object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Fluorescein()
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "fluorescein", "partPerBillion", "CyclopsFluorescein") {}
    /**
     * @brief Destroy the Turner Cyclops Fluorescein variable object - no action
     * needed.
     */
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
 * - Minimum detection limit:  2 ppb (Phycocyanin pigment from Prozyme diluted
 * in Deionized water)
 * - Linear range:  0-4,500 ppb
 * - LED (CWL):  590 nm
 * - Excitation wavelength:  590/30 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
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
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "blue_GreenAlgae_Cyanobacteria_Phycocyanin",
                   "partPerBillion", "CyclopsPhycocyanin") {}
    /**
     * @brief Destroy the Turner Cyclops Phycocyanin variable object - no action
     * needed.
     */
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
 * - Minimum detection limit:  0.1 ppb (Phycoerythrin pigment from Prozyme
 * diluted in Deionized water)
 * - Linear range:  0-750 ppb
 * - LED (CWL):  525 nm
 * - Excitation wavelength:  515-547 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "phycoerythrin",
                   "partPerBillion", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Phycoerythrin object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Phycoerythrin()
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "phycoerythrin", "partPerBillion", "CyclopsPhycoerythrin") {}
    /**
     * @brief Destroy the Turner Cyclops Phycoerythrin variable object - no
     * action needed.
     */
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
 * - Minimum detection limit:
 *   - 0.1 ppb Quinine Sulfate
 *   - 0.5 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid Tetrasodium Salt)
 * - Linear range:
 *   - 0-1,500 ppb Quinine Sulfate
 *   - 0-3,000 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid Tetrasodium Salt)
 * - LED (CWL):  365nm
 * - Excitation wavelength:  325/120 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
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
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "fluorescenceDissolvedOrganicMatter", "partPerBillion",
                   "CyclopsCDOM") {}
    /**
     * @brief Destroy the Turner Cyclops CDOM variable object - no action
     * needed.
     */
    ~TurnerCyclops_CDOM() {}
};


/**
 * @brief The Variable sub-class used for the
 * [crude oil output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for crude
 * oil!**_  Crude oil / petroleum models will be marked with an **“O”** at the
 * top of the sensor housing near the cable connections.
 *
 * Crude oil / petroleum concentration is measured (and should be calibrated) in
 * parts per billion (ppb).
 *
 * - Minimum detection limit:  0.2 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid
 * Tetrasodium Salt)
 * - Linear range:  0-1,500 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid
 * Tetrasodium Salt)
 * - LED (CWL):  365 nm
 * - Excitation wavelength:  325/120 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "petroleumHydrocarbonTotal",
                   "partPerBillion", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_CrudeOil object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_CrudeOil()
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "petroleumHydrocarbonTotal", "partPerBillion",
                   "CyclopsCrudeOil") {}
    /**
     * @brief Destroy the Turner Cyclops CrudeOil variable object - no action
     * needed.
     */
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
 * - Minimum detection limit:  0.6 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid
 * Tetrasodium Salt)
 * - Linear range:  0-2,500 ppb PTSA (1,3,6,8-Pyrenetetrasulfonic Acid
 * Tetrasodium Salt)
 * - LED (CWL):  365 nm
 * - Excitation wavelength:  325/120 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "opticalBrighteners",
                   "partPerBillion", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Brighteners object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Brighteners()
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "opticalBrighteners", "partPerBillion",
                   "CyclopsOpticalBrighteners") {}
    /**
     * @brief Destroy the Turner Cyclops Brighteners object - no action needed.
     */
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
 * - Minimum detection limit:  0.05 NTU
 * - Linear range:  0-1,500 NTU
 * - LED (CWL):  850 nm
 * - Excitation wavelength:  850 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "Turbidity",
                   "nephelometricTurbidityUnit", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Turbidity object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Turbidity()
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "Turbidity", "nephelometricTurbidityUnit",
                   "CyclopsTurbidity") {}
    /**
     * @brief Destroy the Turner Cyclops Turbidity variable object - no action
     * needed.
     */
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
 * - Minimum detection limit:  0.1 ppb
 * - Linear range:  0-650 ppb
 * - LED (CWL):  365
 * - Excitation wavelength:  325/120 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "ptsa", "partPerBillion",
                   varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_PTSA object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_PTSA()
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "ptsa", "partPerBillion", "CyclopsPTSA") {}
    /**
     * @brief Destroy the Turner Cyclops PTSA variable object - no action
     * needed.
     */
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
 * - Minimum detection limit:  0.4 ppm
 * - Linear range:  0-20 ppm
 * - LED (CWL):  255 nm
 * - Excitation wavelength:  <=290 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "btex", "partPerMillion",
                   varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_BTEX object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_BTEX()
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "btex", "partPerMillion", "CyclopsBTEX") {}
    /**
     * @brief Destroy the Turner Cyclops BTEX variable object - no action
     * needed.
     */
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
 * - Minimum detection limit:  3 ppb
 * - Linear range:  0-5,000 ppb
 * - LED (CWL):  275 nm
 * - Excitation wavelength:  -
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "tryptophan", "partPerBillion",
                   varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_Tryptophan object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_Tryptophan()
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "tryptophan", "partPerBillion", "CyclopsTryptophan") {}
    /**
     * @brief Destroy the Turner Cyclops Tryptophan variable object - no action
     * needed.
     */
    ~TurnerCyclops_Tryptophan() {}
};


/**
 * @brief The Variable sub-class used for the
 * [chlorophyll output](@ref sensor_cyclops_output) from a
 * [Turner Cyclops-7F](@ref sensor_cyclops).
 *
 * @note _**This can only be used for a Cyclops-7F that is configured for the
 * red excitation of chlorophyll!**_  Chlorophyll (red excitation) models will
 * be marked with a **“D”** at the top of the sensor housing near the cable
 * connections.
 *
 * Chlorophyll concentration is measured (and should be calibrated) in
 * micrograms per Liter (µ/L).
 *
 * - Minimum detection limit:  0.3 µg/L
 * - Linear range:  0-500 µg/L
 * - LED (CWL):  635 nm
 * - Excitation wavelength:  <=635 nm
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
        : Variable(parentSense, (uint8_t)CYCLOPS_VAR_NUM,
                   (uint8_t)CYCLOPS_RESOLUTION, "chlorophyllFluorescence",
                   "microgramPerLiter", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerCyclops_RedChlorophyll object.
     *
     * @note This must be tied with a parent TurnerCyclops before it can be
     * used.
     */
    TurnerCyclops_RedChlorophyll()
        : Variable((uint8_t)CYCLOPS_VAR_NUM, (uint8_t)CYCLOPS_RESOLUTION,
                   "chlorophyllFluorescence", "microgramPerLiter",
                   "CyclopsRedChlorophyll") {}
    /**
     * @brief Destroy the Turner Cyclops Red Chlorophyll variable object - no
     * action needed.
     */
    ~TurnerCyclops_RedChlorophyll() {}
};
/**@}*/
#endif  // SRC_SENSORS_TURNERCYCLOPS_H_

// cSpell:ignore fluorophores BTEX PTSA Pyrenetetrasulfonic Tetrasodium
// cSpell:ignore Ethylbenzene Prozyme sensor_cyclops_calib
