/*
 *YosemitechY511.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Yosemitech Y511 Turbidity Sensor with Wiper
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
#ifndef YosemitechY511_h
#define YosemitechY511_h

// Included Dependencies
#include "VariableBase.h"
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
#define Y511_NUM_VARIABLES 2
#define Y511_WARM_UP_TIME_MS 8000         // 500 ms to receive commands, but if activating brush warmup+stabilization must >20s
#define Y511_STABILIZATION_TIME_MS 40000  // warmup+stabilization > 48 s for consecutive readings to give different results
#define Y511_MEASUREMENT_TIME_MS 4000     // could potentially be lower with a longer stabilization time. More testing needed.

#define Y511_TURB_RESOLUTION 2
#define Y511_TURB_VAR_NUM 0

#define Y511_TEMP_RESOLUTION 1
#define Y511_TEMP_VAR_NUM 1

// The main class for the Decagon Y511
class YosemitechY511 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY511(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                        Y511, "YosemitechY511", Y511_NUM_VARIABLES,
                        Y511_WARM_UP_TIME_MS, Y511_STABILIZATION_TIME_MS, Y511_MEASUREMENT_TIME_MS)
    {}
    YosemitechY511(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                        Y511, "YosemitechY511", Y511_NUM_VARIABLES,
                        Y511_WARM_UP_TIME_MS, Y511_STABILIZATION_TIME_MS, Y511_MEASUREMENT_TIME_MS)
    {}
    ~YosemitechY511(){}
};


// Defines the Turbidity
class YosemitechY511_Turbidity : public Variable
{
public:
    YosemitechY511_Turbidity(Sensor *parentSense, const char *uuid = "",
                             const char *customVarCode = "Y511Turbidity")
      : Variable(parentSense,
                 (const uint8_t)Y511_TURB_VAR_NUM,
                 (uint8_t)Y511_TURB_RESOLUTION,
                 "turbidity", "nephelometricTurbidityUnit",
                 customVarCode, uuid)
    {}
    YosemitechY511_Turbidity()
      : Variable((const uint8_t)Y511_TURB_VAR_NUM,
                 (uint8_t)Y511_TURB_RESOLUTION,
                 "turbidity", "nephelometricTurbidityUnit", "Y511Turbidity")
    {}
    ~YosemitechY511_Turbidity(){}
};


// Defines the Temperature Variable
class YosemitechY511_Temp : public Variable
{
public:
    YosemitechY511_Temp(Sensor *parentSense, const char *uuid = "",
                        const char *customVarCode = "Y511Temp")
      : Variable(parentSense,
                 (const uint8_t)Y511_TEMP_VAR_NUM,
                 (uint8_t)Y511_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius",
                 customVarCode, uuid)
    {}
    YosemitechY511_Temp()
      : Variable((const uint8_t)Y511_TEMP_VAR_NUM,
                 (uint8_t)Y511_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius", "Y511Temp")
    {}
    ~YosemitechY511_Temp(){}
};

#endif  // Header Guard
