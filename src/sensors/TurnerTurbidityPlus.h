/**
 * @file TurnerTurbidityPlus.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Matt Barney <mbarney@tu.org>; edited by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Contains the TurnerTurbidityPlus sensor subclass and the variable
 * subclasses TurnerTurbidityPlus_Turbidity and TurnerTurbidityPlus_Voltage.
 *
 * These are used for the Turner Turbidity Plus.
 *
 * This depends on the Adafruit ADS1X15 v2.x library.
 */
/**
 * @defgroup sensor_turbidity_plus Turner Turbidity Plus
 * Classes for the Turner Turbidity Plus sensor.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 *
 * @section sensor_turbidity_plus_datasheet Sensor Datasheet
 * [Datasheet](http://docs.turnerdesigns.com/t2/doc/brochures/S-0210.pdf)
 *
 * @section sensor_turbidity_plus_flags Build flags
 * - ```-D MS_USE_ADS1015```
 *      - switches from the 16-bit ADS1115 to the 12 bit ADS1015
 * - ```-D TURBIDITY_PLUS_WIPER_TRIGGER_PULSE_MS=x```
 *      - Changes the wiper trigger pulse duration from 50 ms to x ms
 * - ```-D TURBIDITY_PLUS_WIPER_ROTATION_WAIT_MS=x```
 *      - Changes the wiper rotation wait time from 8000 ms to x ms
 *
 * @section sensor_turbidity_plus_ctor Sensor Constructor
 * {{ @ref TurnerTurbidityPlus::TurnerTurbidityPlus }}
 *
 * ___
 * @section sensor_turbidity_plus_examples Example Code
 * The Alphasense CO2 sensor is used in the @menulink{turner_turbidity_plus}
 * example.
 *
 * @menusnip{turner_turbidity_plus}
 */

// Header Guards
#ifndef SRC_SENSORS_TURNERTURBIDITYPLUS_H_
#define SRC_SENSORS_TURNERTURBIDITYPLUS_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_TURNERTURBIDITYPLUS_DEBUG
#define MS_DEBUGGING_STD "TurnerTurbidityPlus"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include "VariableBase.h"
#include "SensorBase.h"
#include "AnalogVoltageBase.h"

/** @ingroup sensor_turbidity_plus */
/**@{*/

/**
 * @anchor sensor_turbidity_plus_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by Turbidity Plus
 */
/**@{*/
/**
 * @brief Sensor::_numReturnedValues; the Turbidity Plus can report 2 values.
 */
#define TURBIDITY_PLUS_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; turbidity is calculated from raw voltage
/// using the input calibration equation.
#define TURBIDITY_PLUS_INC_CALC_VARIABLES 1
/**@}*/

/**
 * @anchor sensor_turbidity_plus_config
 * @name Configuration Defines
 * Defines to set the timing configuration of the Turner Turbidity Plus sensor.
 */
/**@{*/
#if !defined(TURBIDITY_PLUS_WIPER_TRIGGER_PULSE_MS) || defined(DOXYGEN)
/**
 * @brief Wiper trigger pulse duration in milliseconds
 */
#define TURBIDITY_PLUS_WIPER_TRIGGER_PULSE_MS 50
#endif
#if !defined(TURBIDITY_PLUS_WIPER_ROTATION_WAIT_MS) || defined(DOXYGEN)
/**
 * @brief Wait time for wiper rotation to complete in milliseconds
 */
#define TURBIDITY_PLUS_WIPER_ROTATION_WAIT_MS 8000
#endif
/**@}*/

/**
 * @anchor sensor_turbidity_plus_timing
 * @name Sensor Timing
 * The sensor timing for an Turbidity Plus
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the ADS1115 warms up in 2ms.
#define TURBIDITY_PLUS_WARM_UP_TIME_MS 2
/**
 * @brief Sensor::_stabilizationTime_ms; Turner states that it takes 3 sec
 * settling time from power-on.
 */
