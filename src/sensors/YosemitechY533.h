/*
 *YosemitechY533.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Yosemitech Y533 ORP sensor
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
 *     Accuracy is ±0.1 pH
 *     Range is 2-12 pH
 *
 * For Temperature:
 *     Resolution is 0.1 °C
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * For ORP/Voltage:
 *     Resolution is 1 mV
 *     Accuracy is ± 20 mV
 *     Range is -999 ~ 999 mV
 *
 * Time before sensor responds after power - 500ms
 * Time between "StartMeasurement" command and stable reading - 4.5sec
*/

// Header Guards
#ifndef YosemitechY533_h
#define YosemitechY533_h

// Included Dependencies
#include "VariableBase.h"
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
#define Y533_NUM_VARIABLES 3
#define Y533_WARM_UP_TIME_MS 500
#define Y533_STABILIZATION_TIME_MS 4500
#define Y533_MEASUREMENT_TIME_MS 1800

#define Y533_PH_RESOLUTION 2
#define Y533_PH_VAR_NUM 0

#define Y533_TEMP_RESOLUTION 1
#define Y533_TEMP_VAR_NUM 1

#define Y533_VOLT_RESOLUTION 0
#define Y533_VOLT_VAR_NUM 2

// The main class for the Decagon Y533
class YosemitechY533 : public YosemitechParent
{
public:
    // Constructors with overloads
    YosemitechY533(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                        Y533, "YosemitechY533", Y533_NUM_VARIABLES,
                        Y533_WARM_UP_TIME_MS, Y533_STABILIZATION_TIME_MS, Y533_MEASUREMENT_TIME_MS)
    {}
    YosemitechY533(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2 = -1,
                   int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : YosemitechParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                        Y533, "YosemitechY533", Y533_NUM_VARIABLES,
                        Y533_WARM_UP_TIME_MS, Y533_STABILIZATION_TIME_MS, Y533_MEASUREMENT_TIME_MS)
    {}
    ~YosemitechY533(){}
};


// Defines the pH Variable
class YosemitechY533_pH : public Variable
{
public:
    YosemitechY533_pH()
      : Variable(Y533_PH_VAR_NUM, Y533_PH_RESOLUTION,
                 "pH", "pH", "Y533pH")

    {}
    ~YosemitechY533_pH(){}
};


// Defines the Temperature Variable
class YosemitechY533_Temp : public Variable
{
public:
    YosemitechY533_Temp()
      : Variable(Y533_TEMP_VAR_NUM, Y533_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius", "Y533Temp")

    {}
    ~YosemitechY533_Temp(){}
};


// Defines the Electrode Electrical Potential
class YosemitechY533_Voltage : public Variable
{
public:
    YosemitechY533_Voltage()
      : Variable(Y533_VOLT_VAR_NUM, Y533_VOLT_RESOLUTION,
                 "voltage", "millivolt", "Y533Potential")

    {}
    ~YosemitechY533_Voltage(){}
};

// Undefine debugging macro, if applicable, so we don't have macro "leaks"
#ifdef DEBUGGING_SERIAL_OUTPUT
#undef DEBUGGING_SERIAL_OUTPUT
#endif

#endif  // Header Guard
