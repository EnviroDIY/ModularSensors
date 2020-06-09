/**
 * @file KellerAcculevel.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the KellerAcculevel sensor subclass and the
 * KellerAcculevel_Pressure, KellerAcculevel_Temp, and KellerAcculevel_Height
 * variable subclasses.
 *
 * These are for Keller Series 30, Class 5, Group 20 sensors using Modbus
 * communication, that are Software version 5.20-12.28 and later (i.e. made
 * after the 2012 in the 28th week).
 *
 * Only tested on the Acculevel.
 *
 * Documentation for the Keller Protocol commands and responses, along with
 * information about the various variables, can be found in the EnviroDIY
 * KellerModbus library at: https://github.com/EnviroDIY/KellerModbus
 */

// Header Guards
#ifndef SRC_SENSORS_KELLERACCULEVEL_H_
#define SRC_SENSORS_KELLERACCULEVEL_H_

// Included Dependencies
#include "sensors/KellerParent.h"

// Sensor Specific Defines
/// Sensor::_warmUpTime_ms; the Acculevel warms up in 500ms.
#define ACCULEVEL_WARM_UP_TIME_MS 500
/// Sensor::_stabilizationTime_ms; the Acculevel is stable after 5000ms.
#define ACCULEVEL_STABILIZATION_TIME_MS 5000
/// Sensor::_measurementTime_ms; the Acculevel takes 1500ms to complete a
/// measurement.
#define ACCULEVEL_MEASUREMENT_TIME_MS 1500

/// Decimals places in string representation; pressure should have 5.
#define ACCULEVEL_PRESSURE_RESOLUTION 5

/// Decimals places in string representation; temperature should have 2.
#define ACCULEVEL_TEMP_RESOLUTION 2

/// Decimals places in string representation; height should have 4.
#define ACCULEVEL_HEIGHT_RESOLUTION 4


// The main class for the Keller Sensors
class KellerAcculevel : public KellerParent {
 public:
    // Constructors with overloads
    KellerAcculevel(byte modbusAddress, Stream* stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : KellerParent(
              modbusAddress, stream, powerPin, powerPin2, enablePin,
              measurementsToAverage, Acculevel_kellerModel, "KellerAcculevel",
              KELLER_NUM_VARIABLES, ACCULEVEL_WARM_UP_TIME_MS,
              ACCULEVEL_STABILIZATION_TIME_MS, ACCULEVEL_MEASUREMENT_TIME_MS) {}
    KellerAcculevel(byte modbusAddress, Stream& stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : KellerParent(
              modbusAddress, stream, powerPin, powerPin2, enablePin,
              measurementsToAverage, Acculevel_kellerModel, "KellerAcculevel",
              KELLER_NUM_VARIABLES, ACCULEVEL_WARM_UP_TIME_MS,
              ACCULEVEL_STABILIZATION_TIME_MS, ACCULEVEL_MEASUREMENT_TIME_MS) {}
    // Destructor
    ~KellerAcculevel() {}
};


// Defines the PressureGauge (vented & barometricPressure corrected) variable
class KellerAcculevel_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new KellerAcculevel_Pressure object.
     *
     * @param parentSense The parent KellerAcculevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is kellerAccuPress
     */
    explicit KellerAcculevel_Pressure(KellerAcculevel* parentSense,
                                      const char*      uuid = "",
                                      const char* varCode   = "kellerAccuPress")
        : Variable(parentSense, (const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)ACCULEVEL_PRESSURE_RESOLUTION, "pressureGauge",
                   "millibar", varCode, uuid) {}
    /**
     * @brief Construct a new KellerAcculevel_Pressure object.
     *
     * @note This must be tied with a parent KellerAcculevel before it can be
     * used.
     */
    KellerAcculevel_Pressure()
        : Variable((const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)ACCULEVEL_PRESSURE_RESOLUTION, "pressureGauge",
                   "millibar", "kellerAccuPress") {}
    /**
     * @brief Destroy the KellerAcculevel_Pressure object - no action needed.
     */
    ~KellerAcculevel_Pressure() {}
};


// Defines the Temperature Variable
class KellerAcculevel_Temp : public Variable {
 public:
    /**
     * @brief Construct a new KellerAcculevel_Temp object.
     *
     * @param parentSense The parent KellerAcculevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is kellerAccuTemp
     */
    explicit KellerAcculevel_Temp(KellerAcculevel* parentSense,
                                  const char*      uuid    = "",
                                  const char*      varCode = "kellerAccuTemp")
        : Variable(parentSense, (const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)ACCULEVEL_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new KellerAcculevel_Temp object.
     *
     * @note This must be tied with a parent KellerAcculevel before it can be
     * used.
     */
    KellerAcculevel_Temp()
        : Variable((const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)ACCULEVEL_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "kellerAccuTemp") {}
    /**
     * @brief Destroy the KellerAcculevel_Temp object - no action needed.
     */
    ~KellerAcculevel_Temp() {}
};

// Defines the gageHeight (Water level with regard to an arbitrary gage datum)
// Variable
class KellerAcculevel_Height : public Variable {
 public:
    /**
     * @brief Construct a new KellerAcculevel_Height object.
     *
     * @param parentSense The parent KellerAcculevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is kellerAccuHeight
     */
    explicit KellerAcculevel_Height(KellerAcculevel* parentSense,
                                    const char*      uuid = "",
                                    const char* varCode   = "kellerAccuHeight")
        : Variable(parentSense, (const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)ACCULEVEL_HEIGHT_RESOLUTION, "gaugeHeight", "meter",
                   varCode, uuid) {}
    /**
     * @brief Construct a new KellerAcculevel_Height object.
     *
     * @note This must be tied with a parent KellerAcculevel before it can be
     * used.
     */
    KellerAcculevel_Height()
        : Variable((const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)ACCULEVEL_HEIGHT_RESOLUTION, "gaugeHeight", "meter",
                   "kellerAccuHeight") {}
    /**
     * @brief Destroy the KellerAcculevel_Height object - no action needed.
     */
    ~KellerAcculevel_Height() {}
};

#endif  // SRC_SENSORS_KELLERACCULEVEL_H_
