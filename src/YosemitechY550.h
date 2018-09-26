/*
 *YosemitechY550.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Yosemitech Y550 COD Sensor with Wiper
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
 *     Accuracy is ＜5% or 0.3NTU
 *     Range is 0.1~1000 NTU
 *
 * For Temperature:
 *     Accuracy is ± 0.2°C
 *     Range is 5°C to + 45°C
 *
 * For COD:
 *     Accuracy is ± ???
 *     Range is 0.75 to 370 mg/L COD (equiv. KHP) 0.2 - 150 mg/L TOC (equiv. KHP)
 *     Resolution is 0.01 mg/L COD
 *
 * Time before sensor responds after power - 500ms
 * Time between "StartMeasurement" command and stable reading - 22sec
*/

#ifndef YosemitechY550_h
#define YosemitechY550_h

#include "YosemitechParent.h"
#include "VariableBase.h"

#define Y550_NUM_VARIABLES 2
#define Y550_WARM_UP_TIME_MS 1500
#define Y550_STABILIZATION_TIME_MS 2000
#define Y550_MEASUREMENT_TIME_MS 2000

#define Y550_COD_RESOLUTION 2
#define Y550_COD_VAR_NUM 0

#define Y550_TEMP_RESOLUTION 2
#define Y550_TEMP_VAR_NUM 1

#define Y550_TURB_RESOLUTION 2
#define Y550_TURB_VAR_NUM 2

// The main class for the Decagon Y550
class YosemitechY550 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY550(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                        Y550, "YosemitechY550", Y550_NUM_VARIABLES,
                        Y550_WARM_UP_TIME_MS, Y550_STABILIZATION_TIME_MS, Y550_MEASUREMENT_TIME_MS)
    {}
    YosemitechY550(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                        Y550, "YosemitechY550", Y550_NUM_VARIABLES,
                        Y550_WARM_UP_TIME_MS, Y550_STABILIZATION_TIME_MS, Y550_MEASUREMENT_TIME_MS)
    {}
};


// Defines the Turbidity
class YosemitechY550_COD : public Variable
{
public:
    YosemitechY550_COD(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, Y550_COD_VAR_NUM,
                "COD", "milligramPerLiter",
                Y550_COD_RESOLUTION,
                "Y550COD", UUID, customVarCode)
    {}
};


// Defines the Temperature Variable
class YosemitechY550_Temp : public Variable
{
public:
    YosemitechY550_Temp(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, Y550_TEMP_VAR_NUM,
                "temperature", "degreeCelsius",
                Y550_TEMP_RESOLUTION,
                "Y550temp", UUID, customVarCode)
    {}
};


// Defines the Turbidity
class YosemitechY550_Turbidity : public Variable
{
public:
    YosemitechY550_Turbidity(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, Y550_TURB_VAR_NUM,
                "turbidity", "nephelometricTurbidityUnit",
                Y550_TURB_RESOLUTION,
                "Y550Turbidity", UUID, customVarCode)
    {}
};

#endif
