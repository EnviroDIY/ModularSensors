/*
 *InsituTrollSdi12.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org)
 * and extended by Neil Hancock
 *
 *This file is for the Insitu Level/Aqua Troll Devices - Tested Level Troll 500
 *It is dependent on the EnviroDIY SDI-12 library and the SDI12Sensors super
 *class.
 *
 * The Insitu Aqua/Level Troll require 8-36VDC (extra boost)
 * Instructions for cabling with a Polo #boost are at the end
 *
 *Documentation for the SDI-12 Protocol commands and responses
 * The Insitu Level/Aqua Troll can be found at:
 * Insitu SDI-12-Commands-and-Level-TROLL-400-500-700-Responses 20140210.pdf
 * Insitu SDI-12-Commands-and-Aqua-TROLL-100-200-Responses 20070123.pdf
 *
 * I haven't audited why Insitu have a different SDI manual for Level and Aqua
 *
 * For Pressure:
 *  Resolution is 0.001
 *  Accuracy is ±?
 *  Range is 0 – ?
 *
 * For Temperature:
 *  Resolution is 0.1°C
 *  Accuracy is ±1°C
 *  Range is -11°C to +49°C
 *
 * For Depth:
 *  Resolution is 2 mm
 *  Accuracy is ±0.05% of full scale
 *  Range is 0 to 5 m or 0 to 10 m, depending on model
 *
 * Maximum warm-up time in SDI-12 mode: 500ms, assume stability at warm-up
 * Maximum measurement duration: 500ms
 *
 * The Insitu Aqua/Level Trolls are programmed through WinSitu
 * Parameters are very flexible and need to be aligned with this program
 * The SDI address needs to be changed to what the class is set to - default is
 *'1' The depth sensor third paramter needs to be created. The expected
 *paramters and order are 0 Pressure (PSI)   ITROLL_PRESSURE_VAR_NUM 1
 *Temperature (C)  ITROLL_TEMP_VAR_NUM 2 Depth (ft)       ITROLL_DEPTH_VAR_NUM
 *Resolution 0.005% For 11.5ft +/ 0.00005ft
 *
 */

// Header Guards
#ifndef InsituTrollSdi12_h
#define InsituTrollSdi12_h

// Included Dependencies
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines
#define ITROLL_NUM_VARIABLES 3
#define ITROLL_WARM_UP_TIME_MS 500
#define ITROLL_STABILIZATION_TIME_MS 0
#define ITROLL_MEASUREMENT_TIME_MS 500

#define ITROLL_PRESSURE_RESOLUTION 5
// adding extra digit to resolution for averaging
#define ITROLL_PRESSURE_VAR_NUM 0

#define ITROLL_TEMP_RESOLUTION 2
// adding extra digit to resolution for averaging
#define ITROLL_TEMP_VAR_NUM 1

#define ITROLL_DEPTH_RESOLUTION 5
// adding extra digit to resolution for averaging
#define ITROLL_DEPTH_VAR_NUM 2

// The main class for the Insitu Level/Aqua Troll
class InsituTrollSdi12 : public SDI12Sensors {
public:
  // Constructors with overloads
  InsituTrollSdi12(char SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
      : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                     "InsituTrollSdi12", ITROLL_NUM_VARIABLES,
                     ITROLL_WARM_UP_TIME_MS, ITROLL_STABILIZATION_TIME_MS,
                     ITROLL_MEASUREMENT_TIME_MS) {}
  InsituTrollSdi12(char *SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
      : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                     "InsituTrollSdi12", ITROLL_NUM_VARIABLES,
                     ITROLL_WARM_UP_TIME_MS, ITROLL_STABILIZATION_TIME_MS,
                     ITROLL_MEASUREMENT_TIME_MS) {}
  InsituTrollSdi12(int SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
      : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                     "InsituTrollSdi12", ITROLL_NUM_VARIABLES,
                     ITROLL_WARM_UP_TIME_MS, ITROLL_STABILIZATION_TIME_MS,
                     ITROLL_MEASUREMENT_TIME_MS) {}
  // Destructor
  ~InsituTrollSdi12() {}
};

// Defines the Conductivity Variable
class InsituTrollSdi12_Pressure : public Variable {
public:
  InsituTrollSdi12_Pressure(
      Sensor *parentSense, const char *uuid = "",
      const char *varCode = "ITROLLPressure") // Does this maap to something?
      : Variable(parentSense, (const uint8_t)ITROLL_PRESSURE_VAR_NUM,
                 (uint8_t)ITROLL_PRESSURE_RESOLUTION, "pressure", "PSI",
                 varCode, uuid) {}
  InsituTrollSdi12_Pressure()
      : Variable((const uint8_t)ITROLL_PRESSURE_VAR_NUM,
                 (uint8_t)ITROLL_PRESSURE_RESOLUTION, "pressure", "PSI",
                 "Insitu TROLL Pressure") {}
  ~InsituTrollSdi12_Pressure() {}
};

// Defines the Temperature Variable
class InsituTrollSdi12_Temp : public Variable {
public:
  InsituTrollSdi12_Temp(Sensor *parentSense, const char *uuid = "",
                        const char *varCode = "ITROLLtemp")
      : Variable(parentSense, (const uint8_t)ITROLL_TEMP_VAR_NUM,
                 (uint8_t)ITROLL_TEMP_RESOLUTION, "temperature",
                 "degreeCelsius", varCode, uuid) {}
  InsituTrollSdi12_Temp()
      : Variable((const uint8_t)ITROLL_TEMP_VAR_NUM,
                 (uint8_t)ITROLL_TEMP_RESOLUTION, "temperature",
                 "degreeCelsius", "Insitu TROLL temperature") {}
  ~InsituTrollSdi12_Temp() {}
};

// Defines the Depth Variable
class InsituTrollSdi12_Depth : public Variable {
public:
  InsituTrollSdi12_Depth(Sensor *parentSense, const char *uuid = "",
                         const char *varCode = "ITROLLdepth")
      : Variable(parentSense, (const uint8_t)ITROLL_DEPTH_VAR_NUM,
                 (uint8_t)ITROLL_DEPTH_RESOLUTION, "waterDepth", "feet",
                 varCode, uuid) {}
  InsituTrollSdi12_Depth()
      : Variable((const uint8_t)ITROLL_DEPTH_VAR_NUM,
                 (uint8_t)ITROLL_DEPTH_RESOLUTION, "waterDepth", "feet",
                 "Insitu TROLL depth") {}
  ~InsituTrollSdi12_Depth() {}
};

#endif // Header Guard
