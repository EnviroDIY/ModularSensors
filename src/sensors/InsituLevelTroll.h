/*
 *InsituLevelTroll.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Aufdenkampe <aaufdenkampe@limno.com>,updated by Neil Hancock.
 *
 *This file is for Modbus communication to  Insitu Level Troll System Spec 1 and Spec 3,
 *from InSitu Modbus Communication Protocol Version 5.10  ar
 *Only tested on the Insitu LT400
 *
 * It uses the KellerParent as the base ~ which maybe should be called ModbusParent
 *
*/

// Header Guards
#ifndef InsituLevelTroll_h
#define InsituLevelTroll_h

// Included Dependencies
#include "sensors/KellerParent.h"

// Sensor Specific Defines
#define LEVELTROLL_WARM_UP_TIME_MS 500
#define LEVELTROLL_STABILIZATION_TIME_MS 5000
#define LEVELTROLL_MEASUREMENT_TIME_MS 1500

#define LEVELTROLL_PRESSURE_RESOLUTION 5

#define LEVELTROLL_TEMP_RESOLUTION 2

#define LEVELTROLL_HEIGHT_RESOLUTION 4


// The main class for Modbus Keller/Insitu Sensors
class InsituLevelTroll : public KellerParent
{
public:
    // Constructors with overloads
    InsituLevelTroll(byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2 = -1,
                    int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : KellerParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Leveltroll_InsituModel, "InsituLevelTroll", KELLER_NUM_VARIABLES,
                    LEVELTROLL_WARM_UP_TIME_MS, LEVELTROLL_STABILIZATION_TIME_MS, LEVELTROLL_MEASUREMENT_TIME_MS)
    {}
    InsituLevelTroll(byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2 = -1,
                    int8_t enablePin = -1, uint8_t measurementsToAverage = 1)
     : KellerParent(modbusAddress, stream, powerPin, powerPin2, enablePin, measurementsToAverage,
                    Leveltroll_InsituModel, "InsituLevelTroll", KELLER_NUM_VARIABLES,
                    LEVELTROL_WARM_UP_TIME_MS, LEVELTROL_STABILIZATION_TIME_MS, LEVELTROL_MEASUREMENT_TIME_MS)
    {}
    // Destructor
    ~InsituLevelTroll(){}
};


// Defines the PressureGauge (vented & barometricPressure corrected) variable
class InsituLevelTroll_Pressure : public Variable
{
public:
    InsituLevelTroll_Pressure(Sensor *parentSense,
                             const char *uuid = "",
                             const char *varCode = "Insitu LTxPress")
      : Variable(parentSense,
                 (const uint8_t)KELLER_PRESSURE_VAR_NUM,
                 (uint8_t)LEVELTROL_PRESSURE_RESOLUTION,
                 "pressureGauge", "millibar",
                 varCode, uuid)
    {}
    InsituLevelTroll_Pressure()
      : Variable((const uint8_t)KELLER_PRESSURE_VAR_NUM,
                 (uint8_t)LEVELTROL_PRESSURE_RESOLUTION,
                 "pressureGauge", "millibar", "Insitu LTxPress")
    {}
    ~InsituLevelTroll_Pressure(){}
};


// Defines the Temperature Variable
class InsituLevelTroll_Temp : public Variable
{
public:
    InsituLevelTroll_Temp(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "Insitu LTxTemp")
      : Variable(parentSense,
                 (const uint8_t)KELLER_TEMP_VAR_NUM,
                 (uint8_t)LEVELTROL_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius",
                 varCode, uuid)
    {}
    InsituLevelTroll_Temp()
      : Variable((const uint8_t)KELLER_TEMP_VAR_NUM,
                 (uint8_t)LEVELTROL_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius", "Insitu LTxTemp")
    {}
    ~InsituLevelTroll_Temp(){}
};

// Defines the gageHeight (Water level with regard to an arbitrary gage datum) Variable
class InsituLevelTroll_Height : public Variable
{
public:
    InsituLevelTroll_Height(Sensor *parentSense,
                           const char *uuid = "",
                           const char *varCode = "InsituLTxHeight")
      : Variable(parentSense,
                 (const uint8_t)KELLER_HEIGHT_VAR_NUM,
                 (uint8_t)LEVELTROL_HEIGHT_RESOLUTION,
                 "gaugeHeight", "meter",
                 varCode, uuid)
    {}
    InsituLevelTroll_Height()
      : Variable((const uint8_t)KELLER_HEIGHT_VAR_NUM,
                 (uint8_t)LEVELTROL_HEIGHT_RESOLUTION,
                 "gaugeHeight", "meter", "InsituLTxHeight")
    {}
    ~InsituLevelTroll_Height(){}
};

#endif  // Header Guard
