/**
 * @file AlphasenseCO2.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written by Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * and Bella Henkel <bella.henkel@mnsu.edu>
 * Adapted from ApogeeSQ212.h and
 * https://github.com/bellahenkel/Soil-Sensing-Device
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AlphasenseCO2 sensor class and the variable subclasses
 * AlphasenseCO2_CO2 and AlphasenseCO2_Voltage.
 *
 * These are used for the Alphasense IRC-A1 Non-Dispersive Infrared (NDIR)
 * Carbon Dioxide (CO2) sensor. This library will almost certainly also work
 * with the Alphasense IRC-AT CO2 sensor (which uses a thermopile detector),
 * although the warmup and stabilization times might be different.
 */
/* clang-format off */
/**
 * @defgroup sensor_alphasense_co2 Alphasense IRC-A1 CO2
 * Classes for the Alphasense IRC-A1 Non-Dispersive Infrared (NDIR)
 * Carbon Dioxide (CO2) sensor.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_alphasense_co2_intro Introduction
 * The [Alphasense IRC-A1 Non-Dispersive Infrared (NDIR) Carbon Dioxide (CO2)
 * sensor](https://www.alphasense.com/products/carbon-dioxide/)
 * The CO2 sensor requires a 2-5 V DC power source with a
 * current draw of 20 to 60 mA.  The power supply to the sensor can be
 * stopped between measurements.
 *
 * To convert the sensor's analog 4-20 mA signal to a high resolution digital
 * signal, the sensor must be attached to an analog-to-digital converter with
 * an resistor in series. Furthermore,
 * https://www.alphasense.com/products/ndir-safety/
 * https://www.alphasense.com/wp-content/uploads/2018/04/IRC-A1.pdf
 * https://www.alphasense.com/wp-content/uploads/2017/09/NDIR-Transmitter.pdf
 * https://www.alphasense.com/wp-content/uploads/2022/10/AAN_202-04_App-Note_V0.pdf
 * https://www.alphasense.com/wp-content/uploads/2022/10/AAN_201-06_App-Note_V0.pdf
 *
 * See the
 * [ADS1115](@ref analog_group) for details on the ADC conversion.
 *
 *
 * @section sensor_alphasense_co2_datasheet Sensor Datasheet
 * [Datasheet](https://www.alphasense.com/wp-content/uploads/2018/04/IRC-A1.pdf)
 *
 * @section sensor_alphasense_co2_flags Build flags
 * - ```-D ALPHASENSE_CO2_SENSE_RESISTOR_OHM=x```
 *      - Changes the sense resistor value from 250.0 ohms to x ohms
 * - ```-D ALPHASENSE_CO2_MFG_SCALE=x```
 *      - Changes the manufacturer scale factor from 312.5 ppm/mA to x ppm/mA
 * - ```-D ALPHASENSE_CO2_MFG_OFFSET=x```
 *      - Changes the manufacturer offset from 1250.0 ppm to x ppm
 * - ```-D ALPHASENSE_CO2_VOLTAGE_MULTIPLIER=x```
 *      - Changes the voltage multiplier from 1.0 to x
 *
 * @section sensor_alphasense_co2_ctor Sensor Constructor
 * {{ @ref AlphasenseCO2::AlphasenseCO2 }}
 *
 * ___
 * @section sensor_alphasense_co2_examples Example Code
 * The Alphasense CO2 sensor is used in the @menulink{alphasense_co2} example.
 *
 * @menusnip{alphasense_co2}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_ALPHASENSECO2_H_
#define SRC_SENSORS_ALPHASENSECO2_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_ALPHASENSECO2_DEBUG
#define MS_DEBUGGING_STD "AlphasenseCO2"
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

/** @ingroup sensor_alphasense_co2 */
/**@{*/

/**
 * @anchor sensor_alphasense_co2_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by the Alphasense CO2 sensor
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the Alphasense CO2 sensor can report 2
/// values, raw voltage and calculated CO2.
#define ALPHASENSE_CO2_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; CO2 is calculated from the raw voltage.
#define ALPHASENSE_CO2_INC_CALC_VARIABLES 1
/**@}*/

/**
 * @anchor sensor_alphasense_co2_config
 * @name Configuration Defines
 * Defines to set the calibration of the Alphasense CO2 sensor.
 */
/**@{*/
#if !defined(ALPHASENSE_CO2_SENSE_RESISTOR_OHM) || defined(DOXYGEN)
/**
 * @brief Sense resistor value in ohms for current conversion
 */
#define ALPHASENSE_CO2_SENSE_RESISTOR_OHM 250.0f
#endif
#if !defined(ALPHASENSE_CO2_MFG_SCALE) || defined(DOXYGEN)
/**
 * @brief Manufacturer scale factor for CO2 conversion (ppm/mA)
 */
#define ALPHASENSE_CO2_MFG_SCALE 312.5f
#endif
#if !defined(ALPHASENSE_CO2_MFG_OFFSET) || defined(DOXYGEN)
/**
 * @brief Manufacturer offset for CO2 conversion (ppm)
 */
