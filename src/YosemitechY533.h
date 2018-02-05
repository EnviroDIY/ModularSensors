/*
 *YosemitechY533.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Yosemitech Y533 ORP sensor
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

#ifndef YosemitechY533_h
#define YosemitechY533_h

#include "YosemitechParent.h"
#include "VariableBase.h"

#define Y533_NUM_VARIABLES 3
#define Y533_WARM_UP 500
#define Y533_STABILITY 4500
#define Y533_RESAMPLE 1800

#define Y533_PH_RESOLUTION 1
#define Y533_PH_VAR_NUM 0

#define Y533_TEMP_RESOLUTION 2
#define Y533_TEMP_VAR_NUM 1

#define Y533_VOLT_RESOLUTION 2
#define Y533_VOLT_VAR_NUM 2

// The main class for the Decagon Y533
class YosemitechY533 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY533(byte modbusAddress, Stream* stream, int powerPin,
                   int enablePin = -1, int readingsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, enablePin, readingsToAverage,
                        Y533, F("YosemitechY533"), Y533_NUM_VARIABLES,
                        Y533_WARM_UP, Y533_STABILITY, Y533_RESAMPLE)
    {}
    YosemitechY533(byte modbusAddress, Stream& stream, int powerPin,
                   int enablePin = -1, int readingsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, enablePin, readingsToAverage,
                        Y533, F("YosemitechY533"), Y533_NUM_VARIABLES,
                        Y533_WARM_UP, Y533_STABILITY, Y533_RESAMPLE)
    {}
};


// Defines the pH
class YosemitechY533_pH : public Variable
{
public:
    YosemitechY533_pH(Sensor *parentSense,
                      String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y533_PH_VAR_NUM,
                F("pH"), F("pH"),
                Y533_PH_RESOLUTION,
                F("Y533pH"), UUID, customVarCode)
    {}
};


// Defines the Temperature Variable
class YosemitechY533_Temp : public Variable
{
public:
    YosemitechY533_Temp(Sensor *parentSense,
                        String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y533_TEMP_VAR_NUM,
                F("temperature"), F("degreeCelsius"),
                Y533_TEMP_RESOLUTION,
                F("Y533temp"), UUID, customVarCode)
    {}
};


// Defines the Electrode Electrical Potential
class YosemitechY533_Voltage : public Variable
{
public:
    YosemitechY533_Voltage(Sensor *parentSense,
                           String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y533_VOLT_VAR_NUM,
                F("voltage"), F("millivolt"),
                Y533_VOLT_RESOLUTION,
                F("Y533Potential"), UUID, customVarCode)
    {}
};

#endif
