/*
 *YosemitechY532.cpp
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
 *     Resolution is 0.01 pH units
 *     Accuracy is ± 0.1 pH units
 *     Range is 2 to 12 pH units
 *
 * For Temperature:
 *     Resolution is 0.1 °C
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 500ms
 * Time between "StartMeasurement" command and stable reading - 4.5sec
*/

#include "sensors/YosemitechY532.h"

// Constructors with overloads
YosemitechY532::YosemitechY532(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2,
                               int8_t enablePin, uint8_t measurementsToAverage)
  : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Y532, "YosemitechY532", Y532_NUM_VARIABLES,
                    Y532_WARM_UP_TIME_MS, Y532_STABILIZATION_TIME_MS, Y532_MEASUREMENT_TIME_MS)
{}
YosemitechY532::YosemitechY532(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2,
                               int8_t enablePin, uint8_t measurementsToAverage)
  : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Y532, "YosemitechY532", Y532_NUM_VARIABLES,
                    Y532_WARM_UP_TIME_MS, Y532_STABILIZATION_TIME_MS, Y532_MEASUREMENT_TIME_MS)
{}
YosemitechY532::~YosemitechY532(){}


/***
// Defines the pH
class YosemitechY532_pH : public Variable
{
public:
    YosemitechY532_pH(Sensor *parentSense,
                      const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, Y532_PH_VAR_NUM,
                "pH", "pH",
                Y532_PH_RESOLUTION,
                "Y532pH", UUID, customVarCode)
    {}
    ~YosemitechY532_pH(){}
};


// Defines the Temperature Variable
class YosemitechY532_Temp : public Variable
{
public:
    YosemitechY532_Temp(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, Y532_TEMP_VAR_NUM,
                "temperature", "degreeCelsius",
                Y532_TEMP_RESOLUTION,
                "Y532Temp", UUID, customVarCode)
    {}
    ~YosemitechY532_Temp(){}
};


// Defines the Electrode Electrical Potential
class YosemitechY532_Voltage : public Variable
{
public:
    YosemitechY532_Voltage(Sensor *parentSense,
                           const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, Y532_VOLT_VAR_NUM,
                "voltage", "millivolt",
                Y532_VOLT_RESOLUTION,
                "Y532Potential", UUID, customVarCode)
    {}
    ~YosemitechY532_Voltage(){}
};
***/
