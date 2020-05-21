/**
 * @file KellerNanolevel.h
 * @brief This file contains the KellerNanolevel sensor subclass and the
 * KellerNanolevel_Pressure, KellerNanolevel_Temp, and KellerNanolevel_Height
 * variable subclasses.  These are for Keller Series 30, Class 5, Group 20
 * sensors using Modbus communication, that are Software version 5.20-12.28 and
 * later (i.e. made after the 2012 in the 28th week).
 *
 * Documentation for the Keller Protocol commands and responses, along with
 * information about the various variables, can be found in the EnviroDIY
 * KellerModbus library at: https://github.com/EnviroDIY/KellerModbus
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com> and Neil
 * Hancock Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 */

// Header Guards
#ifndef SRC_SENSORS_KELLERNANOLEVEL_H_
#define SRC_SENSORS_KELLERNANOLEVEL_H_

// Included Dependencies
#include "sensors/KellerParent.h"

// Sensor Specific Defines
#define NANOLEVEL_WARM_UP_TIME_MS 500
#define NANOLEVEL_STABILIZATION_TIME_MS 5000
#define NANOLEVEL_MEASUREMENT_TIME_MS 1500

#define NANOLEVEL_PRESSURE_RESOLUTION 5

#define NANOLEVEL_TEMP_RESOLUTION 2

#define NANOLEVEL_HEIGHT_RESOLUTION 4


// The main class for the Keller Sensors
class KellerNanolevel : public KellerParent {
 public:
    // Constructors with overloads
    KellerNanolevel(byte modbusAddress, Stream* stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : KellerParent(
              modbusAddress, stream, powerPin, powerPin2, enablePin,
              measurementsToAverage, Nanolevel_kellerModel, "KellerNanolevel",
              KELLER_NUM_VARIABLES, NANOLEVEL_WARM_UP_TIME_MS,
              NANOLEVEL_STABILIZATION_TIME_MS, NANOLEVEL_MEASUREMENT_TIME_MS) {}
    KellerNanolevel(byte modbusAddress, Stream& stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : KellerParent(
              modbusAddress, stream, powerPin, powerPin2, enablePin,
              measurementsToAverage, Nanolevel_kellerModel, "KellerNanolevel",
              KELLER_NUM_VARIABLES, NANOLEVEL_WARM_UP_TIME_MS,
              NANOLEVEL_STABILIZATION_TIME_MS, NANOLEVEL_MEASUREMENT_TIME_MS) {}
    // Destructor
    ~KellerNanolevel() {}
};


// Defines the PressureGauge (vented & barometricPressure corrected) variable
class KellerNanolevel_Pressure : public Variable {
 public:
    explicit KellerNanolevel_Pressure(KellerNanolevel* parentSense,
                                      const char*      uuid = "",
                                      const char* varCode   = "kellerNanoPress")
        : Variable(parentSense, (const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)NANOLEVEL_PRESSURE_RESOLUTION, "pressureGauge",
                   "millibar", varCode, uuid) {}
    KellerNanolevel_Pressure()
        : Variable((const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)NANOLEVEL_PRESSURE_RESOLUTION, "pressureGauge",
                   "millibar", "kellerNanoPress") {}
    ~KellerNanolevel_Pressure() {}
};


// Defines the Temperature Variable
class KellerNanolevel_Temp : public Variable {
 public:
    explicit KellerNanolevel_Temp(KellerNanolevel* parentSense,
                                  const char*      uuid    = "",
                                  const char*      varCode = "kellerNanoTemp")
        : Variable(parentSense, (const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)NANOLEVEL_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    KellerNanolevel_Temp()
        : Variable((const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)NANOLEVEL_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "kellerNanoTemp") {}
    ~KellerNanolevel_Temp() {}
};

// Defines the gageHeight (Water level with regard to an arbitrary gage datum)
// Variable
class KellerNanolevel_Height : public Variable {
 public:
    explicit KellerNanolevel_Height(KellerNanolevel* parentSense,
                                    const char*      uuid = "",
                                    const char* varCode   = "kellerNanoHeight")
        : Variable(parentSense, (const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)NANOLEVEL_HEIGHT_RESOLUTION, "gaugeHeight", "meter",
                   varCode, uuid) {}
    KellerNanolevel_Height()
        : Variable((const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)NANOLEVEL_HEIGHT_RESOLUTION, "gaugeHeight", "meter",
                   "kellerNanoHeight") {}
    ~KellerNanolevel_Height() {}
};

#endif  // SRC_SENSORS_KELLERNANOLEVEL_H_
