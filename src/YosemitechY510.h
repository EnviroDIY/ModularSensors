/*
 *YosemitechY510.h
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
 * For Turbidity:
 *     Accuracy is ＜5% or 0.3NTU
 *     Range is 0.1~1000 NTU
 *
 * For Temperature:
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 275-300ms (use 350 for safety)
 * Time between "StartMeasurement" command and stable reading - 8sec
*/

#ifndef YosemitechY510_h
#define YosemitechY510_h

#include "YosemitechParent.h"
#include "VariableBase.h"

#define Y510_NUM_MEASUREMENTS 2
#define Y510_WARM_UP 500
#define Y510_STABILIZATION 22000
#define Y510_REMEASUREMENT 1700

#define Y510_TURB_RESOLUTION 2
#define Y510_TURB_VAR_NUM 0

#define Y510_TEMP_RESOLUTION 2
#define Y510_TEMP_VAR_NUM 1

// The main class for the Decagon Y510
class YosemitechY510 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY510(byte modbusAddress, int powerPin, Stream* stream,
                   int enablePin = -1, int numReadings = 1)
     : YosemitechParent(modbusAddress, powerPin, stream, enablePin, numReadings,
                        F("YosemitechY510"), Y510_NUM_MEASUREMENTS,
                        Y510, Y510_WARM_UP, Y510_STABILIZATION, Y510_REMEASUREMENT)
    {}
    YosemitechY510(byte modbusAddress, int powerPin, Stream& stream,
                   int enablePin = -1, int numReadings = 1)
     : YosemitechParent(modbusAddress, powerPin, stream, enablePin, numReadings,
                        F("YosemitechY510"), Y510_NUM_MEASUREMENTS,
                        Y510, Y510_WARM_UP, Y510_STABILIZATION, Y510_REMEASUREMENT)
    {}
};


// Defines the Turbidity
class YosemitechY510_Turbidity : public Variable
{
public:
    YosemitechY510_Turbidity(Sensor *parentSense, String customVarCode = "")
     : Variable(parentSense, Y510_TURB_VAR_NUM,
                F("turbidity"), F("nephelometricTurbidityUnit"),
                Y510_TURB_RESOLUTION,
                F("Y510Turbidity"), customVarCode)
    {}
};


// Defines the Temperature Variable
class YosemitechY510_Temp : public Variable
{
public:
    YosemitechY510_Temp(Sensor *parentSense, String customVarCode = "")
     : Variable(parentSense, Y510_TEMP_VAR_NUM,
                F("temperature"), F("degreeCelsius"),
                Y510_TEMP_RESOLUTION,
                F("Y510temp"), customVarCode)
    {}
};

#endif
