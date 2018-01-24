/*
 *YosemitechY520.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Decagon Devices 5TM Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library and the YosemitechParent super class.
 *
 *Documentation for the Modbus Protocol commands and responses can be found
 *within the documentation in the YosemitechModbus library at:
 *https://github.com/EnviroDIY/YosemitechModbus
 *
 *These devices output very high "resolution" (32bits) so the resolutions are
 *based on their accuracy, not the resolution of the sensor
 *
 * For Conductivity:
 *     Accuracy is 1%
 *     Range is 1uS/cm~200 mS/cm
 *
 * For Temperature:
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 275-300ms (use 350 for safety)
 * Time between "StartMeasurement" command and stable reading - 8sec
*/

#ifndef YosemitechY520_h
#define YosemitechY520_h

#include "YosemitechParent.h"
#include "VariableBase.h"

#define Y520_NUM_MEASUREMENTS 2
#define Y520_WARM_UP 1200
#define Y520_STABILIZATION 10000
#define Y520_REMEASUREMENT 2700

#define Y520_COND_RESOLUTION 1
#define Y520_COND_VAR_NUM 0

#define Y520_TEMP_RESOLUTION 2
#define Y520_TEMP_VAR_NUM 1

// The main class for the Decagon Y520
class YosemitechY520 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY520(byte modbusAddress, int powerPin, Stream* stream,
                   int enablePin = -1, int numReadings = 1)
     : YosemitechParent(modbusAddress, powerPin, stream, enablePin, numReadings,
                        F("YosemitechY520"), Y520_NUM_MEASUREMENTS,
                        Y520, Y520_WARM_UP, Y520_STABILIZATION, Y520_REMEASUREMENT)
    {}
    YosemitechY520(byte modbusAddress, int powerPin, Stream& stream,
                   int enablePin = -1, int numReadings = 1)
     : YosemitechParent(modbusAddress, powerPin, stream, enablePin, numReadings,
                        F("YosemitechY520"), Y520_NUM_MEASUREMENTS,
                        Y520, Y520_WARM_UP, Y520_STABILIZATION, Y520_REMEASUREMENT)
    {}
};


// Defines the Conductivity
class YosemitechY520_Cond : public Variable
{
public:
    YosemitechY520_Cond(Sensor *parentSense, String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y520_COND_VAR_NUM,
                F("specificConductance"), F("microsiemenPerCentimeter"),
                Y520_COND_RESOLUTION,
                F("Y520Cond"), UUID, customVarCode)
    {}
};


// Defines the Temperature Variable
class YosemitechY520_Temp : public Variable
{
public:
    YosemitechY520_Temp(Sensor *parentSense, String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y520_TEMP_VAR_NUM,
                F("temperature"), F("degreeCelsius"),
                Y520_TEMP_RESOLUTION,
                F("Y520temp"), UUID, customVarCode)
    {}
};

#endif