#define TURBIDITY_PLUS_STABILIZATION_TIME_MS 3000
/**
 *  @brief Sensor::_measurementTime_ms; the ADS1115 completes 860 conversions
 * per second, but the wait for the conversion to complete is built into the
 * underlying library, so we do not need to wait further here.
 */
#define TURBIDITY_PLUS_MEASUREMENT_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_turbidity_plus_output
 * @name Calibrated Turbidity Output
 * The primary turbidity output from an Turbidity Plus
 */
/**@{*/
/// Variable number; the primary variable is stored in sensorValues[0].
#define TURBIDITY_PLUS_VAR_NUM 0
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/);
/// "turbidity"
#define TURBIDITY_PLUS_VAR_NAME "turbidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "nephelometricTurbidityUnit" (NTU)
#define TURBIDITY_PLUS_UNIT_NAME "nephelometricTurbidityUnit"
/// @brief Default variable short code; "TurnerTurbidity"
#define TURBIDITY_PLUS_DEFAULT_CODE "TurnerTurbidity"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; 1.
#define TURBIDITY_PLUS_RESOLUTION 1
#else
/// @brief Decimals places in string representation; 5.
#define TURBIDITY_PLUS_RESOLUTION 5
#endif
/**@}*/

/**
 * @anchor sensor_turbidity_plus_voltage
 * @name Voltage
 * The voltage variable from an Turbidity Plus
 * - Range is 0 to 3.6V when using an ADS1x15 powered at 3.3V
 *     - Full sensor range is 0-5V
 * - Accuracy:
 *     - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset error)
 *       - @m_span{m-dim}(@ref #TURBIDITY_PLUS_VOLTAGE_RESOLUTION = 4)@m_endspan
 *     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15%
 * (gain error), <3 LSB (offset error)
 *       - @m_span{m-dim}(@ref #TURBIDITY_PLUS_VOLTAGE_RESOLUTION = 1)@m_endspan
 *
 * {{ @ref TurnerTurbidityPlus_Voltage::TurnerTurbidityPlus_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[1].
#define TURBIDITY_PLUS_VOLTAGE_VAR_NUM 1
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/); "voltage"
#define TURBIDITY_PLUS_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/units/); "volt" (V)
#define TURBIDITY_PLUS_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "TurbidityPlusVoltage"
#define TURBIDITY_PLUS_VOLTAGE_DEFAULT_CODE "TurbidityPlusVoltage"
#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1.
///  - Resolution:
///     - 12-bit ADC (ADS1015): 2 mV
#define TURBIDITY_PLUS_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4.
///  - Resolution:
///     - 16-bit ADC (ADS1115): 0.125 mV
#define TURBIDITY_PLUS_VOLTAGE_RESOLUTION 4
#endif
/**@}*/
/**
 * @brief The Sensor sub-class for the [Turner Turbidity Plus turbidity
 * sensor](@ref sensor_turbidity_plus).
 *
 * @ingroup sensor_turbidity_plus
 */
