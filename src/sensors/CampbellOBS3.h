/**
 * @file CampbellOBS3.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the CampbellOBS3 sensor subclass and the variable subclasses
 * CampbellOBS3_Turbidity and CampbellOBS3_Voltage.
 *
 * These are used for the Campbell Scientific OBS-3+.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 */
/* clang-format off */
/**
 * @defgroup sensor_obs3 Campbell OBS3+
 * Classes for the Campbell OBS3+ analog turbidity sensor.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_obs3_intro Introduction
 *
 * @warning This sensor is no longer manufactured.
 *
 * The OBS-3+ puts out a simple analog signal between 0 and 2.5V.  When the
 * sensor is purchased, included in the packaging is a calibration certificate
 * to use to convert the voltage into turbidity.
 *
 * @note The 5V and 4-20mA versions of the OBS3+ are _not_ supported by this
 * library.
 *
 * The OBS3+ supports two different turbidity ranges.  The low and high range
 * signals are read independently of each other - the signals are on different
 * wires.  Each range has a separate calibrations.
 *
 * Before applying any turbidity calibration, the analog output from the OBS3+
 * must be converted into a high resolution digital signal.  See the
 * [ADS1115 page](@ref analog_group) for details on the conversion.
 *
 * @section sensor_obs3_datasheet Sensor Datasheet
 * - [Basic Concepts](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Campbell-OBS3-Basics.pdf)
 * - [Manual](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Campbell-OBS3-Manual.pdf)
 *
 * @note Low and high range are treated as completely independent, so only 2
 * "variables" are measured by each sensor - one for the raw voltage and another
 * for the calibrated turbidity.  To get both high and low range values, create
 * two sensor objects!
 *
 * @section sensor_obs3_flags Build flags
 * - ```-D MS_USE_ADS1015```
 *      - switches from the 16-bit ADS1115 to the 12 bit ADS1015
 *
 * @section sensor_obs3_ctor Sensor Constructor
 * {{ @ref CampbellOBS3::CampbellOBS3 }}
 *
 * ___
 * @section sensor_obs3_examples Example Code
 * The Campbell OBS3+ is used in the @menulink{campbell_obs3} example.
 *
 * @menusnip{campbell_obs3}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_CAMPBELLOBS3_H_
#define SRC_SENSORS_CAMPBELLOBS3_H_

// Debugging Statement
// #define MS_CAMPBELLOBS3_DEBUG

#ifdef MS_CAMPBELLOBS3_DEBUG
#define MS_DEBUGGING_STD "CampbellOBS3"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
/** @ingroup sensor_obs3 */
/**@{*/
/**
 * @brief Sensor::_numReturnedValues; the OBS3 can report 2 values.
 *
 * Low and high range are treated as completely independent, so only 2
 * "variables" are measured by each sensor - one for the raw voltage and another
 * for the calibrated turbidity.  To get both high and low range values, create
 * two sensor objects!
 */
#define OBS3_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; turbidity is calculated from raw voltage
/// using the input calibration equation.
#define OBS3_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_obs3_timing
 * @name Sensor Timing
 * The sensor timing for an OBS3+
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the ADS1115 warms up in 2ms.
#define OBS3_WARM_UP_TIME_MS 2
/// @brief Sensor::_stabilizationTime_ms; minimum stabilization time for the
/// OBS3 is 2s (2000ms).
#define OBS3_STABILIZATION_TIME_MS 2000
/// @brief Sensor::_measurementTime_ms; OBS3 takes 100ms to complete a
/// measurement - Maximum data rate = 10Hz (100ms/sample).
#define OBS3_MEASUREMENT_TIME_MS 100
/**@}*/

