/**
 * @file ApogeeSQ212.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.h by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ApogeeSQ212 sensor subclass and the variable subclasses
 * ApogeeSQ212_PAR and ApogeeSQ212_Voltage.
 *
 * These are used for the Apogee SQ-212 quantum light sensor.
 */
/* clang-format off */
/**
 * @defgroup sensor_sq212 Apogee SQ-212
 * Classes for the Apogee SQ-212 quantum light sensor.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_sq212_intro Introduction
 * The [Apogee SQ-212 quantum light
 * sensor](https://www.apogeeinstruments.com/sq-212-amplified-0-2-5-volt-sun-calibration-quantum-sensor/)
 * measures [photosynthetically active radiation
 * (PAR)](https://en.wikipedia.org/wiki/Photosynthetically_active_radiation) -
 * typically defined as total radiation across a range of 400 to 700 nm.  PAR is
 * often expressed as photosynthetic photon flux density (PPFD): photon flux in
 * units of micromoles per square meter per second (μmol m-2 s-1, equal to
 * microEinsteins per square meter per second) summed from 400 to 700 nm.  The
 * raw output from the sensor is a simple analog signal which must be converted
 * to a digital signal and then multiplied by a calibration factor to get the
 * final PAR value.  The PAR sensor requires a 5-24 V DC power source with a
 * nominal current draw of 300 μA.  The power supply to the sensor can be
 * stopped between measurements.
 *
 * To convert the sensor's analog signal to a high resolution digital signal,
 * the sensor must be attached to an analog-to-digital converter.  See the
 * [ADS1115](@ref analog_group) for details on the conversion.
 *
 * The calibration factor this library uses to convert from raw voltage to PAR
 * is that specified by Apogee for the SQ-212: 1 µmol mˉ² sˉ¹ per mV (reciprocal
 * of sensitivity).  If needed, this calibration factor can be modified by
 * compiling with the build flag ```-D SQ212_CALIBRATION_FACTOR=x``` where x is
 * the calibration factor.  This allows you to adjust the calibration or change
 * to another Apogee sensor (e.g. SQ-215 or SQ225) as needed.
 *
 * @section sensor_sq212_datasheet Sensor Datasheet
 * [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Apogee
 * SQ-212-215 Manual.pdf)
 *
 * @section sensor_sq212_flags Build flags
 * - ```-D SQ212_CALIBRATION_FACTOR=x```
 *      - Changes the calibration factor from 1 to x
 *
 * @section sensor_sq212_ctor Sensor Constructor
 * {{ @ref ApogeeSQ212::ApogeeSQ212 }}
 *
 * ___
 * @section sensor_sq212_examples Example Code
 * The SQ-212 is used in the @menulink{apogee_sq212} example.
 *
 * @menusnip{apogee_sq212}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_APOGEESQ212_H_
#define SRC_SENSORS_APOGEESQ212_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_APOGEESQ212_DEBUG
#define MS_DEBUGGING_STD "ApogeeSQ212"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include "VariableBase.h"
#include "SensorBase.h"

// Forward declaration
class AnalogVoltageBase;

/** @ingroup sensor_sq212 */
/**@{*/

/**
 * @anchor sensor_sq212_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by the Apogee SQ-212
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the SQ212 can report 2 values, raw
/// voltage and calculated PAR.
#define SQ212_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; PAR is calculated from the raw voltage.
#define SQ212_INC_CALC_VARIABLES 1
/**@}*/

/**
 * @anchor sensor_sq212_config
 * @name Configuration Defines
 * Defines to set the calibration of the SQ-212 and the address of the ADD.
 */
/**@{*/
#if !defined(SQ212_CALIBRATION_FACTOR) || defined(DOXYGEN)
/**
 * @brief The calibration factor between output in volts and PAR
 * (microeinsteinPerSquareMeterPerSecond) 1 µmol mˉ² sˉ¹ per mV (reciprocal of
 * sensitivity)
 */
#define SQ212_CALIBRATION_FACTOR 1
#endif
/**@}*/

/**
 * @anchor sensor_sq212_timing
 * @name Sensor Timing
 * The sensor timing for an Apogee SQ-212
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; the warm up time is unknown; using the 2ms for
 * the TI ADS1x15 to warm up
 *
 * @todo Measure warm-up time of the SQ-212
 */
