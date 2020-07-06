/**
 * @file AtlasScientificCO2.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AtlasScientificCO2 subclass of the AtlasParent sensor
 * class along with the variable subclasses AtlasScientificCO2_CO2 and
 * AtlasScientificCO2_Temp.
 *
 * These are used for any sensor attached to an Atlas EZO CO2 circuit.
 *
 * @copydetails AtlasScientificCO2
 *
 * @defgroup atlas_co2_group Atlas Scientific CO2 circuit
 * Classes for the Atlas EZO CO2 circuit
 *
 * @copydetails AtlasScientificCO2
 */

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICCO2_H_
#define SRC_SENSORS_ATLASSCIENTIFICCO2_H_

// Debugging Statement
// #define MS_ATLASSCIENTIFICCO2_DEBUG

#ifdef MS_ATLASSCIENTIFICCO2_DEBUG
#define MS_DEBUGGING_STD "AtlasScientificCO2"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "sensors/AtlasParent.h"

/**
 * @brief Default I2C address is 0x69 (105)
 */
#define ATLAS_CO2_I2C_ADDR 0x69  // 105

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the Atlas CO2 sensor can report 2 values.
#define ATLAS_CO2_NUM_VARIABLES 2
/// @todo (SRGDamia1):  Test timing with sensor
/// Sensor::_warmUpTime_ms; the Atlas CO2 sensor warms up in 850ms.
#define ATLAS_CO2_WARM_UP_TIME_MS 850
// NOTE:  This has a long stabilization time!
/// Sensor::_stabilizationTime_ms; the Atlas CO2 sensor is stable after 10000ms.
#define ATLAS_CO2_STABILIZATION_TIME_MS 10000
/// Sensor::_measurementTime_ms; the Atlas CO2 sensor takes 900ms to complete a
/// measurement.
#define ATLAS_CO2_MEASUREMENT_TIME_MS 900

/// Decimals places in string representation; CO2 should have 1.
#define ATLAS_CO2_RESOLUTION 1
/// Variable number; CO2 is stored in sensorValues[0].
#define ATLAS_CO2_VAR_NUM 0

/// Decimals places in string representation; CO2TEMP should have 0.
#define ATLAS_CO2TEMP_RESOLUTION 0
/// Variable number; CO2TEMP is stored in sensorValues[1].
#define ATLAS_CO2TEMP_VAR_NUM 1

/**
 * @brief The main class for the Atlas Scientific gaseous CO2 and temperature
 * sensor - used for any sensor attached to an Atlas EZO CO2 circuit.
 *
 * Documentation on the CO2 circuit and probe is available here:
 * https://www.atlas-scientific.com/probes/ezo-co2-embedded-ndir-carbon-dioxide-sensor
 *
 * Timing:
 *     - warms up in 850ms
 *     - not stable until 10s after warm-up
 *     - measurements take 900ms to complete
 *
 * For Carbon Dioxide:
 *   @copydetails AtlasScientificCO2_CO2
 *
 * For Temperature:
 *   @copydetails AtlasScientificCO2_Temp
 *
 * @note According to the probe datasheet, the temperature measurement is only
 * intended to be used to verify that the sensor is in equilibrium with its
 * surroundings.
 *
 * @ingroup atlas_group
 * @ingroup atlas_co2_group
 */
class AtlasScientificCO2 : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific CO2 object
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas
     * circuit.  Use -1 if the sensor is continuously powered.
     * @param i2cAddressHex The I2C address of the Atlas circuit.  Defaults to
     * 0x69.
     * @param measurementsToAverage The number of measurements to average.
     * Defaults to 1.
     */
    explicit AtlasScientificCO2(int8_t  powerPin,
                                uint8_t i2cAddressHex = ATLAS_CO2_I2C_ADDR,
                                uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the Atlas Scientific CO2 object - no action needed.
     */
    ~AtlasScientificCO2();

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This begins the Wire library (sets pin modes for I2C), tells the CO2
     * circuit to report all possible measurement parameters, and sets the
     * status bit if successful.  The circuit must be powered for setup.
     *
     * @return **true** The setup was successful
     * @return **false** Some part of the setup failed
     */
    bool setup(void) override;
};

/**
 * @brief The variable class used for CO2 concentration measured by an Atlas
 * Scientific CO2 circuit.
 *
 *   - Resolution is 1 ppm
 *   - Accuracy is ± 3% or ± 30 ppm
 *   - Range is 0 − 10000 ppm
 *   - Reported as parts per million
 *   - Result stored in sensorValues[0]
 *   - Default variable code is AtlasCO2ppm
 *
 * @ingroup atlas_group
 * @ingroup atlas_co2_group
 */
class AtlasScientificCO2_CO2 : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificCO2_CO2 object.
     *
     * @param parentSense The parent AtlasScientificCO2 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AtlasCO2ppm.
     */
    explicit AtlasScientificCO2_CO2(AtlasScientificCO2* parentSense,
                                    const char*         uuid    = "",
                                    const char*         varCode = "AtlasCO2ppm")
        : Variable(parentSense, (const uint8_t)ATLAS_CO2_VAR_NUM,
                   (uint8_t)ATLAS_CO2_RESOLUTION, "carbonDioxide",
                   "partPerMillion", varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificCO2_CO2 object.
     *
     * @note This must be tied with a parent AtlasScientificCO2 before it can be
     * used.
     */
    AtlasScientificCO2_CO2()
        : Variable((const uint8_t)ATLAS_CO2_VAR_NUM,
                   (uint8_t)ATLAS_CO2_RESOLUTION, "carbonDioxide",
                   "partPerMillion", "AtlasCO2ppm") {}
    /**
     * @brief Destroy the AtlasScientificCO2_CO2 object - no action needed.
     */
    ~AtlasScientificCO2_CO2() {}
};

/**
 * @brief The variable class used for temperature measured by an Atlas
 * Scientific CO2 circuit.
 *
 *   - Resolution is 1°C
 *   - Accuracy is not reported on the sensor datasheet
 *   - Range is -20°C to +50°C
 *   - Reported as degrees Celsius
 *   - Result stored in sensorValues[1]
 *   - Default variable code is AtlasCO2Temp
 *
 * @ingroup atlas_group
 * @ingroup atlas_co2_group
 */
class AtlasScientificCO2_Temp : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificCO2_Temp object.
     *
     * @param parentSense The parent AtlasScientificCO2 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AtlasCO2Temp.
     */
    explicit AtlasScientificCO2_Temp(AtlasScientificCO2* parentSense,
                                     const char*         uuid = "",
                                     const char* varCode      = "AtlasCO2Temp")
        : Variable(parentSense, (const uint8_t)ATLAS_CO2TEMP_VAR_NUM,
                   (uint8_t)ATLAS_CO2TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificCO2_Temp object.
     *
     * @note This must be tied with a parent AtlasScientificCO2 before it can be
     * used.
     */
    AtlasScientificCO2_Temp()
        : Variable((const uint8_t)ATLAS_CO2TEMP_VAR_NUM,
                   (uint8_t)ATLAS_CO2TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "AtlasCO2Temp") {}
    /**
     * @brief Destroy the AtlasScientificCO2_Temp object - no action needed.
     */
    ~AtlasScientificCO2_Temp() {}
};

#endif  // SRC_SENSORS_ATLASSCIENTIFICCO2_H_