class TurnerTurbidityPlus : public Sensor {
 public:
    /**
     * @brief Construct a new Turner Turbidity Plus object - need the power pin,
     * the analog data and reference channels, and the calibration info.
     *
     * By default, this constructor will use a new TIADS1x15Base object with all
     * default values for voltage readings, but a pointer to a custom
     * AnalogVoltageBase object can be passed in if desired.
     *
     * @param powerPin The pin on the mcu controlling power to the Turbidity
     * Plus Use -1 if it is continuously powered.
     * - The Turbidity Plus requires a 3-15V power supply, which can be turned
     * off between measurements.
     * @param wiperTriggerPin The pin on the mcu that triggers the sensor's
     * wiper.
     * @param analogChannel The primary analog channel for differential
     * measurement
     * @param analogReferenceChannel The secondary (reference) analog channel
     * for differential measurement
     * @param conc_std The concentration of the standard used for a 1-point
     * sensor calibration. The concentration units should be the same as the
     * final measuring units.
     * @param volt_std The voltage (in volts) measured for the conc_std.  This
     * voltage should be the final voltage *after* accounting for any voltage
     * dividers or gain settings.
     * @param volt_blank The voltage (in volts) measured for a blank. This
     * voltage should be the final voltage *after* accounting for any voltage
     * dividers or gain settings.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param analogVoltageReader Pointer to an AnalogVoltageBase object for
     * voltage measurements.  Pass nullptr (the default) to have the constructor
     * internally create and own a TIADS1x15Base instance.
     *
     * @attention For 3.3V processors like the Mayfly, The Turner's 0-5V output
     * signal must be shifted down to a maximum of 3.3V. This can be done either
     * with a level-shifting chip (e.g. Adafruit BSS38), OR by connecting the
     * Turner's output signal via a voltage divider. By default, the
     * TurnerTurbidityPlus object does **NOT** include any level-shifting or
     * voltage dividers. To have a voltage divider applied correctly, you must
     * supply a pointer to a custom AnalogVoltageBase object that applies the
     * voltage divider to the raw voltage readings. For example, if you are
     * using a simple voltage divider with two equal resistors, you would need
     * to use an AnalogVoltageBase object that multiplies the raw voltage
     * readings by 2 to account for the halving of the signal by the voltage
     * divider.
     *
     * @warning In library versions 0.37.0 and earlier, a different constructor
     * was used that required an enum object instead of two different analog
     * channel inputs for the differential voltage measurement. If you are using
     * code from a previous version of the library, make sure to update your
     * code to use the new constructor and provide the correct analog channel
     * inputs for the differential voltage measurement.
     */
    TurnerTurbidityPlus(int8_t powerPin, int8_t wiperTriggerPin,
                        int8_t analogChannel, int8_t analogReferenceChannel,
                        float conc_std, float volt_std, float volt_blank,
                        uint8_t            measurementsToAverage = 1,
                        AnalogVoltageBase* analogVoltageReader   = nullptr);
    /**
     * @brief Destroy the Turner Turbidity Plus object
     */
    ~TurnerTurbidityPlus();

    // Delete copy constructor and copy assignment operator to prevent shallow
    // copies
    TurnerTurbidityPlus(const TurnerTurbidityPlus&)            = delete;
    TurnerTurbidityPlus& operator=(const TurnerTurbidityPlus&) = delete;

    // Delete move constructor and move assignment operator
    TurnerTurbidityPlus(TurnerTurbidityPlus&&)            = delete;
    TurnerTurbidityPlus& operator=(TurnerTurbidityPlus&&) = delete;

    String getSensorLocation(void) override;

    /**
     * @brief Run one wiper cycle
     */
    void runWiper(void);

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets pin mode on the wiper trigger pin
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;

    bool wake(void) override;

    void powerUp(void) override;

