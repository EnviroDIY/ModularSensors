/*
 *KellerNanolevel.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Aufdenkampe <aaufdenkampe@limno.com>,updated by Neil Hancock.
 *
 *This file is for Modbus communication to  Keller Series 30, Class 5, Group 20 sensors,
 *that are Software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week)
 *Only tested the Acculevel
 *
 *Documentation for the Keller Protocol commands and responses, along with
 *information about the various variables, can be found
 *in the EnviroDIY KellerModbus library at:
 * https://github.com/EnviroDIY/KellerModbus
*/

// Header Guards
#ifndef KellerNanolevel_h
#define KellerNanolevel_h

// Debugging Statement
// #define MS_KELLERNANOLEVEL_DEBUG
// #define MS_KELLERNANOLEVEL_DEBUG_DEEP

#ifdef MS_KELLERNANOLEVEL_DEBUG
#define MS_DEBUGGING_STD "KellerNanolevel"
#endif

#ifdef MS_KELLERNANOLEVEL_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "KellerNanolevel"
#endif

// Included Dependencies
#include "VariableBase.h"
#include "sensors/KellerParent.h"

// Sensor Specific Defines
#define NANOLEVEL_WARM_UP_TIME_MS 500
#define NANOLEVEL_STABILIZATION_TIME_MS 5000
#define NANOLEVEL_MEASUREMENT_TIME_MS 1500

#define NANOLEVEL_PRESSURE_RESOLUTION 5

#define NANOLEVEL_TEMP_RESOLUTION 2

#define NANOLEVEL_HEIGHT_RESOLUTION 4


// The main class for the Keller Sensors
class KellerNanolevel : public KellerParent
{
public:
    // Constructors with overloads
    KellerNanolevel(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2 = -1,
                    int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : KellerParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Nanolevel_kellerModel, "KellerNanolevel", KELLER_NUM_VARIABLES,
                    NANOLEVEL_WARM_UP_TIME_MS, NANOLEVEL_STABILIZATION_TIME_MS, NANOLEVEL_MEASUREMENT_TIME_MS)
    {}
    KellerNanolevel(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2 = -1,
                    int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : KellerParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Nanolevel_kellerModel, "KellerNanolevel", KELLER_NUM_VARIABLES,
                    NANOLEVEL_WARM_UP_TIME_MS, NANOLEVEL_STABILIZATION_TIME_MS, NANOLEVEL_MEASUREMENT_TIME_MS)
    {}
    // Destructor
    ~KellerNanolevel(){}
};


// Defines the PressureGauge (vented & barometricPressure corrected) variable
class KellerNanolevel_Pressure : public Variable
{
public:
    KellerNanolevel_Pressure(Sensor *parentSense,
                             const char *uuid = "",
                             const char *varCode = "kellerNanoPress")
      : Variable(parentSense,
                 (const uint8_t)KELLER_PRESSURE_VAR_NUM,
                 (uint8_t)NANOLEVEL_PRESSURE_RESOLUTION,
                 "pressureGauge", "millibar",
                 varCode, uuid)
    {}
    KellerNanolevel_Pressure()
      : Variable((const uint8_t)KELLER_PRESSURE_VAR_NUM,
                 (uint8_t)NANOLEVEL_PRESSURE_RESOLUTION,
                 "pressureGauge", "millibar", "kellerNanoPress")
    {}
    ~KellerNanolevel_Pressure(){}
};


// Defines the Temperature Variable
class KellerNanolevel_Temp : public Variable
{
public:
    KellerNanolevel_Temp(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "kellerNanoTemp")
      : Variable(parentSense,
                 (const uint8_t)KELLER_TEMP_VAR_NUM,
                 (uint8_t)NANOLEVEL_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius",
                 varCode, uuid)
    {}
    KellerNanolevel_Temp()
      : Variable((const uint8_t)KELLER_TEMP_VAR_NUM,
                 (uint8_t)NANOLEVEL_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius", "kellerNanoTemp")
    {}
    ~KellerNanolevel_Temp(){}
};

// Defines the gageHeight (Water level with regard to an arbitrary gage datum) Variable
class KellerNanolevel_Height : public Variable
{
public:
    KellerNanolevel_Height(Sensor *parentSense,
                           const char *uuid = "",
                           const char *varCode = "kellerNanoHeight")
      : Variable(parentSense,
                 (const uint8_t)KELLER_HEIGHT_VAR_NUM,
                 (uint8_t)NANOLEVEL_HEIGHT_RESOLUTION,
                 "gaugeHeight", "meter",
                 varCode, uuid)
    {}
    KellerNanolevel_Height()
      : Variable((const uint8_t)KELLER_HEIGHT_VAR_NUM,
                 (uint8_t)NANOLEVEL_HEIGHT_RESOLUTION,
                 "gaugeHeight", "meter", "kellerNanoHeight")
    {}
    ~KellerNanolevel_Height(){}
};

#endif  // Header Guard
