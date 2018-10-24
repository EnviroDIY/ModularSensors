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
 *Documentation for the Yosemitech Protocol commands and responses, along with
 *information about the various variables, can be found
 *in the EnviroDIY KellerModbus library at:
 * https://github.com/EnviroDIY/KellerModbus
*/

// Header Guards
#ifndef KellerAcculevel_h
#define KellerAcculevel_h

// Included Dependencies
#include "VariableBase.h"
#include "sensors/KellerParent.h"

// Sensor Specific Defines
#define KellerAcculevel_WARM_UP_TIME_MS 500
#define KellerAcculevel_STABILIZATION_TIME_MS 5000
#define KellerAcculevel_MEASUREMENT_TIME_MS 1500

#define KellerAcculevel_PRESSURE_RESOLUTION 5

#define KellerAcculevel_TEMP_RESOLUTION 2

#define KellerAcculevel_HEIGHT_RESOLUTION 4


// The main class for the Keller Sensors
class KellerAcculevel : public KellerParent
{
public:
    // Constructors with overloads
    KellerAcculevel(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : KellerParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Acculevel, "KellerAcculevel", KELLER_NUM_VARIABLES,
                    KellerAcculevel_WARM_UP_TIME_MS, KellerAcculevel_STABILIZATION_TIME_MS, KellerAcculevel_MEASUREMENT_TIME_MS)
    {}
    KellerAcculevel(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : KellerParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Acculevel, "KellerAcculevel", KELLER_NUM_VARIABLES,
                    KellerAcculevel_WARM_UP_TIME_MS, KellerAcculevel_STABILIZATION_TIME_MS, KellerAcculevel_MEASUREMENT_TIME_MS)
    {}
    // Destructor
    ~KellerAcculevel(){}
};


// Defines the PressureGauge (vented & barometricPressure corrected) variable
class KellerAcculevel_Pressure : public Variable
{
public:
    KellerAcculevel_Pressure(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, KELLER_PRESSURE_VAR_NUM,
                "pressureGauge", "millibar",
                KellerAcculevel_PRESSURE_RESOLUTION,
                "kellerPress", UUID, customVarCode)
    {}
    ~KellerAcculevel_Pressure(){}
};


// Defines the Temperature Variable
class KellerAcculevel_Temp : public Variable
{
public:
    KellerAcculevel_Temp(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, KELLER_TEMP_VAR_NUM,
                "temperature", "degreeCelsius",
                KellerAcculevel_TEMP_RESOLUTION,
                "kellerTemp", UUID, customVarCode)
    {}
    ~KellerAcculevel_Temp(){}
};

// Defines the gageHeight (Water level with regard to an arbitrary gage datum) Variable
class KellerAcculevel_Height : public Variable
{
public:
    KellerAcculevel_Height(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, KELLER_HEIGHT_VAR_NUM,
                "gaugeHeight", "meter",
                KellerAcculevel_HEIGHT_RESOLUTION,
                "kellerHeight", UUID, customVarCode)
    {}
    ~KellerAcculevel_Height(){}
};

#endif  // Header Guard