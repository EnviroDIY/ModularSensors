/*
 *YosemitechY514.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Yosemitech Y514 Chlorophyll Sensor with Wiper
 *It is dependent on the YosemitechParent super class.
 *
 *Documentation for the Modbus Protocol commands and responses can be found
 *within the documentation in the YosemitechModbus library at:
 *https://github.com/EnviroDIY/YosemitechModbus
 *
 *These devices output very high "resolution" (32bits) so the resolutions are
 *based on their accuracy, not the resolution of the sensor
 *
 * For Chlorophyll:
 *     Accuracy is 1%
 *     Range is 0~400ug/L(Chl) or 0~100RFU
 *     Resolution is 0.1ug/L Chl
 *
 * For Temperature:
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 1.2 seconds
 * Time between "StartMeasurement" command and stable reading - 8sec
*/

#ifndef YosemitechY514_h
#define YosemitechY514_h

#include "YosemitechParent.h"
#include "VariableBase.h"

#define Y514_NUM_VARIABLES 2
#define Y514_WARM_UP_TIME_MS 1300
#define Y514_STABILIZATION_TIME_MS 8000
#define Y514_MEASUREMENT_TIME_MS 2000

#define Y514_CHLORO_RESOLUTION 1
#define Y514_CHLORO_VAR_NUM 0

#define Y514_TEMP_RESOLUTION 2
#define Y514_TEMP_VAR_NUM 1

// The main class for the Decagon Y514
class YosemitechY514 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY514(byte modbusAddress, Stream* stream, int powerPin,
                   int enablePin = -1, int measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, enablePin, measurementsToAverage,
                        Y514, F("YosemitechY514"), Y514_NUM_VARIABLES,
                        Y514_WARM_UP_TIME_MS, Y514_STABILIZATION_TIME_MS, Y514_MEASUREMENT_TIME_MS)
    {}
    YosemitechY514(byte modbusAddress, Stream& stream, int powerPin,
                   int enablePin = -1, int measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, enablePin, measurementsToAverage,
                        Y514, ("YosemitechY514"), Y514_NUM_VARIABLES,
                        Y514_WARM_UP_TIME_MS, Y514_STABILIZATION_TIME_MS, Y514_MEASUREMENT_TIME_MS)
    {}
};


// Defines the Chlorophyll Concentration
class YosemitechY514_Chlorophyll : public Variable
{
public:
    YosemitechY514_Chlorophyll(Sensor *parentSense,
                               String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y514_CHLORO_VAR_NUM,
                F("chlorophyllFluorescence"), F("microgramPerLiter"),
                Y514_CHLORO_RESOLUTION,
                F("Y514Chloro"), UUID, customVarCode)
    {}
};


// Defines the Temperature Variable
class YosemitechY514_Temp : public Variable
{
public:
    YosemitechY514_Temp(Sensor *parentSense,
                        String UUID = "", String customVarCode = "")
     : Variable(parentSense, Y514_TEMP_VAR_NUM,
                F("temperature"), F("degreeCelsius"),
                Y514_TEMP_RESOLUTION,
                F("Y514temp"), UUID, customVarCode)
    {}
};
#endif