    void powerDown(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    /**
     * @brief Internal reference to the pin on the mcu that triggers the
     * sensor's wiper.
     */
    int8_t _wiperTriggerPin;
    /**
     * @brief The concentration of the standard used for a 1-point sensor
     * calibration. The concentration units should be the same as the final
     * measuring units.
     */
    float _conc_std;
    /**
     * @brief The voltage (in volts) measured for the conc_std. This voltage
     * should be the final voltage *after* accounting for any voltage dividers
     * or gain settings.
     */
    float _volt_std;
    /**
     * @brief The voltage (in volts) measured for a blank. This voltage should
     * be the final voltage *after* accounting for any voltage.
     */
    float _volt_blank;

    /**
     * @brief The second (reference) pin for differential voltage measurements.
     */
    int8_t _analogReferenceChannel;
    /// @brief Pointer to analog voltage reader
    AnalogVoltageBase* _analogVoltageReader;
    /// @brief Flag to track if this object owns the analog voltage reader and
    /// should delete it in the destructor
    bool _ownsAnalogVoltageReader;
};


// Also returning raw voltage
/**
 * @brief The Variable sub-class used for the [raw voltage output](@ref
 * sensor_turbidity_plus_voltage) from a [Turner Turbidity Plus](@ref
 * sensor_turbidity_plus).
 *
 * This could be helpful if the calibration equation was typed incorrectly or if
 * it is suspected to have changed over time.
 *
 * @ingroup sensor_turbidity_plus
 */
class TurnerTurbidityPlus_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new TurnerTurbidityPlus_Voltage object.
     *
     * @param parentSense The parent TurnerTurbidityPlus providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "TurnerVoltage".
     */
    explicit TurnerTurbidityPlus_Voltage(
        TurnerTurbidityPlus* parentSense, const char* uuid = "",
        const char* varCode = TURBIDITY_PLUS_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)TURBIDITY_PLUS_VOLTAGE_VAR_NUM,
                   (uint8_t)TURBIDITY_PLUS_VOLTAGE_RESOLUTION,
                   TURBIDITY_PLUS_VOLTAGE_VAR_NAME,
                   TURBIDITY_PLUS_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new TurnerTurbidityPlus_Voltage object.
     *
     * @note This must be tied with a parent TurnerTurbidityPlus before it can
     * be used.
     */
    TurnerTurbidityPlus_Voltage()
        : Variable((uint8_t)TURBIDITY_PLUS_VOLTAGE_VAR_NUM,
                   (uint8_t)TURBIDITY_PLUS_VOLTAGE_RESOLUTION,
                   TURBIDITY_PLUS_VOLTAGE_VAR_NAME,
                   TURBIDITY_PLUS_VOLTAGE_UNIT_NAME,
                   TURBIDITY_PLUS_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the TurnerTurbidityPlus_Voltage object - no action needed.
     */
    ~TurnerTurbidityPlus_Voltage() {}
};


/**
 * @brief The Variable sub-class used for the
 * [turbidity output](@ref sensor_turbidity_plus_output) from a
 * [Turner Turbidity Plus](@ref sensor_turbidity_plus).
 *
 * Turbidity is measured (and should be calibrated) in nephelometric turbidity
 * units (NTU).
 *
 *
 * @ingroup sensor_turbidity_plus
 */
class TurnerTurbidityPlus_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new TurnerTurbidityPlus_Turbidity object.
     *
     * @param parentSense The parent TurnerTurbidityPlus providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "TurnerTurbidity".
     */
    explicit TurnerTurbidityPlus_Turbidity(
        TurnerTurbidityPlus* parentSense, const char* uuid = "",
        const char* varCode = TURBIDITY_PLUS_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)TURBIDITY_PLUS_VAR_NUM,
                   (uint8_t)TURBIDITY_PLUS_RESOLUTION, TURBIDITY_PLUS_VAR_NAME,
                   TURBIDITY_PLUS_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new TurnerTurbidityPlus_Turbidity object.
     *
     * @note This must be tied with a parent TurnerTurbidityPlus before it can
     * be used.
     */
    TurnerTurbidityPlus_Turbidity()
        : Variable((uint8_t)TURBIDITY_PLUS_VAR_NUM,
                   (uint8_t)TURBIDITY_PLUS_RESOLUTION, TURBIDITY_PLUS_VAR_NAME,
                   TURBIDITY_PLUS_UNIT_NAME, TURBIDITY_PLUS_DEFAULT_CODE) {}
    /**
     * @brief Destroy the TurnerTurbidityPlus_Turbidity object - no action
     * needed.
     */
    ~TurnerTurbidityPlus_Turbidity() {}
};
/**@}*/

// cspell:words GAIN_TWOTHIRDS

#endif  // SRC_SENSORS_TURNERTURBIDITYPLUS_H_