#define SQ212_WARM_UP_TIME_MS 2
/**
 * @brief Sensor::_stabilizationTime_ms; the ADS1115 is stable after 2ms.
 *
 * The stabilization time of the SQ-212 itself is not known!
 *
 * @todo Measure stabilization time of the SQ-212
 */
#define SQ212_STABILIZATION_TIME_MS 2
/// @brief Sensor::_measurementTime_ms; ADS1115 takes almost 2ms to complete a
/// measurement (860/sec).
#define SQ212_MEASUREMENT_TIME_MS 2
/**@}*/

/**
 * @anchor sensor_sq212_par
 * @name PAR
 * The PAR variable from an Apogee SQ-212
 * - Range is 0 to 2500 µmol m-2 s-1
 * - Accuracy is ± 0.5%
 * - Resolution:
 *   - 16-bit ADC (ADS1115): 0.3125 µmol m-2 s-1 (ADS1115)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 5 µmol m-2
 * s-1 (ADS1015)
 * - Reported as microeinsteins per square meter per second (µE m-2 s-1 or µmol
 * m-2 s-1)
 *
 * {{ @ref ApogeeSQ212_PAR }}
 */
/**@{*/
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; PAR should have 0 when
/// using an ADS1015.
#define SQ212_PAR_RESOLUTION 0
#else
/// @brief Decimals places in string representation; PAR should have 4 when
/// using an ADS1115.
#define SQ212_PAR_RESOLUTION 4
#endif
/// Variable number; PAR is stored in sensorValues[0].
#define SQ212_PAR_VAR_NUM 0
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/);
/// "radiationIncomingPAR"
#define SQ212_PAR_VAR_NAME "radiationIncomingPAR"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "microeinsteinPerSquareMeterPerSecond" (µE m-2 s-1 or µmol * m-2 s-1)
#define SQ212_PAR_UNIT_NAME "microeinsteinPerSquareMeterPerSecond"
/// @brief Default variable short code; "photosyntheticallyActiveRadiation"
#define SQ212_PAR_DEFAULT_CODE "photosyntheticallyActiveRadiation"
/**@}*/

/**
 * @anchor sensor_sq212_voltage
 * @name Voltage
 * The voltage variable from an Apogee SQ-212
 * - Range is 0 to 3.6V [when ADC is powered at 3.3V]
 * - Accuracy is ± 0.5%
 *   - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset error)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15%
 * (gain error), <3 LSB (offset error)
 * - Resolution [assuming the ADC is powered at 3.3V with inbuilt gain set to 1
 * (0-4.096V)]:
 *   - 16-bit ADC (ADS1115): 0.125 mV (ADS1115)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 2 mV
 * (ADS1015)
 *
 * {{ @ref ApogeeSQ212_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[1].
#define SQ212_VOLTAGE_VAR_NUM 1
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/); "voltage"
#define SQ212_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt" (V)
#define SQ212_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "SQ212Voltage"
#define SQ212_VOLTAGE_DEFAULT_CODE "SQ212Voltage"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1 when
/// used with an ADS1015.
#define SQ212_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4 when
/// used with an ADS1115.
#define SQ212_VOLTAGE_RESOLUTION 4
#endif
/**@}*/

/**
 * @brief The Sensor sub-class for the [Apogee SQ-212](@ref sensor_sq212) sensor
 *
 * @ingroup sensor_sq212
 */
