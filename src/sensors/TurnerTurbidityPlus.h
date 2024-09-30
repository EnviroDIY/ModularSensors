/**
 * @file TurnerTurbidityPlus.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from TurnerCyclops by Matt Barney <mbarney@tu.org>
 *
 * @brief Contains the TurnerTurbidityPlus sensor subclass and the variable subclasses
 * TurnerTurbidityPlus_Turbidity and TurnerTurbidityPlus_Voltage.
 *
 * These are used for the Turner Turbidity Plus.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 */

// Header Guards
#ifndef SRC_SENSORS_TURNERTURBIDITYPLUS_H_
#define SRC_SENSORS_TURNERTURBIDITYPLUS_H_

// Debugging Statement
// #define MS_TURNERTURBIDITYPLUS_DEBUG

#ifdef MS_TURNERTURBIDITYPLUS_DEBUG
#define MS_DEBUGGING_STD "TurnerTurbidityPlus"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_ADS1015.h>

// Sensor Specific Defines
/** @ingroup sensor_turbidity_plus */
/**@{*/
/**
 * @brief Sensor::_numReturnedValues; the Turbidity Plus can report 2 values.
 *
 */
#define TURBIDITY_PLUS_NUM_VARIABLES 2

/**
 * @anchor sensor_turbidity_plus_timing
 * @name Sensor Timing
 * The sensor timing for an Turbidity Plus
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the ADS1115 warms up in 2ms.
#define TURBIDITY_PLUS_WARM_UP_TIME_MS 2
/**
 * @brief Sensor::_stabilizationTime_ms; Turner states that it takes 3 sec settling time from power-on. 
 */
#define TURBIDITY_PLUS_STABILIZATION_TIME_MS 3000
/**
 *  @brief Sensor::_measurementTime_ms; the ADS1115 completes 860 conversions per
 * second, but the wait for the conversion to complete is built into the
 * underlying library, so we do not need to wait further here.
*/
#define TURBIDITY_PLUS_MEASUREMENT_TIME_MS 0
/**@}*/

/* clang-format off */
/**
 * @anchor sensor_turbidity_plus_output
 * @name Calibrated Parameter Output
 * The primary output variable from an Turbidity Plus
 */
