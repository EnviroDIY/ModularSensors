/*
 *YosemitechY510.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Yosemitech Y510 Turbidity Sensor
 *It is dependent on the YosemitechParent super class.
 *
 *Documentation for the Modbus Protocol commands and responses can be found
 *within the documentation in the YosemitechModbus library at:
 *https://github.com/EnviroDIY/YosemitechModbus
 *
 *These devices output very high "resolution" (32bits) so the resolutions are
 *based on their accuracy, not the resolution of the sensor
 *
 * For Turbidity:
 *     Accuracy is ± 5 % or 0.3 NTU
 *     Range is 0.1 to 1000 NTU
 *
 * For Temperature:
 *     Resolution is 0.1 °C
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 500ms
 * Time between "StartMeasurement" command and stable reading - 22sec
*/

// Header Guards
#ifndef YosemitechY510_h
#define YosemitechY510_h

#include "sensors/YosemitechY510.h"

// Constructors with overloads
YosemitechY510::YosemitechY510(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2,
                               int8_t enablePin, uint8_t measurementsToAverage)
  : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Y510, "YosemitechY510", Y510_NUM_VARIABLES,
                    Y510_WARM_UP_TIME_MS, Y510_STABILIZATION_TIME_MS, Y510_MEASUREMENT_TIME_MS)
{}
YosemitechY510::YosemitechY510(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2,
                               int8_t enablePin, uint8_t measurementsToAverage)
  : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Y510, "YosemitechY510", Y510_NUM_VARIABLES,
                    Y510_WARM_UP_TIME_MS, Y510_STABILIZATION_TIME_MS, Y510_MEASUREMENT_TIME_MS)
{}
YosemitechY510::~YosemitechY510(){}

/***
// Defines the Turbidity
class YosemitechY510_Turbidity : public Variable
{
public:
    YosemitechY510_Turbidity(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, Y510_TURB_VAR_NUM,
                "turbidity", "nephelometricTurbidityUnit",
                Y510_TURB_RESOLUTION,
                "Y510Turbidity", UUID, customVarCode)
    {}
    ~YosemitechY510_Turbidity(){}
};


// Defines the Temperature Variable
class YosemitechY510_Temp : public Variable
{
public:
    YosemitechY510_Temp(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, Y510_TEMP_VAR_NUM,
                "temperature", "degreeCelsius",
                Y510_TEMP_RESOLUTION,
                "Y510Temp", UUID, customVarCode)
    {}
    ~YosemitechY510_Temp(){}
};
***/