/**
 * @anchor sensor_obs3_turbidity
 * @name Turbidity
 * The turbidity variable from an OBS3+
 * - Range: (depends on sediment size, particle shape, and reflectivity)
 *     - Turbidity (low/high):
 *         - T1: 250/1000 NTU
 *         - T2: 500/2000 NTU
 *         - T3: 1000/4000 NTU
 *     - Mud: 5000 to 10,000 mg L–1
 *     - Sand: 50,000 to 100,000 mg L–1
 * - Accuracy: (whichever is larger)
 *     - Turbidity: 2% of reading or 0.5 NTU
 *     - Mud: 2% of reading or 1 mg L–1
 *     - Sand: 4% of reading or 10 mg L–1
 * - Resolution:
 *     - 16-bit ADC, Turbidity:
 *         - T1: 0.03125/0.125 NTU
 *         - T2: 0.0625/0.25 NTU
 *         - T3: 0.125/0.5 NTU
 *         - @m_span{m-dim}@ref #OBS3_RESOLUTION = 5@m_endspan
 *     - 12-bit ADC, Turbidity:
 *         - T1: 0.5/2.0 NTU
 *         - T2: 1.0/4.0 NTU
 *         - T3: 2.0/8.0 NTU
 *         - @m_span{m-dim}@ref #OBS3_RESOLUTION = 1@m_endspan
 *
 * {{ @ref CampbellOBS3_Turbidity::CampbellOBS3_Turbidity }}
 */
/**@{*/
/// Variable number; turbidity is stored in sensorValues[0].
#define OBS3_TURB_VAR_NUM 0
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; turbidity should have 1.
#define OBS3_RESOLUTION 1
#else
/// @brief Decimals places in string representation; turbidity should have 5.
#define OBS3_RESOLUTION 5
#endif
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "turbidity"
#define OBS3_TURB_VAR_NAME "turbidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "nephelometricTurbidityUnit" (NTU)
#define OBS3_TURB_UNIT_NAME "nephelometricTurbidityUnit"
/// @brief Default variable short code; "OBS3Turbidity"
#define OBS3_TURB_DEFAULT_CODE "OBS3Turbidity"
/**@}*/

/**
 * @anchor sensor_obs3_voltage
 * @name Voltage
 * The voltage variable from an OBS3+
 * - Range is 0 to 2.5V
 * - Accuracy:
 *     - 16-bit ADC (ADS1115): < 0.25% (gain error), < 0.25 LSB (offset errror)
 *       - @m_span{m-dim}@ref #OBS3_VOLTAGE_RESOLUTION = 4@m_endspan
 *     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15%
 * (gain error), < 3 LSB (offset errror)
 *       - @m_span{m-dim}@ref #OBS3_VOLTAGE_RESOLUTION = 1@m_endspan
 *
 * {{ @ref CampbellOBS3_Voltage::CampbellOBS3_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[1].
#define OBS3_VOLTAGE_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "voltage"
#define OBS3_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt"
#define OBS3_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "OBS3Voltage"
#define OBS3_VOLTAGE_DEFAULT_CODE "OBS3Voltage"

#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1.
///  - Resolution:
///     - 16-bit ADC (ADS1115): 0.125 mV
#define OBS3_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4.
///  - Resolution:
///     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 2 mV
#define OBS3_VOLTAGE_RESOLUTION 4
#endif
/**@}*/

/// @brief The assumed address of the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Campbell OBS3 analog turbidity sensor](@ref sensor_obs3).
 *
 * Low and high range are treated as completely independent, so only 2
 * "variables" are measured by each sensor - one for the raw voltage and another
 * for the calibrated turbidity.  To get both high and low range values, create
 * two sensor objects!
 *
 * @ingroup sensor_obs3
 */
