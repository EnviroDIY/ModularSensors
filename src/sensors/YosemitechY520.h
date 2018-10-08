/*
 *YosemitechY520.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Yosemitech Y520 4-pole Conductivity sensor
 *It is dependent on the YosemitechParent super class.
 *
 *Documentation for the Modbus Protocol commands and responses can be found
 *within the documentation in the YosemitechModbus library at:
 *https://github.com/EnviroDIY/YosemitechModbus
 *
 *These devices output very high "resolution" (32bits) so the resolutions are
 *based on their accuracy, not the resolution of the sensor
 *
 * For Conductivity:
 *     Resolution is 0.1 µS/cm
 *     Accuracy is ± 1 % Full Scale
 *     Range is 1 µS/cm to 200 mS/cm
 *
 * For Temperature:
 *     Resolution is 0.1 °C
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 1200 ms
 * Time between "StartMeasurement" command and stable reading - 10sec
*/

// Header Guards
#ifndef YosemitechY520_h
#define YosemitechY520_h

// Included Dependencies
#include "VariableBase.h"
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
#define Y504_NUM_VARIABLES 3
#define Y504_WARM_UP_TIME_MS 375
#define Y504_STABILIZATION_TIME_MS 8000
#define Y504_MEASUREMENT_TIME_MS 1700

#define Y520_COND_RESOLUTION 1
#define Y520_COND_VAR_NUM 0

#define Y520_TEMP_RESOLUTION 1
#define Y520_TEMP_VAR_NUM 1

// The main class for the Decagon Y520
class YosemitechY520 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY520(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                        Y520, "YosemitechY520", Y520_NUM_VARIABLES,
                        Y520_WARM_UP_TIME_MS, Y520_STABILIZATION_TIME_MS, Y520_MEASUREMENT_TIME_MS)
    {}
    YosemitechY520(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                        Y520, "YosemitechY520", Y520_NUM_VARIABLES,
                        Y520_WARM_UP_TIME_MS, Y520_STABILIZATION_TIME_MS, Y520_MEASUREMENT_TIME_MS)
    {}
    ~YosemitechY520(){}
};


// Defines the Conductivity
class YosemitechY520_Cond : public Variable
{
public:
    YosemitechY520_Cond(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, Y520_COND_VAR_NUM,
                "specificConductance", "microsiemenPerCentimeter",
                Y520_COND_RESOLUTION,
                "Y520Cond", UUID, customVarCode)
    {}
    ~YosemitechY520_Cond(){}
};


// Defines the Temperature Variable
class YosemitechY520_Temp : public Variable
{
public:
    YosemitechY520_Temp(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, Y520_TEMP_VAR_NUM,
                "temperature", "degreeCelsius",
                Y520_TEMP_RESOLUTION,
                "Y520Temp", UUID, customVarCode)
    {}
    ~YosemitechY520_Temp(){}
};

#endif  // Header Guard
