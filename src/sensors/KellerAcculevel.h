/*
 *KellerAcculevel.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Aufdenkampe <aaufdenkampe@limno.com>
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
#ifndef KellerAcculevel_h
#define KellerAcculevel_h

// Included Dependencies
#include "sensors/KellerParent.h"

// Sensor Specific Defines
#define ACCULEVEL_WARM_UP_TIME_MS 500
#define ACCULEVEL_STABILIZATION_TIME_MS 5000
#define ACCULEVEL_MEASUREMENT_TIME_MS 1500

#define ACCULEVEL_PRESSURE_RESOLUTION 5

#define ACCULEVEL_TEMP_RESOLUTION 2

#define ACCULEVEL_HEIGHT_RESOLUTION 4


// The main class for the Keller Sensors
class KellerAcculevel : public KellerParent
{
public:
    // Constructors with overloads
    KellerAcculevel(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2 = -1,
                    int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : KellerParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Acculevel_kellerModel, "KellerAcculevel", KELLER_NUM_VARIABLES,
                    ACCULEVEL_WARM_UP_TIME_MS, ACCULEVEL_STABILIZATION_TIME_MS, ACCULEVEL_MEASUREMENT_TIME_MS)
    {}
    KellerAcculevel(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2 = -1,
                    int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : KellerParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Acculevel_kellerModel, "KellerAcculevel", KELLER_NUM_VARIABLES,
                    ACCULEVEL_WARM_UP_TIME_MS, ACCULEVEL_STABILIZATION_TIME_MS, ACCULEVEL_MEASUREMENT_TIME_MS)
    {}
    // Destructor
    ~KellerAcculevel(){}
};


// Defines the PressureGauge (vented & barometricPressure corrected) variable
class KellerAcculevel_Pressure : public Variable
{
public:
    KellerAcculevel_Pressure(Sensor *parentSense,
                             const char *uuid = "",
                             const char *varCode = "kellerAccuPress")
      : Variable(parentSense,
                 (const uint8_t)KELLER_PRESSURE_VAR_NUM,
                 (uint8_t)ACCULEVEL_PRESSURE_RESOLUTION,
                 "pressureGauge", "millibar",
                 varCode, uuid)
    {}
    KellerAcculevel_Pressure()
      : Variable((const uint8_t)KELLER_PRESSURE_VAR_NUM,
                 (uint8_t)ACCULEVEL_PRESSURE_RESOLUTION,
                 "pressureGauge", "millibar", "kellerAccuPress")
    {}
    ~KellerAcculevel_Pressure(){}
};


// Defines the Temperature Variable
class KellerAcculevel_Temp : public Variable
{
public:
    KellerAcculevel_Temp(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "kellerAccuTemp")
      : Variable(parentSense,
                 (const uint8_t)KELLER_TEMP_VAR_NUM,
                 (uint8_t)ACCULEVEL_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius",
                 varCode, uuid)
    {}
    KellerAcculevel_Temp()
      : Variable((const uint8_t)KELLER_TEMP_VAR_NUM,
                 (uint8_t)ACCULEVEL_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius", "kellerAccuTemp")
    {}
    ~KellerAcculevel_Temp(){}
};

// Defines the gageHeight (Water level with regard to an arbitrary gage datum) Variable
class KellerAcculevel_Height : public Variable
{
public:
    KellerAcculevel_Height(Sensor *parentSense,
                           const char *uuid = "",
                           const char *varCode = "kellerAccuHeight")
      : Variable(parentSense,
                 (const uint8_t)KELLER_HEIGHT_VAR_NUM,
                 (uint8_t)ACCULEVEL_HEIGHT_RESOLUTION,
                 "gaugeHeight", "meter",
                 varCode, uuid)
    {}
    KellerAcculevel_Height()
      : Variable((const uint8_t)KELLER_HEIGHT_VAR_NUM,
                 (uint8_t)ACCULEVEL_HEIGHT_RESOLUTION,
                 "gaugeHeight", "meter", "kellerAccuHeight")
    {}
    ~KellerAcculevel_Height(){}
};

#endif  // Header Guard