/* clang-format on */
class CampbellOBS3 : public Sensor {
 public:
    // The constructor - need the power pin, the ADS1X15 data channel, and the
    // calibration info
    /**
     * @brief Construct a new Campbell OBS3 object - need the power pin, the
     * ADS1X15 data channel, and the calibration info.
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core.  Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The pin on the mcu controlling power to the OBS3+
     * Use -1 if it is continuously powered.
     * - The ADS1x15 requires an input voltage of 2.0-5.5V, but this library
     * assumes the ADS is powered with 3.3V.
     * - The OBS-3 itself requires a 5-15V power supply, which can be turned off
     * between measurements.
     * @param adsChannel The analog data channel _on the TI ADS1115_ that the
     * OBS3 is connected to (0-3).
     * @param x2_coeff_A The x2 (A) coefficient for the calibration _in volts_
     * @param x1_coeff_B The x (B) coefficient for the calibration _in volts_
     * @param x0_coeff_C The x0 (C) coefficient for the calibration _in volts_
     * @param i2cAddress The I2C address of the ADS 1x15, default is 0x48 (ADDR
     * = GND)
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    CampbellOBS3(int8_t powerPin, uint8_t adsChannel, float x2_coeff_A,
                 float x1_coeff_B, float x0_coeff_C,
                 uint8_t i2cAddress            = ADS1115_ADDRESS,
                 uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the Campbell OBS3 object
     */
    ~CampbellOBS3();

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
    float   _x2_coeff_A, _x1_coeff_B, _x0_coeff_C;
    uint8_t _i2cAddress;
};


// The main variable returned is turbidity
// To utilize both high and low gain turbidity, you must create *two* sensor
// objects on two different data channels and then create two variable objects,
// one tied to each sensor.
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [turbidity output](@ref sensor_obs3_turbidity) from a [Campbell OBS3+](@ref sensor_obs3).
 *
 * @ingroup sensor_obs3
 */
/* clang-format on */
class CampbellOBS3_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new CampbellOBS3_Turbidity object.
     *
     * @param parentSense The parent CampbellOBS3 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "OBS3Turbidity".
     */
    explicit CampbellOBS3_Turbidity(
        CampbellOBS3* parentSense, const char* uuid = "",
        const char* varCode = OBS3_TURB_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)OBS3_TURB_VAR_NUM,
                   (uint8_t)OBS3_RESOLUTION, OBS3_TURB_VAR_NAME,
                   OBS3_TURB_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new CampbellOBS3_Turbidity object.
     *
     * @note This must be tied with a parent CampbellOBS3 before it can be used.
     */
    CampbellOBS3_Turbidity()
        : Variable((const uint8_t)OBS3_TURB_VAR_NUM, (uint8_t)OBS3_RESOLUTION,
                   OBS3_TURB_VAR_NAME, OBS3_TURB_UNIT_NAME,
                   OBS3_TURB_DEFAULT_CODE) {}
    ~CampbellOBS3_Turbidity() {}
};


// Also returning raw voltage
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_obs3_voltage) from a [Campbell OBS3+](@ref sensor_obs3).
 *
 * This could be helpful if the calibration equation was typed incorrectly.
 *
 * @ingroup sensor_obs3
 */
/* clang-format on */
class CampbellOBS3_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new CampbellOBS3_Voltage object.
     *
     * @param parentSense The parent CampbellOBS3 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "OBS3Voltage".
     */
    explicit CampbellOBS3_Voltage(
        CampbellOBS3* parentSense, const char* uuid = "",
        const char* varCode = OBS3_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)OBS3_VOLTAGE_VAR_NUM,
                   (uint8_t)OBS3_VOLTAGE_RESOLUTION, OBS3_VOLTAGE_VAR_NAME,
                   OBS3_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new CampbellOBS3_Voltage object.
     *
     * @note This must be tied with a parent CampbellOBS3 before it can be used.
     */
    CampbellOBS3_Voltage()
        : Variable((const uint8_t)OBS3_VOLTAGE_VAR_NUM,
                   (uint8_t)OBS3_VOLTAGE_RESOLUTION, OBS3_VOLTAGE_VAR_NAME,
                   OBS3_VOLTAGE_UNIT_NAME, OBS3_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the CampbellOBS3_Voltage object - no action needed.
     */
    ~CampbellOBS3_Voltage() {}
};
/**@}*/
#endif  // SRC_SENSORS_CAMPBELLOBS3_H_
