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
 * @note This has a long (10s) stabilization time!
 */
/* clang-format off */
/**
 * @defgroup atlas_co2_group Atlas Scientific EZO-CO2 Embedded NDIR CO2 %Sensor
 * Classes for the [Atlas EZO CO2 circuit](@ref atlas_co2_page).
 *
 * @ingroup atlas_group
 *
 * @copydoc atlas_co2_page
 */
/* clang-format on */
/* clang-format off */
/**
 * @page atlas_co2_page Atlas Scientific EZO-CO2 Embedded NDIR CO2 %Sensor
 *
 * @tableofcontents
 *
 * @section atlas_co2_datasheet Sensor Datasheet
 * Documentation on the CO2 circuit and probe is available here:
 * https://www.atlas-scientific.com/probes/ezo-co2-carbon-dioxide-sensor/
 *
 * @section atlas_co2_sensor The Atlas CO2 Sensor
 * @ctor_doc{AtlasScientificCO2, int8_t powerPin, uint8_t i2cAddress, uint8_t measurementsToAverage}
 * @subsection atlas_co2_timing Sensor Timing
 *   - warms up in 850ms
 *   - not stable until **10s** after warm-up
 *   - measurements take 900ms to complete
 *
 * @note This has a long (10s) stabilization time!
 *
 * ___
 * @section atlas_co2_co2 Carbon Dioxide Output
 * @variabledoc{AtlasScientificCO2,CO2}
 *   - Resolution is 1 ppm
 *   - Accuracy is ± 3% or ± 30 ppm
 *   - Range is 0 − 10000 ppm
 *   - Reported as parts per million (ppm)
 *   - Result stored in sensorValues[0]
 *   - Default variable code is AtlasCO2ppm
 *
 * ___
 * @section atlas_co2_temp Temperature Output
 * @variabledoc{AtlasScientificCO2,Temp}
 *   - Resolution is 1°C
 *   - Accuracy is not reported on the sensor datasheet
 *   - Range is -20°C to +50°C
 *   - Reported as degrees Celsius (°C)
 *   - Result stored in sensorValues[1]
 *   - Default variable code is AtlasCO2Temp
 *
 * @note According to the probe datasheet, the temperature measurement is only
 * intended to be used to verify that the sensor is in equilibrium with its
 * surroundings.
 *
 * ___
 * @section atlas_co2_examples Example Code
 * The Atlas CO2 sensor is used in the @menulink{atlas_co2} example.
 *
 * @menusnip{atlas_co2}
 */
/* clang-format on */

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
/// Sensor::_stabilizationTime_ms; the Atlas CO2 sensor is stable after 10000ms.
#define ATLAS_CO2_STABILIZATION_TIME_MS 10000
/**
 * @brief Sensor::_measurementTime_ms; the Atlas CO2 sensor takes 900ms to
 * complete a measurement.
 */
#define ATLAS_CO2_MEASUREMENT_TIME_MS 900

/// Decimals places in string representation; CO2 should have 1.
#define ATLAS_CO2_RESOLUTION 1
/// Variable number; CO2 is stored in sensorValues[0].
#define ATLAS_CO2_VAR_NUM 0

/// Decimals places in string representation; CO2TEMP should have 0.
#define ATLAS_CO2TEMP_RESOLUTION 0
/// Variable number; CO2TEMP is stored in sensorValues[1].
#define ATLAS_CO2TEMP_VAR_NUM 1

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Atlas Scientific gaseous CO2 and temperature sensor](@ref atlas_co2_page)
 *  - used for any sensor attached to an
 * [Atlas EZO CO2 circuit](https://www.atlas-scientific.com/probes/ezo-co2-carbon-dioxide-sensor/).
 *
 * @ingroup atlas_co2_group
 */
/* clang-format on */
class AtlasScientificCO2 : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific CO2 object
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas CO2
     * circuit.  Use -1 if it is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @param i2cAddressHex The I2C address of the Atlas circuit; optional with
     * the Atlas-supplied default address of 0x69.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @warning **You must isolate the data lines of all Atlas circuits from the
     * main I2C bus if you wish to turn off their power!**  If you do not
     * isolate them from your main I2C bus and you turn off power to the
     * circuits between measurements the I2C lines will be pulled down to ground
     * causing the I2C bus (and thus your logger) to crash.
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
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [CO2 concentration output](@ref atlas_co2_co2) from an
 * [Atlas Scientific CO2 circuit](@ref atlas_co2_page).
 *
 * @ingroup atlas_co2_group
 */
/* clang-format on */
class AtlasScientificCO2_CO2 : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificCO2_CO2 object.
     *
     * @param parentSense The parent AtlasScientificCO2 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AtlasCO2ppm".
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

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref atlas_co2_temp) from an
 * [Atlas Scientific CO2 circuit](@ref atlas_co2_page).
 *
 *
 * @ingroup atlas_co2_group
 */
/* clang-format on */
class AtlasScientificCO2_Temp : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificCO2_Temp object.
     *
     * @param parentSense The parent AtlasScientificCO2 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AtlasCO2Temp".
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