/**@{*/
/* clang-format on */
/// Variable number; the primary variable is stored in sensorValues[0].
#define TURBIDITY_PLUS_VAR_NUM 0
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
 *     - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset errror)
 *       - @m_span{m-dim}(@ref #TURBIDITY_PLUS_VOLTAGE_RESOLUTION = 4)@m_endspan
 *     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15%
 * (gain error), <3 LSB (offset errror)
 *       - @m_span{m-dim}(@ref #TURBIDITY_PLUS_VOLTAGE_RESOLUTION = 1)@m_endspan
 *
 * {{ @ref TurnerTurbidityPlus_Voltage::TurnerTurbidityPlus_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[1].
#define TURBIDITY_PLUS_VOLTAGE_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "voltage"
#define TURBIDITY_PLUS_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt"
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

/// @brief The assumed address of the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Turner Turbidity Plus turbidity sensor](@ref sensor_turbidity_plus).
 *
 * @ingroup sensor_turbidity_plus
 */
/* clang-format on */
class TurnerTurbidityPlus : public Sensor {
 public:
    // The constructor - need the power pin, the ADS1X15 data channel, and the
    // calibration info
    /* clang-format off */
    /**
     * @brief Construct a new Turner Turbidity Plus object - need the power pin, the
     * ADS1X15 data channel, and the calibration info.
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core.  Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The pin on the mcu controlling power to the Turbidity Plus
     * Use -1 if it is continuously powered.
     * - The ADS1x15 requires an input voltage of 2.0-5.5V
     * - The Turbidity Plus itself requires a 3-15V power supply, which can be
     * turned off between measurements.
     * @param wiperTriggerPin The pin on the mcu that triggers the sensor's wiper.
     * 
     * @param adsDiffMux Which two pins _on the TI ADS1115_ that will measure 
     * differential voltage from the Turbidity Plus. See Adafruit_ADS1015.h.
     * @param conc_std The concentration of the standard used for a 1-point
     * sensor calibration.  The concentration units should be the same as the
     * final measuring units.
     * @param volt_std The voltage (in volts) measured for the conc_std.  This
     * voltage should be the final voltage *after* accounting for any voltage
     * dividers or gain settings.
     * @param volt_blank The voltage (in volts) measured for a blank.  This
     * voltage should be the final voltage *after* accounting for any voltage
     * dividers or gain settings.
     * @param i2cAddress The I2C address of the ADS 1x15, default is 0x48 (ADDR
     * = GND)
     * @param PGA_gain The programmable gain amplification to set on the 
     * ADS 1x15, default is GAIN_DEFAULT (0).
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param voltageDividerFactor For 3.3V processors like the Mayfly, The 
     * Turner's 0-5V output signal must be shifted down to a maximum of 3.3V.
     * This can be done either either with a level-shifting chip (e.g. Adafruit BSS38),
     * OR by connecting the Turner's output signal via a voltage divider. This 
     * voltageDividerFactor is used for the latter case: e.g., a divider that 
     * uses 2 matched resistors will halve the voltage reading and requires a 
     * voltageDividerFactor of 2. The default value is 1.
     */
    /* clang-format on */
    TurnerTurbidityPlus(int8_t powerPin, int8_t wiperTriggerPin, adsDiffMux_t adsDiffMux, float conc_std,
                  float volt_std, float volt_blank,
                  uint8_t i2cAddress            = ADS1115_ADDRESS,
                  adsGain_t PGA_gain = GAIN_DEFAULT, 
                  uint8_t measurementsToAverage = 1,
                  float voltageDividerFactor = 1);
    /**
     * @brief Destroy the Turner Turbidity Plus object
     */
    ~TurnerTurbidityPlus();

    /**
     * @copydoc Sensor::getSensorLocation()
     */
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

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    int8_t  _wiperTriggerPin;
    adsDiffMux_t _adsDiffMux;
    float   _conc_std, _volt_std, _volt_blank;
    uint8_t _i2cAddress;
    adsGain_t _PGA_gain;
    float _voltageDividerFactor;
};


// Also returning raw voltage
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sensor_turbidity_plus_voltage) from a
 * [Turner Turbidity Plus](@ref sensor_turbidity_plus).
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
        : Variable(parentSense, (const uint8_t)TURBIDITY_PLUS_VOLTAGE_VAR_NUM,
                   (uint8_t)TURBIDITY_PLUS_VOLTAGE_RESOLUTION,
                   TURBIDITY_PLUS_VOLTAGE_VAR_NAME, TURBIDITY_PLUS_VOLTAGE_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new TurnerTurbidityPlus_Voltage object.
     *
     * @note This must be tied with a parent TurnerTurbidityPlus before it can be
     * used.
     */
    TurnerTurbidityPlus_Voltage()
        : Variable((const uint8_t)TURBIDITY_PLUS_VOLTAGE_VAR_NUM,
                   (uint8_t)TURBIDITY_PLUS_VOLTAGE_RESOLUTION,
                   TURBIDITY_PLUS_VOLTAGE_VAR_NAME, TURBIDITY_PLUS_VOLTAGE_UNIT_NAME,
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
    explicit TurnerTurbidityPlus_Turbidity(TurnerTurbidityPlus* parentSense,
                                     const char*    uuid = "",
                                     const char* varCode = "TurnerTurbidity")
        : Variable(parentSense, (const uint8_t)TURBIDITY_PLUS_VAR_NUM,
                   (uint8_t)TURBIDITY_PLUS_RESOLUTION, "Turbidity",
                   "nephelometricTurbidityUnit", varCode, uuid) {}
    /**
     * @brief Construct a new TurnerTurbidityPlus_Turbidity object.
     *
     * @note This must be tied with a parent TurnerTurbidityPlus before it can be
     * used.
     */
    TurnerTurbidityPlus_Turbidity()
        : Variable((const uint8_t)TURBIDITY_PLUS_VAR_NUM, (uint8_t)TURBIDITY_PLUS_RESOLUTION,
                   "Turbidity", "nephelometricTurbidityUnit",
                   "TurnerTurbidity") {}
    ~TurnerTurbidityPlus_Turbidity() {}
};

/**@}*/
#endif  // SRC_SENSORS_TURNERTURBIDITYPLUS_H_
