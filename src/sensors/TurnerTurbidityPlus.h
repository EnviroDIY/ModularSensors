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
#include <Adafruit_ADS1X15.h>

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
 * @anchor sensor__turbidity_plus_config
 * @name Configuration Defines
 * Defines to set the address of the ADD.
 */
/**@{*/
/**
 * @brief Enum for the pins used for differential voltages.
 */
typedef enum : uint16_t {
    DIFF_MUX_0_1,  ///< differential across pins 0 and 1
    DIFF_MUX_0_3,  ///< differential across pins 0 and 3
    DIFF_MUX_1_3,  ///< differential across pins 1 and 3
    DIFF_MUX_2_3   ///< differential across pins 2 and 3
} ttp_adsDiffMux_t;
/// @brief The assumed address of the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48
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
///     - 16-bit ADC (ADS1115): 0.125 mV
#define TURBIDITY_PLUS_VOLTAGE_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4.
///  - Resolution:
///     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): 2 mV
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
    // The constructor - need the power pin, the ADS1X15 data channel, and the
    // calibration info
    /**
     * @brief Construct a new Turner Turbidity Plus object - need the power pin,
     * the ADS1X15 data channel, and the calibration info.
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core. Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The pin on the mcu controlling power to the Turbidity
     * Plus Use -1 if it is continuously powered.
     * - The ADS1x15 requires an input voltage of 2.0-5.5V
     * - The Turbidity Plus itself requires a 3-15V power supply, which can be
     * turned off between measurements.
     * @param wiperTriggerPin The pin on the mcu that triggers the sensor's
     * wiper.
     * @param adsDiffMux Which two pins _on the TI ADS1115_ that will measure
     * differential voltage. See #ttp_adsDiffMux_t
     * @param conc_std The concentration of the standard used for a 1-point
     * sensor calibration. The concentration units should be the same as the
     * final measuring units.
     * @param volt_std The voltage (in volts) measured for the conc_std.  This
     * voltage should be the final voltage *after* accounting for any voltage
     * dividers or gain settings.
     * @param volt_blank The voltage (in volts) measured for a blank. This
     * voltage should be the final voltage *after* accounting for any voltage
     * @param i2cAddress The I2C address of the ADS 1x15, default is 0x48 (ADDR
     * = GND)
     * @param PGA_gain The programmable gain amplification to set on the
     * ADS 1x15, default is GAIN_ONE (0-4.096V).
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param voltageDividerFactor For 3.3V processors like the Mayfly, The
     * Turner's 0-5V output signal must be shifted down to a maximum of 3.3V.
     * This can be done either either with a level-shifting chip (e.g. Adafruit
     * BSS38), OR by connecting the Turner's output signal via a voltage
     * divider. This voltageDividerFactor is used for the latter case: e.g., a
     * divider that uses 2 matched resistors will halve the voltage reading and
     * requires a voltageDividerFactor of 2. The default value is 1.
     */
    TurnerTurbidityPlus(int8_t powerPin, int8_t wiperTriggerPin,
                        ttp_adsDiffMux_t adsDiffMux, float conc_std,
                        float volt_std, float volt_blank,
                        uint8_t   i2cAddress            = ADS1115_ADDRESS,
                        adsGain_t PGA_gain              = GAIN_ONE,
                        uint8_t   measurementsToAverage = 1,
                        float     voltageDividerFactor  = 1);
    /**
     * @brief Destroy the Turner Turbidity Plus object
     */
    ~TurnerTurbidityPlus();

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
     * @brief Which two pins _on the TI ADS1115_ that will measure differential
     * voltage from the Turbidity Plus. See #ttp_adsDiffMux_t
     */
    ttp_adsDiffMux_t _adsDiffMux;
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
     * @brief Internal reference to the I2C address of the TI-ADS1x15
     */
    uint8_t _i2cAddress;
    /**
     * @brief The programmable gain amplification to set on the ADS 1x15,
     * default is GAIN_ONE (+/-4.096V range = Gain 1).
     *
     * Other gain options are:
     *   GAIN_TWOTHIRDS = +/-6.144V range = Gain 2/3,
     *   GAIN_ONE = +/-4.096V range = Gain 1,
     *   GAIN_TWO = +/-2.048V range = Gain 2,
     *   GAIN_FOUR = +/-1.024V range = Gain 4,
     *   GAIN_EIGHT = +/-0.512V range = Gain 8,
     *   GAIN_SIXTEEN = +/-0.256V range = Gain 16
     *
     * @todo Determine gain automatically based on the board voltage?
     */
    adsGain_t _PGA_gain;
    /**
     * @brief For 3.3V processors like the Mayfly, The Turner's 0-5V output
     * signal must be shifted down to a maximum of 3.3V. This can be done either
     * either with a level-shifting chip (e.g. Adafruit BSS38), OR by connecting
     * the Turner's output signal via a voltage divider. This
     * voltageDividerFactor is used for the latter case: e.g., a divider that
     * uses 2 matched resistors will halve the voltage reading and requires a
     * voltageDividerFactor of 2. The default value is 1.
     */
    float _voltageDividerFactor;
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
