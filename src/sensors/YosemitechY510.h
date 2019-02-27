/*
 *YosemitechY510.h
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

// Included Dependencies
#include "VariableBase.h"
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
#define Y510_NUM_VARIABLES 2
#define Y510_WARM_UP_TIME_MS 500
#define Y510_STABILIZATION_TIME_MS 22000
#define Y510_MEASUREMENT_TIME_MS 1700

#define Y510_TURB_RESOLUTION 2
#define Y510_TURB_VAR_NUM 0

#define Y510_TEMP_RESOLUTION 1
#define Y510_TEMP_VAR_NUM 1

// The main class for the Decagon Y510
class YosemitechY510 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY510(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                        Y510, "YosemitechY510", Y510_NUM_VARIABLES,
                        Y510_WARM_UP_TIME_MS, Y510_STABILIZATION_TIME_MS, Y510_MEASUREMENT_TIME_MS)
    {}
    YosemitechY510(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                        Y510, "YosemitechY510", Y510_NUM_VARIABLES,
                        Y510_WARM_UP_TIME_MS, Y510_STABILIZATION_TIME_MS, Y510_MEASUREMENT_TIME_MS)
    {}
    ~YosemitechY510(){}
};


// Defines the Turbidity
class YosemitechY510_Turbidity : public Variable
{
public:
    YosemitechY510_Turbidity()
      : Variable(Y510_TURB_VAR_NUM, Y510_TURB_RESOLUTION,
                 "turbidity", "nephelometricTurbidityUnit", "Y510Turbidity")

    {}
    ~YosemitechY510_Turbidity(){}
};


// Defines the Temperature Variable
class YosemitechY510_Temp : public Variable
{
public:
    YosemitechY510_Temp()
      : Variable(Y510_TEMP_VAR_NUM, Y510_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius", "Y510Temp")

    {}
    ~YosemitechY510_Temp(){}
};

#endif  // Header Guard
