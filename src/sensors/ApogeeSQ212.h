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
 * These are used for the Apogee SQ-212 Quantum Light sensor.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 *
 * @copydetails ApogeeSQ212
 */

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
#define ADS1115_ADDRESS 0x48
// 1001 000 (ADDR = GND)

#define SQ212_NUM_VARIABLES 2
// Using the warm-up time of the ADS1115
#define SQ212_WARM_UP_TIME_MS 2
// These times are not known!
#define SQ212_STABILIZATION_TIME_MS 2
#define SQ212_MEASUREMENT_TIME_MS 2

#define SQ212_PAR_VAR_NUM 0
#ifdef MS_USE_ADS1015
#define SQ212_PAR_RESOLUTION 0
#else
#define SQ212_PAR_RESOLUTION 4
#endif

#define SQ212_VOLTAGE_VAR_NUM 1
#ifdef MS_USE_ADS1015
#define SQ212_VOLT_RESOLUTION 1
#else
#define SQ212_VOLT_RESOLUTION 4
#endif

/**
 * @brief The main class for the Apogee SQ-212 sensor
 *
 * The Apogee SQ-212 Quantum Light sensor measures photosynthetically active
 * radiation (PAR) and is typically defined as total radiation across a range of
 * 400 to 700 nm. PAR is often expressed as photosynthetic photon flux density
 * (PPFD): photon flux in units of micromoles per square meter per second (μmol
 * m-2 s-1, equal to microEinsteins per square meter per second) summed from 400
 * to 700 nm.
 *
 * For photosynthetically active radiation (PAR):
 *   - Range is 0 to 2500 µmol m-2 s-1
 *   - Accuracy is ± 0.5%
 *   - Resolution:
 *     - 16-bit ADC: 0.3125 µmol m-2 s-1 (ADS1115)
 *     - 12-bit ADC: 5 µmol m-2 s-1 (ADS1015)
 *   - Reported as microeinsteins per square meter per second
 *   - Result stored as sensorValues[0]
 *
 * For raw voltage as reported by ADS1x15:
 *   - Range is 0 to 3.6V [when ADC is powered at 3.3V]
 *   - Accuracy is ± 0.5%
 *     - 16-bit ADC: < 0.25% (gain error), <0.25 LSB (offset errror)
 *     - 12-bit ADC: < 0.15% (gain error), <3 LSB (offset errror)
 *   - Resolution [assuming the ADC is powered at 3.3V with inbuilt gain set to
 *     1 (0-4.096V)]:
 *     - 16-bit ADC: 0.125 mV (ADS1115)
 *     - 12-bit ADC: 2 mV (ADS1015)
 *   - Reported as volts
 *   - Result stored as sensorValues[1]
 *
 * Technical specifications for the Apogee SQ-212 can be found at:
 * https://www.apogeeinstruments.com/sq-212-amplified-0-2-5-volt-sun-calibration-quantum-sensor/
 *
 * Power supply: 5-24 V DC with a nominal current draw of 300 μA
 *
 * Response time: < 1ms
 * Resample time: max of ADC (860/sec)
 */
class ApogeeSQ212 : public Sensor {
 public:
    // The constructor - need the power pin and the data channel on the ADS1x15
    /**
     * @brief Construct a new Apogee SQ-212 object - need the power pin and the
     * data channel on the ADS1x15
     *
     * @param powerPin The pin on the mcu controlling power to the Apogee
     * SQ-212.  Use -1 if the sensor is continuously powered.
     * @param adsChannel The ACS channel the Apogee SQ-212 is connected to
     * (0-3).
     * @param i2cAddress The I2C address of the ADS 1x15, defaults to 0x48.
     * @param measurementsToAverage
     */
    ApogeeSQ212(int8_t powerPin, uint8_t adsChannel,
                uint8_t i2cAddress            = ADS1115_ADDRESS,
                uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the ApogeeSQ212 object - no action needed
     */
    ~ApogeeSQ212();

    /**
     * @brief Get the pin or connection location between the mcu and the sensor.
     * For the SQ-212 returns the I1C address of the ADS and the channel that
     * the SQ-212 is attached to.
     *
     * @return String Text describing how the sensor is attached to the mcu.
     */
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    uint8_t _adsChannel;
    uint8_t _i2cAddress;
};


/**
 * @brief The variable class used for photosynthetically active radiation (PAR)
 * measured by an Apogee SQ-212.
 *
 * For photosynthetically active radiation (PAR):
 *   - Range is 0 to 2500 µmol m-2 s-1
 *   - Accuracy is ± 0.5%
 *   - Resolution:
 *     - 16-bit ADC: 0.3125 µmol m-2 s-1 (ADS1115)
 *     - 12-bit ADC: 5 µmol m-2 s-1 (ADS1015)
 *   - Reported as microeinsteins per square meter per second
 *   - Result stored as sensorValues[0]
 */
class ApogeeSQ212_PAR : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSQ212_PAR object.
     *
     * @param parentSense The parent ApogeeSQ212 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is radiationIncomingPAR.
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


/**
 * @brief The variable class used for raw voltage measured by an Apogee SQ-212.
 *
 * For raw voltage as reported by ADS1x15:
 *   - Range is 0 to 3.6V [when ADC is powered at 3.3V]
 *   - Accuracy is ± 0.5%
 *     - 16-bit ADC: < 0.25% (gain error), <0.25 LSB (offset errror)
 *     - 12-bit ADC: < 0.15% (gain error), <3 LSB (offset errror)
 *   - Resolution [assuming the ADC is powered at 3.3V with inbuilt gain set to
 *     1 (0-4.096V)]:
 *     - 16-bit ADC: 0.125 mV (ADS1115)
 *     - 12-bit ADC: 2 mV (ADS1015)
 *   - Reported as volts
 *   - Result stored as sensorValues[1]
 */
class ApogeeSQ212_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new ApogeeSQ212_Voltage object.
     *
     * @param parentSense The parent ApogeeSQ212 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is SQ212Voltage.
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