#define ALPHASENSE_CO2_MFG_OFFSET 1250.0f
#endif
#if !defined(ALPHASENSE_CO2_VOLTAGE_MULTIPLIER) || defined(DOXYGEN)
/**
 * @brief Voltage multiplier for direct voltage reading
 */
#define ALPHASENSE_CO2_VOLTAGE_MULTIPLIER 1.0f
#endif
/**@}*/

/**
 * @anchor sensor_alphasense_co2_timing
 * @name Sensor Timing
 * The sensor timing for an Alphasense IRC-A1 CO2
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms;
 * The TI ADS1x15 to warm up time is 2 ms, and we get
 * Alphasense CO2 sensor readings in <200 ms second.
 */
#define ALPHASENSE_CO2_WARM_UP_TIME_MS 200
/**
 * @brief Sensor::_stabilizationTime_ms;
 * The manufacturer provides the following stabilization times:
 *   - To final zero ± 100ppm: < 30 s @ 20°C
 *   - To specification: < 30 minutes @ 20°C
 * We found that values leveled off after ~35 s. See:
 * https://github.com/bellahenkel/Soil-Sensing-Device/tree/main/examples/getValuesCO2
 */
#define ALPHASENSE_CO2_STABILIZATION_TIME_MS 35000
/**
 * @brief Sensor::_measurementTime_ms;
 * The Alphasense IRC-A1 CO2 sensor will return a new number every 200 ms,
 * but there appears to be a cyclic response with a ~7 sec period,
 * so we recommend averaging 1 s measurements over 28 seconds (3 periods)
 */
#define ALPHASENSE_CO2_MEASUREMENT_TIME_MS 1000
/**@}*/

/**
 * @anchor sensor_alphasense_co2_co2
 * @name CO2 Concentration
 * The CO2 variable from an Alphasense IRC-A1 CO2
 * - Accuracy is ± 1%FS or ± 50 ppm (for IAQ type)
 * - Range is 0 − 5000 ppm (for IAQ type)
 * - Resolution: < 1 ppm
 *   - 16-bit ADC (ADS1115): < 1 ppm (5 significant figures)
 *   - 12-bit ADC (ADS1015): worse
 *
 * {{ @ref AlphasenseCO2_CO2 }}
 */
/**@{*/
/// Variable number; CO2 is stored in sensorValues[0].
#define ALPHASENSE_CO2_VAR_NUM 0
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/); "carbonDioxide"
#define ALPHASENSE_CO2_VAR_NAME "carbonDioxide"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "partPerMillion" (ppm)
#define ALPHASENSE_CO2_UNIT_NAME "partPerMillion"
/// @brief Default variable short code; "AlphasenseCO2ppm"
#define ALPHASENSE_CO2_DEFAULT_CODE "AlphasenseCO2ppm"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; CO2 should have 0 when
/// using an ADS1015.
#define ALPHASENSE_CO2_RESOLUTION 0
#else
/// @brief Decimals places in string representation; CO2 should have 4 when
/// using an ADS1115.
#define ALPHASENSE_CO2_RESOLUTION 4
#endif
/**@}*/

/**
 * @anchor sensor_alphasense_co2_voltage
 * @name Voltage
 * The voltage variable from an Alphasense IRC-A1 CO2
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
 * {{ @ref AlphasenseCO2_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[1].
#define ALPHASENSE_CO2_VOLTAGE_VAR_NUM 1
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/); "voltage"
#define ALPHASENSE_CO2_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/units/); "volt" (V)
#define ALPHASENSE_CO2_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "AlphasenseCO2Voltage"
#define ALPHASENSE_CO2_VOLTAGE_DEFAULT_CODE "AlphasenseCO2Voltage"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1 when
/// used with an ADS1015.
#define ALPHASENSE_CO2_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4 when
/// used with an ADS1115.
#define ALPHASENSE_CO2_VOLTAGE_RESOLUTION 4
#endif
/**@}*/

/**
 * @brief The Sensor sub-class for the [Alphasense IRC-A1 CO2](@ref
 * sensor_alphasense_co2) sensor
 *
 * @ingroup sensor_alphasense_co2
 */
