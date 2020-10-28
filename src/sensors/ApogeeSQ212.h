/**
 * @file ApogeeSQ212.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.h by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ApogeeSQ212 sensor subclass and the variable subclasses
 * ApogeeSQ212_PAR and ApogeeSQ212_Voltage.
 *
 * These are used for the Apogee SQ-212 quantum light sensor.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 */
/* clang-format off */
/**
 * @defgroup sq212_group Apogee SQ-212
 * Classes for the Apogee SQ-212 quantum light sensor.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sq212_intro Introduction
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
 * @section sq212_datasheet Sensor Datasheet
 * [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Apogee
 * SQ-212-215 Manual.pdf)
 *
 * @section sq212_sensor The SQ-212 Sensor
 * @ctor_doc{ApogeeSQ212, int8_t powerPin, uint8_t adsChannel, uint8_t i2cAddress, uint8_t measurementsToAverage}
 * @subsection sq212_timing Sensor Timing
 * - Response time (of the ADC): < 1ms
 * - Resample time: max of ADC (860/sec)
 * @subsection sq212_flags Build flags
 * - ```-D MS_USE_ADS1015```
 *      - switches from the 16-bit ADS1115 to the 12 bit ADS1015
 * - ```-D SQ212_CALIBRATION_FACTOR=x```
 *      - Changes the calibration factor from 1 to x
 *
 * ___
 * @section sq212_par PAR Output
 * - Range is 0 to 2500 µmol m-2 s-1
 * - Accuracy is ± 0.5%
 * - Resolution:
 *   - 16-bit ADC: 0.3125 µmol m-2 s-1 (ADS1115)
 *   - 12-bit ADC: 5 µmol m-2 s-1 (ADS1015)
 * - Reported as microeinsteins per square meter per second (µE m-2 s-1 or µmol m-2 s-1)
 * - Result stored in sensorValues[0]
 * - Default variable code is radiationIncomingPAR
 *
 * @variabledoc{sq212_par,ApogeeSQ212,PAR,radiationIncomingPAR}
 *
 * ___
 *
 * @section sq212_voltage Raw Voltage Output
 * - Range is 0 to 3.6V [when ADC is powered at 3.3V]
 * - Accuracy is ± 0.5%
 *   - 16-bit ADC: < 0.25% (gain error), <0.25 LSB (offset error)
 *   - 12-bit ADC: < 0.15% (gain error), <3 LSB (offset error)
 * - Resolution [assuming the ADC is powered at 3.3V with inbuilt gain set to 1
 * (0-4.096V)]:
 *   - 16-bit ADC: 0.125 mV (ADS1115)
 *   - 12-bit ADC: 2 mV (ADS1015)
 * - Reported as volts (V)
 * - Result stored in sensorValues[1]
 * - Default variable code is SQ212Voltage
 *
 * @variabledoc{sq212_voltage,ApogeeSQ212,Voltage,SQ212Voltage}
 *
 * ___
 * @section sq212_examples Example Code
 * The SQ-212 is used in the @menulink{sq212} example.
 *
 * @menusnip{sq212}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_APOGEESQ212_H_
#define SRC_SENSORS_APOGEESQ212_H_

// Debugging Statement
// #define MS_APOGEESQ212_DEBUG

#ifdef MS_APOGEESQ212_DEBUG
#define MS_DEBUGGING_STD "ApogeeSQ212"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the SQ212 can report 2 values.
#define SQ212_NUM_VARIABLES 2
/**
 * @brief Sensor::_warmUpTime_ms; the ADS1115 warms up in 2ms.
 *
 * The warm up time of the SQ-212 itself is not known!
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
/// Sensor::_measurementTime_ms; SQ212 takes 2ms to complete a measurement.
#define SQ212_MEASUREMENT_TIME_MS 2

/// Variable number; PAR is stored in sensorValues[0].
#define SQ212_PAR_VAR_NUM 0
#ifdef MS_USE_ADS1015
/// Decimals places in string representation; PAR should have 0.
#define SQ212_PAR_RESOLUTION 0
#else
/// Decimals places in string representation; PAR should have 4.
#define SQ212_PAR_RESOLUTION 4
#endif

/// Variable number; voltage is stored in sensorValues[1].
#define SQ212_VOLTAGE_VAR_NUM 1
#ifdef MS_USE_ADS1015
/// Decimals places in string representation; voltage should have 1.
#define SQ212_VOLT_RESOLUTION 1
#else
/// Decimals places in string representation; voltage should have 4.
#define SQ212_VOLT_RESOLUTION 4
#endif

/**
 * @brief The calibration factor between output in volts and PAR
 * (microeinsteinPerSquareMeterPerSecond) 1 µmol mˉ² sˉ¹ per mV (reciprocal of
 * sensitivity)
 */
