/*
 *YosemitechY504.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Yosemitech Y504 Optical Dissolved Oxyben sensor
 *It is dependent on the YosemitechParent super class.
 *
 *Documentation for the Modbus Protocol commands and responses can be found
 *within the documentation in the YosemitechModbus library at:
 *https://github.com/EnviroDIY/YosemitechModbus
 *
 *These devices output very high "resolution" (32bits) so the resolutions are
 *based on their accuracy, not the resolution of the sensor
 *
 * For Dissolved Oxygen:
 *     Accuracy is 1%
 *     Range is 0-20mg/L or 0-200% Saturation
 *
 * For Temperature:
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 275-300ms (use 350 for safety)
 * Time between "StartMeasurement" command and stable reading - 8sec
*/

#ifndef YosemitechY504_h
#define YosemitechY504_h

#include "YosemitechParent.h"
#include "VariableBase.h"

#define Y504_NUM_VARIABLES 3
#define Y504_WARM_UP_TIME_MS 375
#define Y504_STABILIZATION_TIME_MS 8000
#define Y504_MEASUREMENT_TIME_MS 1700

#define Y504_DOPCT_RESOLUTION 1
#define Y504_DOPCT_VAR_NUM 0

#define Y504_TEMP_RESOLUTION 2
#define Y504_TEMP_VAR_NUM 1

#define Y504_DOMGL_RESOLUTION 2
#define Y504_DOMGL_VAR_NUM 2

// The main class for the Decagon Y504
class YosemitechY504 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY504(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, enablePin, measurementsToAverage,
                        Y504, F("YosemitechY504"), Y504_NUM_VARIABLES,
                        Y504_WARM_UP_TIME_MS, Y504_STABILIZATION_TIME_MS, Y504_MEASUREMENT_TIME_MS)
    {}
    YosemitechY504(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, enablePin, measurementsToAverage,
                        Y504, F("YosemitechY504"), Y504_NUM_VARIABLES,
                        Y504_WARM_UP_TIME_MS, Y504_STABILIZATION_TIME_MS, Y504_MEASUREMENT_TIME_MS)
    {}
};


// Defines the Dissolved Oxygen Percent Saturation
class YosemitechY504_DOpct : public Variable
{
public:
    YosemitechY504_DOpct(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y504_DOPCT_VAR_NUM,
                F("oxygenDissolvedPercentOfSaturation"), F("percent"),
                Y504_DOPCT_RESOLUTION,
                F("Y504DOpct"), UUID, customVarCode)
    {}
};


// Defines the Temperature Variable
class YosemitechY504_Temp : public Variable
{
public:
    YosemitechY504_Temp(Sensor *parentSense,
                        String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y504_TEMP_VAR_NUM,
                F("temperature"), F("degreeCelsius"),
                Y504_TEMP_RESOLUTION,
                F("Y504temp"), UUID, customVarCode)
    {}
};


// Defines the Dissolved Oxygen Concentration
class YosemitechY504_DOmgL : public Variable
{
public:
    YosemitechY504_DOmgL(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y504_DOMGL_VAR_NUM,
                F("oxygenDissolved"), F("milligramPerLiter"),
                Y504_DOMGL_RESOLUTION,
                F("Y504DOmgL"), UUID, customVarCode)
    {}
};

#endif