class AlphasenseCO2 : public Sensor {
 public:
    /**
     * @brief Construct a new Alphasense IRC-A1 CO2 object - need the power pin
     * and the analog data and reference channels.
     *
     * By default, this constructor will internally create a default
     * AnalogVoltageBase implementation for voltage readings, but a pointer to
     * a custom AnalogVoltageBase object can be passed in if desired.
     *
     * @param powerPin The pin on the mcu controlling power to the
     * Alphasense CO2 sensor.  Use -1 if it is continuously powered.
     * - The Alphasense CO2 sensor requires 2-5 V DC; current draw 20-60 mA
     * @param analogChannel The primary analog channel for differential
     * measurement. Negative or invalid channel numbers or pairings between the
     * analogChannel and analogReferenceChannel are not clamped and will cause
     * the reading to fail and emit a warning.
     * @param analogReferenceChannel The secondary (reference) analog channel
     * for differential measurement. Negative or invalid channel numbers or
     * pairings between the analogChannel and analogReferenceChannel are not
     * clamped and will cause the reading to fail and emit a warning.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 7.
     * @param analogVoltageReader Pointer to an AnalogVoltageBase object for
     * voltage measurements.  Pass nullptr (the default) to have the constructor
     * internally create and own an analog voltage reader.  For backward
     * compatibility, the default reader uses a TI ADS1115 or ADS10115.  If a
     * non-null pointer is supplied, the caller retains ownership and must
     * ensure its lifetime exceeds that of this object.
     *
     * @warning In library versions 0.37.0 and earlier, a different constructor
     * was used that required an enum object instead of two different analog
     * channel inputs for the differential voltage measurement. If you are using
     * code from a previous version of the library, make sure to update your
     * code to use the new constructor and provide the correct analog channel
     * inputs for the differential voltage measurement.
     */
    AlphasenseCO2(int8_t powerPin, int8_t analogChannel,
                  int8_t             analogReferenceChannel,
                  uint8_t            measurementsToAverage = 7,
                  AnalogVoltageBase* analogVoltageReader   = nullptr);
    /**
     * @brief Destroy the AlphasenseCO2 object
     */
    ~AlphasenseCO2();

    // Delete copy constructor and copy assignment operator to prevent shallow
    // copies
    AlphasenseCO2(const AlphasenseCO2&)            = delete;
    AlphasenseCO2& operator=(const AlphasenseCO2&) = delete;

    // Delete move constructor and move assignment operator
    AlphasenseCO2(AlphasenseCO2&&)            = delete;
    AlphasenseCO2& operator=(AlphasenseCO2&&) = delete;

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 private:

    /**
     * @brief The second (reference) pin for differential voltage measurements.
     */
    int8_t _analogReferenceChannel = -1;
    /// @brief Pointer to analog voltage reader
    AnalogVoltageBase* _analogVoltageReader = nullptr;
    /// @brief Flag to track if this object owns the analog voltage reader and
    /// should delete it in the destructor
    bool _ownsAnalogVoltageReader = false;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [carbon dioxide (CO2) output](@ref sensor_alphasense_co2_co2)
 * from an [Alphasense IRC-A1 CO2](@ref sensor_alphasense_co2).
 *
 * @ingroup sensor_alphasense_co2
 */
/* clang-format on */
class AlphasenseCO2_CO2 : public Variable {
 public:
    /**
     * @brief Construct a new AlphasenseCO2_CO2 object.
     *
     * @param parentSense The parent AlphasenseCO2 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "radiationIncomingPAR".
     */
    explicit AlphasenseCO2_CO2(
        AlphasenseCO2* parentSense, const char* uuid = "",
        const char* varCode = ALPHASENSE_CO2_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)ALPHASENSE_CO2_VAR_NUM,
                   (uint8_t)ALPHASENSE_CO2_RESOLUTION, ALPHASENSE_CO2_VAR_NAME,
                   ALPHASENSE_CO2_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AlphasenseCO2_CO2 object.
     *
     * @note This must be tied with a parent AlphasenseCO2 before it can be
     * used.
     */
    AlphasenseCO2_CO2()
        : Variable((uint8_t)ALPHASENSE_CO2_VAR_NUM,
                   (uint8_t)ALPHASENSE_CO2_RESOLUTION, ALPHASENSE_CO2_VAR_NAME,
                   ALPHASENSE_CO2_UNIT_NAME, ALPHASENSE_CO2_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AlphasenseCO2_CO2 object - no action needed.
     */
    ~AlphasenseCO2_CO2() {}
};


/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_alphasense_co2_voltage) from an
 * [Alphasense IRC-A1 CO2](@ref sensor_alphasense_co2).
 *
 * @ingroup sensor_alphasense_co2
 */
class AlphasenseCO2_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new AlphasenseCO2_Voltage object.
     *
     * @param parentSense The parent AlphasenseCO2 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AlphasenseCO2Voltage".
     */
    explicit AlphasenseCO2_Voltage(
        AlphasenseCO2* parentSense, const char* uuid = "",
        const char* varCode = ALPHASENSE_CO2_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)ALPHASENSE_CO2_VOLTAGE_VAR_NUM,
                   (uint8_t)ALPHASENSE_CO2_VOLTAGE_RESOLUTION,
                   ALPHASENSE_CO2_VOLTAGE_VAR_NAME,
                   ALPHASENSE_CO2_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AlphasenseCO2_Voltage object.
     *
     * @note This must be tied with a parent AlphasenseCO2 before it can be
     * used.
     */
    AlphasenseCO2_Voltage()
        : Variable((uint8_t)ALPHASENSE_CO2_VOLTAGE_VAR_NUM,
                   (uint8_t)ALPHASENSE_CO2_VOLTAGE_RESOLUTION,
                   ALPHASENSE_CO2_VOLTAGE_VAR_NAME,
                   ALPHASENSE_CO2_VOLTAGE_UNIT_NAME,
                   ALPHASENSE_CO2_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AlphasenseCO2_Voltage object - no action needed.
     */
    ~AlphasenseCO2_Voltage() {}
};
/**@}*/
#endif  // SRC_SENSORS_ALPHASENSECO2_H_
