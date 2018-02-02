/*
 *YosemitechY532.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Yosemitech Y532 pH sensor
 *It is dependent on the YosemitechParent super class.
 *
 *Documentation for the Modbus Protocol commands and responses can be found
 *within the documentation in the YosemitechModbus library at:
 *https://github.com/EnviroDIY/YosemitechModbus
 *
 *These devices output very high "resolution" (32bits) so the resolutions are
 *based on their accuracy, not the resolution of the sensor
 *
 * For pH:
 *     Accuracy is ±0.1 pH
 *     Range is 2-12 pH
 *
 * For Temperature:
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 500ms
 * Time between "StartMeasurement" command and stable reading - 4.5sec
*/

#ifndef YosemitechY532_h
#define YosemitechY532_h

#include "YosemitechParent.h"
#include "VariableBase.h"

#define Y532_NUM_MEASUREMENTS 3
#define Y532_WARM_UP 500
#define Y532_STABILIZATION 4500
#define Y532_REMEASUREMENT 1800

#define Y532_PH_RESOLUTION 1
#define Y532_PH_VAR_NUM 0

#define Y532_TEMP_RESOLUTION 2
#define Y532_TEMP_VAR_NUM 1

#define Y532_VOLT_RESOLUTION 2
#define Y532_VOLT_VAR_NUM 2

// The main class for the Decagon Y532
class YosemitechY532 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY532(byte modbusAddress, int powerPin, Stream* stream,
                   int enablePin = -1, int numReadings = 1)
     : YosemitechParent(modbusAddress, powerPin, stream, enablePin, numReadings,
                        F("YosemitechY532"), Y532_NUM_MEASUREMENTS,
                        Y532, Y532_WARM_UP, Y532_STABILIZATION, Y532_REMEASUREMENT)
    {}
    YosemitechY532(byte modbusAddress, int powerPin, Stream& stream,
                   int enablePin = -1, int numReadings = 1)
     : YosemitechParent(modbusAddress, powerPin, stream, enablePin, numReadings,
                        F("YosemitechY532"), Y532_NUM_MEASUREMENTS,
                        Y532, Y532_WARM_UP, Y532_STABILIZATION, Y532_REMEASUREMENT)
    {}
};


// Defines the pH
class YosemitechY532_pH : public Variable
{
public:
    YosemitechY532_pH(Sensor *parentSense,
                      String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y532_PH_VAR_NUM,
                F("pH"), F("pH"),
                Y532_PH_RESOLUTION,
                F("Y532pH"), UUID, customVarCode)
    {}
};


// Defines the Temperature Variable
class YosemitechY532_Temp : public Variable
{
public:
    YosemitechY532_Temp(Sensor *parentSense,
                        String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y532_TEMP_VAR_NUM,
                F("temperature"), F("degreeCelsius"),
                Y532_TEMP_RESOLUTION,
                F("Y532temp"), UUID, customVarCode)
    {}
};


// Defines the Electrode Electrical Potential
class YosemitechY532_Voltage : public Variable
{
public:
    YosemitechY532_Voltage(Sensor *parentSense,
                           String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y532_VOLT_VAR_NUM,
                F("voltage"), F("millivolt"),
                Y532_VOLT_RESOLUTION,
                F("Y532Voltage"), UUID, customVarCode)
    {}
};

#endif
