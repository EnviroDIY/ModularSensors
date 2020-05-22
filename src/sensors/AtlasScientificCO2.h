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
 * This depends on the Arduino core Wire library.
 *
 * The output from the Atlas Scientifc CO2 is the temperature in degrees C.
 *     Accuracy is ± 3% or ± 30 ppm
 *     Range is 0 − 10000 ppm
 *     Resolution is 1 ppm
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
#define ATLAS_CO2_NUM_VARIABLES 2
// TODO(SRGDamia1):  Test timing with sensor
#define ATLAS_CO2_WARM_UP_TIME_MS 850
// NOTE:  This has a long stabilization time!
#define ATLAS_CO2_STABILIZATION_TIME_MS 10000
#define ATLAS_CO2_MEASUREMENT_TIME_MS 900

#define ATLAS_CO2_RESOLUTION 1
#define ATLAS_CO2_VAR_NUM 0

#define ATLAS_CO2TEMP_RESOLUTION 0
#define ATLAS_CO2TEMP_VAR_NUM 1

/**
 * @brief The main class for the Atlas Scientific CO2 temperature sensor
 */
class AtlasScientificCO2 : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific CO2 object
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas
     * circuit
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

// The class for the CO2 Concentration Variable
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

// The class for the Temp Variable
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
