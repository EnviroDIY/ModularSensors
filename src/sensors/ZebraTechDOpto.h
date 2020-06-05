/**
 * @file ZebraTechDOpto.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ZebraTechDOpto sensor subclass and the variable
 * subclasses ZebraTechDOpto_Temp, ZebraTechDOpto_DOpct, and
 * ZebraTechDOpto_DOmgL.
 *
 * These are for the ZebraTech D-Opto digital dissolved oxygen sensor.
 *
 * This depends on the SDI12Sensors parent class.
 *
 * The manual for this sensor is available at:
 * www.zebra-tech.co.nz/wp-content/uploads/2014/04/D-Opto-Sensor-manual-A4-ver-2.pdf
 *
 * For Dissolved Oxygen:
 *     Accuracy is 1% of reading or 0.02PPM, whichever is greater
 *     Resolution is 0.01% / 0.001 PPM
 *
 * For Temperature:
 *     Accuracy is ± 0.1°C
 *     Resolution is 0.01°C
 *
 * Maximum warm-up time in SDI-12 mode: ~250ms
 * Excitiation time before measurement: ~5225ms
 * Maximum measurement duration: ~110ms
 *
 * Obnoxiously, the sensor will not take a "concurrent" measurement and leave
 * the sensor powered on, so we must wait entire ~5200ms exitation time and the
 * 110ms measurement time each time.
 * There is the ability to do a non-concurrent measurement and leave the
 * sensor powered on, in which case the re-measurement takes ~110ms, but doing
 * it that way the sensor would send an interruption when it was finished,
 * possibly colliding with and confusing other sensor results
 *
 */

// Header Guards
#ifndef SRC_SENSORS_ZEBRATECHDOPTO_H_
#define SRC_SENSORS_ZEBRATECHDOPTO_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the D-Opto can report 3 values.
#define DOPTO_NUM_VARIABLES 3
/// Sensor::_warmUpTime_ms; the D-Opto warms up in 275ms.
#define DOPTO_WARM_UP_TIME_MS 275
/// Sensor::_stabilizationTime_ms; the D-Opto is stable after 0ms.
#define DOPTO_STABILIZATION_TIME_MS 0
/// Sensor::_measurementTime_ms; the D-Opto takes 5335ms to complete a
/// measurement.
#define DOPTO_MEASUREMENT_TIME_MS 5335

/// Decimals places in string representation; temperature should have 2.
#define DOPTO_TEMP_RESOLUTION 2
/// Variable number; temperature is stored in sensorValues[0].
#define DOPTO_TEMP_VAR_NUM 0

/// Decimals places in string representation; dissolved oxygen percent should
/// have 2.
#define DOPTO_DOPCT_RESOLUTION 2
/// Variable number; dissolved oxygen percent is stored in sensorValues[1]
#define DOPTO_DOPCT_VAR_NUM 1

/// Decimals places in string representation; dissolved oxygen concentration
/// should have 3.
#define DOPTO_DOMGL_RESOLUTION 3
/// Variable number; dissolved oxygen concentration is stored in sensorValues[2]
#define DOPTO_DOMGL_VAR_NUM 2

// The main class for the D-Opto
class ZebraTechDOpto : public SDI12Sensors {
 public:
    // Constructors with overloads
    ZebraTechDOpto(char SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                       DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS,
                       DOPTO_MEASUREMENT_TIME_MS) {}
    ZebraTechDOpto(char* SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                       DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS,
                       DOPTO_MEASUREMENT_TIME_MS) {}
    ZebraTechDOpto(int SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                       DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS,
                       DOPTO_MEASUREMENT_TIME_MS) {}
    ~ZebraTechDOpto() {}
};


// Defines the Temperature Variable
class ZebraTechDOpto_Temp : public Variable {
 public:
    /**
     * @brief Construct a new ZebraTechDOpto_Temp object.
     *
     * @param parentSense The parent ZebraTechDOpto providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is DOtempC
     */
    explicit ZebraTechDOpto_Temp(ZebraTechDOpto* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "DOtempC")
        : Variable(parentSense, (const uint8_t)DOPTO_TEMP_VAR_NUM,
                   (uint8_t)DOPTO_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new ZebraTechDOpto_Temp object.
     *
     * @note This must be tied with a parent ZebraTechDOpto before it can be
     * used.
     */
    ZebraTechDOpto_Temp()
        : Variable((const uint8_t)DOPTO_TEMP_VAR_NUM,
                   (uint8_t)DOPTO_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "DOtempC") {}
    /**
     * @brief Destroy the ZebraTechDOpto_Temp object - no action needed.
     */
    ~ZebraTechDOpto_Temp() {}
};


// Defines the Dissolved Oxygen Percent Saturation
class ZebraTechDOpto_DOpct : public Variable {
 public:
    /**
     * @brief Construct a new ZebraTechDOpto_DOpct object.
     *
     * @param parentSense The parent ZebraTechDOpto providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is DOpercent
     */
    explicit ZebraTechDOpto_DOpct(ZebraTechDOpto* parentSense,
                                  const char*     uuid    = "",
                                  const char*     varCode = "DOpercent")
        : Variable(parentSense, (const uint8_t)DOPTO_DOPCT_VAR_NUM,
                   (uint8_t)DOPTO_DOPCT_RESOLUTION,
                   "oxygenDissolvedPercentOfSaturation", "percent", varCode,
                   uuid) {}
    /**
     * @brief Construct a new ZebraTechDOpto_DOpct object.
     *
     * @note This must be tied with a parent ZebraTechDOpto before it can be
     * used.
     */
    ZebraTechDOpto_DOpct()
        : Variable((const uint8_t)DOPTO_DOPCT_VAR_NUM,
                   (uint8_t)DOPTO_DOPCT_RESOLUTION,
                   "oxygenDissolvedPercentOfSaturation", "percent",
                   "DOpercent") {}
    /**
     * @brief Destroy the ZebraTechDOpto_DOpct object - no action needed.
     */
    ~ZebraTechDOpto_DOpct() {}
};


// Defines the Dissolved Oxygen Concentration
class ZebraTechDOpto_DOmgL : public Variable {
 public:
    /**
     * @brief Construct a new ZebraTechDOpto_DOmgL object.
     *
     * @param parentSense The parent ZebraTechDOpto providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is DOppm
     */
    explicit ZebraTechDOpto_DOmgL(ZebraTechDOpto* parentSense,
                                  const char*     uuid    = "",
                                  const char*     varCode = "DOppm")
        : Variable(parentSense, (const uint8_t)DOPTO_DOMGL_VAR_NUM,
                   (uint8_t)DOPTO_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", varCode, uuid) {}
    /**
     * @brief Construct a new ZebraTechDOpto_DOmgL object.
     *
     * @note This must be tied with a parent ZebraTechDOpto before it can be
     * used.
     */
    ZebraTechDOpto_DOmgL()
        : Variable((const uint8_t)DOPTO_DOMGL_VAR_NUM,
                   (uint8_t)DOPTO_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", "DOppm") {}
    /**
     * @brief Destroy the ZebraTechDOpto_DOmgL object - no action needed.
     */
    ~ZebraTechDOpto_DOmgL() {}
};

#endif  // SRC_SENSORS_ZEBRATECHDOPTO_H_