#ifndef SQ212_CALIBRATION_FACTOR
#define SQ212_CALIBRATION_FACTOR 1
#endif

/// The assumed address of the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48

/**
 * @brief The Sensor sub-class for the [Apogee SQ-212](@ref sq212_group) sensor
 *
 * @ingroup sq212_group
 */
class ApogeeSQ212 : public Sensor {
 public:
    /**
     * @brief Construct a new Apogee SQ-212 object - need the power pin and the
     * data channel on the ADS1x15.
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core. Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The pin on the mcu controlling power to the Apogee
     * SQ-212.  Use -1 if it is continuously powered.
     * - The SQ-212 requires 3.3 to 24 V DC; current draw 10 µA
     * - The ADS1115 requires 2.0-5.5V but is assumed to be powered at 3.3V
     * @param adsChannel The analog data channel the Apogee SQ-212 is connected
     * to _on the TI ADS1115_ (0-3).
     * @param i2cAddress The I2C address of the ADS 1x15, default is 0x48 (ADDR
     * = GND)
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @note  The ADS is expected to be either continuously powered or have
     * its power controlled by the same pin as the SQ-212.  This library does
     * not support any other configuration.
     */
    ApogeeSQ212(int8_t powerPin, uint8_t adsChannel,
                uint8_t i2cAddress            = ADS1115_ADDRESS,
                uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the ApogeeSQ212 object - no action needed
     */
    ~ApogeeSQ212();

    /**
     * @brief Report the I1C address of the ADS and the channel that the SQ-212
     * is attached to.
     *
     * @return **String** Text describing how the sensor is attached to the mcu.
     */
    String getSensorLocation(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    uint8_t _adsChannel;
    uint8_t _i2cAddress;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [photosynthetically active radiation (PAR) output](@ref sq212_par)
 * from an [Apogee SQ-212](@ref sq212_group).
 *
 * @ingroup sq212_group
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
    explicit ApogeeSQ212_PAR(
        ApogeeSQ212* parentSense, const char* uuid = "",
        const char* varCode = "photosyntheticallyActiveRadiation")
        : Variable(parentSense, (const uint8_t)SQ212_PAR_VAR_NUM,
                   (uint8_t)SQ212_PAR_RESOLUTION, "radiationIncomingPAR",
                   "microeinsteinPerSquareMeterPerSecond", varCode, uuid) {}
    /**
     * @brief Construct a new ApogeeSQ212_PAR object.
     *
     * @note This must be tied with a parent ApogeeSQ212 before it can be used.
     */
    ApogeeSQ212_PAR()
        : Variable((const uint8_t)SQ212_PAR_VAR_NUM,
                   (uint8_t)SQ212_PAR_RESOLUTION, "radiationIncomingPAR",
                   "microeinsteinPerSquareMeterPerSecond",
                   "photosyntheticallyActiveRadiation") {}
    /**
     * @brief Destroy the ApogeeSQ212_PAR object - no action needed.
     */
    ~ApogeeSQ212_PAR() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [raw voltage output](@ref sq212_voltage) from an
 * [Apogee SQ-212](@ref sq212_group).
 *
 * @ingroup sq212_group
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
    explicit ApogeeSQ212_Voltage(ApogeeSQ212* parentSense,
                                 const char*  uuid    = "",
                                 const char*  varCode = "SQ212Voltage")
        : Variable(parentSense, (const uint8_t)SQ212_VOLTAGE_VAR_NUM,
                   (uint8_t)SQ212_VOLT_RESOLUTION, "voltage", "volt", varCode,
                   uuid) {}
    /**
     * @brief Construct a new ApogeeSQ212_Voltage object.
     *
     * @note This must be tied with a parent ApogeeSQ212 before it can be used.
     */
    ApogeeSQ212_Voltage()
        : Variable((const uint8_t)SQ212_VOLTAGE_VAR_NUM,
                   (uint8_t)SQ212_VOLT_RESOLUTION, "voltage", "volt",
                   "SQ212Voltage") {}
    /**
     * @brief Destroy the ApogeeSQ212_Voltage object - no action needed.
     */
    ~ApogeeSQ212_Voltage() {}
};

#endif  // SRC_SENSORS_APOGEESQ212_H_
