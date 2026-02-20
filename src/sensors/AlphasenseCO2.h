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
 *
 * This depends on the Adafruit ADS1X15 v2.x library.
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
 * - ```-D MS_USE_ADS1015```
 *      - switches from the 16-bit ADS1115 to the 12 bit ADS1015
 * - ```-D ALPHASENSE_CO2_CALIBRATION_FACTOR=x```
 *      - Changes the calibration factor from 1 to x
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

/** @ingroup sensor_alphasense_co2 */
/**@{*/

/**
 * @anchor sensor_alphasense_co2_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by the Apogee SQ-212
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the Alphasense CO2 sensor can report 2
/// values, raw voltage and calculated CO2.
#define ALPHASENSE_CO2_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; CO2 is calculated from the raw voltage.
#define ALPHASENSE_CO2_INC_CALC_VARIABLES 1
/**@}*/

/**
 * @anchor sensor__alphasense_co2_config
 * @name Configuration Defines
 * Defines to set the calibration of the Alphasense CO2 sensor and the address
 * of the ADD.
 */
/**@{*/
#if !defined(ALPHASENSE_CO2_CALIBRATION_FACTOR) || defined(DOXYGEN)
/**
 * @brief The calibration factor between output in volts and CO2
 * (microeinsteinPerSquareMeterPerSecond) 1 µmol mˉ² sˉ¹ per mV (reciprocal of
 * sensitivity)
 */
#define ALPHASENSE_CO2_CALIBRATION_FACTOR 1
#endif
/**
 * @brief Enum for the pins used for differential voltages.
 */
typedef enum : uint16_t {
    DIFF_MUX_0_1,  ///< differential across pins 0 and 1
    DIFF_MUX_0_3,  ///< differential across pins 0 and 3
    DIFF_MUX_1_3,  ///< differential across pins 1 and 3
    DIFF_MUX_2_3   ///< differential across pins 2 and 3
} aco2_adsDiffMux_t;
/// @brief The assumed address of the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48
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
     * and the on the ADS1x15. Designed to read differential voltage between ads
     * channels 2 and 3
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core. Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The pin on the mcu controlling power to the
     * Alphasense CO2 sensor.  Use -1 if it is continuously powered.
     * - The Alphasense CO2 sensor requires 2-5 V DC; current draw 20-60 mA
     * - The ADS1115 requires 2.0-5.5V but is assumed to be powered at 3.3V
     * @param adsDiffMux Which two pins _on the TI ADS1115_ that will measure
     * differential voltage. See #aco2_adsDiffMux_t.
     * @param i2cAddress The I2C address of the ADS 1x15, default is 0x48 (ADDR
     * = GND)
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 7 [seconds], which is one period of the cycle.
     * @note  The ADS is expected to be either continuously powered or have
     * its power controlled by the same pin as the Alphasense CO2 sensor.  This
     * library does not support any other configuration.
     */
    AlphasenseCO2(int8_t powerPin, aco2_adsDiffMux_t adsDiffMux = DIFF_MUX_2_3,
                  uint8_t i2cAddress            = ADS1115_ADDRESS,
                  uint8_t measurementsToAverage = 7);
    /**
     * @brief Destroy the AlphasenseCO2 object - no action needed
     */
    ~AlphasenseCO2();

    /**
     * @brief Report the I1C address of the ADS and the channel that the
     * Alphasense CO2 sensor is attached to.
     *
     * @return **String** Text describing how the sensor is attached to the mcu.
     */
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    /**
     * @brief Which two pins _on the TI ADS1115_ that will measure differential
     * voltage from the Turbidity Plus. See #aco2_adsDiffMux_t
     */
    aco2_adsDiffMux_t _adsDiffMux;
    /**
     * @brief Internal reference to the I2C address of the TI-ADS1x15
     */
    uint8_t _i2cAddress;
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