class ApogeeSQ212 : public Sensor {
 public:
    /**
     * @brief Construct a new Apogee SQ-212 object - need the power pin and the
     * analog data channel.
     *
     * By default, this constructor will internally create a default
     * AnalogVoltageBase implementation for voltage readings, but a pointer to
     * a custom AnalogVoltageBase object can be passed in if desired.
     *
     * @param powerPin The pin on the mcu controlling power to the Apogee
     * SQ-212.  Use -1 if it is continuously powered.
     * - The SQ-212 requires 3.3 to 24 V DC; current draw 10 µA
     * @param analogChannel The analog data channel or processor pin for voltage
     * measurements.  The significance of the channel number depends on the
     * specific AnalogVoltageBase implementation used for voltage readings. For
     * example, with the TI ADS1x15, this would be the ADC channel (0-3) that
     * the sensor is connected to.  Negative or invalid channel numbers are not
     * clamped and will cause the reading to fail and emit a warning.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param analogVoltageReader Pointer to an AnalogVoltageBase object for
     * voltage measurements.  Pass nullptr (the default) to have the constructor
     * internally create and own an analog voltage reader.  For backward
     * compatibility, the default reader uses a TI ADS1115 or ADS1015.  If a
     * non-null pointer is supplied, the caller retains ownership and must
     * ensure its lifetime exceeds that of this object.
     */
    ApogeeSQ212(int8_t powerPin, int8_t analogChannel,
                uint8_t            measurementsToAverage = 1,
                AnalogVoltageBase* analogVoltageReader   = nullptr);
    /**
     * @brief Destroy the ApogeeSQ212 object
     */
    ~ApogeeSQ212();

    // Delete copy constructor and copy assignment operator to prevent shallow
    // copies
    ApogeeSQ212(const ApogeeSQ212&)            = delete;
    ApogeeSQ212& operator=(const ApogeeSQ212&) = delete;

    // Delete move constructor and move assignment operator
    ApogeeSQ212(ApogeeSQ212&&)            = delete;
    ApogeeSQ212& operator=(ApogeeSQ212&&) = delete;

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    /// @brief Pointer to analog voltage reader
    AnalogVoltageBase* _analogVoltageReader = nullptr;
    /// @brief Flag to track if this object owns the analog voltage reader and
    /// should delete it in the destructor
    bool _ownsAnalogVoltageReader = false;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [photosynthetically active radiation (PAR) output](@ref sensor_sq212_par)
 * from an [Apogee SQ-212](@ref sensor_sq212).
 *
 * @ingroup sensor_sq212
 */
/* clang-format on */
class ApogeeSQ212_PAR : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSQ212_PAR object.
     *
     * @param parentSense The parent ApogeeSQ212 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "radiationIncomingPAR".
     */
    explicit ApogeeSQ212_PAR(ApogeeSQ212* parentSense, const char* uuid = "",
                             const char* varCode = SQ212_PAR_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)SQ212_PAR_VAR_NUM,
                   (uint8_t)SQ212_PAR_RESOLUTION, SQ212_PAR_VAR_NAME,
                   SQ212_PAR_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSQ212_PAR object.
     *
     * @note This must be tied with a parent ApogeeSQ212 before it can be used.
     */
    ApogeeSQ212_PAR()
        : Variable((uint8_t)SQ212_PAR_VAR_NUM, (uint8_t)SQ212_PAR_RESOLUTION,
                   SQ212_PAR_VAR_NAME, SQ212_PAR_UNIT_NAME,
                   SQ212_PAR_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSQ212_PAR object - no action needed.
     */
    ~ApogeeSQ212_PAR() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_sq212_voltage) from an
 * [Apogee SQ-212](@ref sensor_sq212).
 *
 * @ingroup sensor_sq212
 */
/* clang-format on */
class ApogeeSQ212_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSQ212_Voltage object.
     *
     * @param parentSense The parent ApogeeSQ212 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SQ212Voltage".
     */
    explicit ApogeeSQ212_Voltage(
        ApogeeSQ212* parentSense, const char* uuid = "",
        const char* varCode = SQ212_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)SQ212_VOLTAGE_VAR_NUM,
                   (uint8_t)SQ212_VOLTAGE_RESOLUTION, SQ212_VOLTAGE_VAR_NAME,
                   SQ212_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSQ212_Voltage object.
     *
     * @note This must be tied with a parent ApogeeSQ212 before it can be used.
     */
    ApogeeSQ212_Voltage()
        : Variable((uint8_t)SQ212_VOLTAGE_VAR_NUM,
                   (uint8_t)SQ212_VOLTAGE_RESOLUTION, SQ212_VOLTAGE_VAR_NAME,
                   SQ212_VOLTAGE_UNIT_NAME, SQ212_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ApogeeSQ212_Voltage object - no action needed.
     */
    ~ApogeeSQ212_Voltage() {}
};
/**@}*/
#endif  // SRC_SENSORS_APOGEESQ212_H_
