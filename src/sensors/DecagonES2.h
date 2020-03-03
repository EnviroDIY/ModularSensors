/*
 *DecagonES2.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Decagon Devices ES-2 Electrical Conductivity Sensor
 *It is dependent on the EnviroDIY SDI-12 library and the SDI12Sensors super
 *class.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon ES-2 can be found at:
 * http://manuals.decagon.com/Integration%20Guides/ES-2%20Integrators%20Guide.pdf
 *
 * For Specific Conductance:
 *  Resolution is 0.001 mS/cm = 1 µS/cm
 *  Accuracy is ±0.01mS/cm or ±10% (whichever is greater)
 *  Range is 0 – 120 mS/cm (bulk)
 *
 * For Temperature:
 *  Resolution is 0.1°C
 *  Accuracy is ±1°C
 *  Range is -40°C to +50°C
 *
 * Maximum warm-up time in SDI-12 mode: 250ms, assume stability at warm-up
 * Maximum measurement duration: 250ms
 */

// Header Guards
#ifndef SRC_SENSORS_DECAGONES2_H_
#define SRC_SENSORS_DECAGONES2_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines
#define ES2_NUM_VARIABLES 2
#define ES2_WARM_UP_TIME_MS 250
#define ES2_STABILIZATION_TIME_MS 0
#define ES2_MEASUREMENT_TIME_MS 250

#define ES2_COND_RESOLUTION 1
// adding extra digit to resolution for averaging
#define ES2_COND_VAR_NUM 0

#define ES2_TEMP_RESOLUTION 2
// adding extra digit to resolution for averaging
#define ES2_TEMP_VAR_NUM 1

// The main class for the Decagon ES-2
class DecagonES2 : public SDI12Sensors {
 public:
    // Constructors with overloads
    DecagonES2(char SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "DecagonES2", ES2_NUM_VARIABLES, ES2_WARM_UP_TIME_MS,
                       ES2_STABILIZATION_TIME_MS, ES2_MEASUREMENT_TIME_MS) {}
    DecagonES2(char* SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "DecagonES2", ES2_NUM_VARIABLES, ES2_WARM_UP_TIME_MS,
                       ES2_STABILIZATION_TIME_MS, ES2_MEASUREMENT_TIME_MS) {}
    DecagonES2(int SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "DecagonES2", ES2_NUM_VARIABLES, ES2_WARM_UP_TIME_MS,
                       ES2_STABILIZATION_TIME_MS, ES2_MEASUREMENT_TIME_MS) {}
    // Destructor
    ~DecagonES2() {}
};


// Defines the Conductivity Variable
class DecagonES2_Cond : public Variable {
 public:
    DecagonES2_Cond(Sensor* parentSense, const char* uuid = "",
                    const char* varCode = "ES2Cond")
        : Variable(parentSense, (const uint8_t)ES2_COND_VAR_NUM,
                   (uint8_t)ES2_COND_RESOLUTION, "specificConductance",
                   "microsiemenPerCentimeter", varCode, uuid) {}
    DecagonES2_Cond()
        : Variable((const uint8_t)ES2_COND_VAR_NUM,
                   (uint8_t)ES2_COND_RESOLUTION, "specificConductance",
                   "microsiemenPerCentimeter", "ES2Cond") {}
    ~DecagonES2_Cond() {}
};

// Defines the Temperature Variable
class DecagonES2_Temp : public Variable {
 public:
    DecagonES2_Temp(Sensor* parentSense, const char* uuid = "",
                    const char* varCode = "ES2Temp")
        : Variable(parentSense, (const uint8_t)ES2_TEMP_VAR_NUM,
                   (uint8_t)ES2_TEMP_RESOLUTION, "temperature", "degreeCelsius",
                   varCode, uuid) {}
    DecagonES2_Temp()
        : Variable((const uint8_t)ES2_TEMP_VAR_NUM,
                   (uint8_t)ES2_TEMP_RESOLUTION, "temperature", "degreeCelsius",
                   "ES2Temp") {}
    ~DecagonES2_Temp() {}
};

#endif  // SRC_SENSORS_DECAGONES2_H_
