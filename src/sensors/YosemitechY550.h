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

// Header Guards
#ifndef YosemitechY550_h
#define YosemitechY550_h

// Debugging Statement
// #define MS_YOSEMITECHY550_DEBUG
// #define MS_YOSEMITECHY550_DEBUG_DEEP

#ifdef MS_YOSEMITECHY550_DEBUG
#define MS_DEBUGGING_STD
#endif

#ifdef MS_YOSEMITECHY550_DEBUG_DEEP
#define MS_DEBUGGING_DEEP
#endif

// Included Dependencies
#include "VariableBase.h"
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
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
    ~YosemitechY550(){}
};


// Defines the Carbon Oxygen Demand
class YosemitechY550_COD : public Variable
{
public:
    YosemitechY550_COD(Sensor *parentSense,
                       const char *uuid = "",
                       const char *varCode = "Y550COD")
      : Variable(parentSense,
                 (const uint8_t)Y550_COD_VAR_NUM,
                 (uint8_t)Y550_COD_RESOLUTION,
                 "COD", "milligramPerLiter",
                 varCode, uuid)
    {}
    YosemitechY550_COD()
      : Variable((const uint8_t)Y550_COD_VAR_NUM,
                 (uint8_t)Y550_COD_RESOLUTION,
                 "COD", "milligramPerLiter", "Y550COD")
    {}
    ~YosemitechY550_COD(){}
};


// Defines the Temperature Variable
class YosemitechY550_Temp : public Variable
{
public:
    YosemitechY550_Temp(Sensor *parentSense,
                        const char *uuid = "",
                        const char *varCode = "Y550Temp")
      : Variable(parentSense,
                 (const uint8_t)Y550_TEMP_VAR_NUM,
                 (uint8_t)Y550_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius",
                 varCode, uuid)
    {}
    YosemitechY550_Temp()
      : Variable((const uint8_t)Y550_TEMP_VAR_NUM,
                 (uint8_t)Y550_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius", "Y550Temp")
    {}
    ~YosemitechY550_Temp(){}
};


// Defines the Turbidity
class YosemitechY550_Turbidity : public Variable
{
public:
    YosemitechY550_Turbidity(Sensor *parentSense,
                             const char *uuid = "",
                             const char *varCode = "Y550Turbidity")
      : Variable(parentSense,
                 (const uint8_t)Y550_TURB_VAR_NUM,
                 (uint8_t)Y550_TURB_RESOLUTION,
                 "turbidity", "nephelometricTurbidityUnit",
                 varCode, uuid)
    {}
    YosemitechY550_Turbidity()
      : Variable((const uint8_t)Y550_TURB_VAR_NUM,
                 (uint8_t)Y550_TURB_RESOLUTION,
                 "turbidity", "nephelometricTurbidityUnit", "Y550Turbidity")
    {}
    ~YosemitechY550_Turbidity(){}
};

#endif  // Header Guard
