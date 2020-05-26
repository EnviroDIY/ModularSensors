/**
 * @file YosemitechY504.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY504 sensor subclass and the variable
 * subclasses YosemitechY504_DOpct, YosemitechY504_Temp, and
 * YosemitechY504_DOmgL.
 *
 * These are for the Yosemitech Y504 Optical Dissolved Oxygen sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 *
 * These devices output very high "resolution" (32bits) so the resolutions are
 * based on their accuracy, not the resolution of the sensor.
 *
 * For Dissolved Oxygen:
 *     Accuracy is ± 1 %
 *     Range is 0-20mg/L or 0-200% Saturation
 *
 * For Temperature:
 *     Resolution is 0.1 °C
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 275-300ms (use 350 for safety)
 * Time between "StartMeasurement" command and stable reading - 8sec
 */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY504_H_
#define SRC_SENSORS_YOSEMITECHY504_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
#define Y504_NUM_VARIABLES 3
#define Y504_WARM_UP_TIME_MS 375
#define Y504_STABILIZATION_TIME_MS 8000
#define Y504_MEASUREMENT_TIME_MS 1700

#define Y504_DOPCT_RESOLUTION 1
#define Y504_DOPCT_VAR_NUM 0

#define Y504_TEMP_RESOLUTION 1
#define Y504_TEMP_VAR_NUM 1

#define Y504_DOMGL_RESOLUTION 2
#define Y504_DOMGL_VAR_NUM 2

// The main class for the Decagon Y504
class YosemitechY504 : public YosemitechParent {
 public:
    // Constructors with overloads
    YosemitechY504(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y504,
                           "YosemitechY504", Y504_NUM_VARIABLES,
                           Y504_WARM_UP_TIME_MS, Y504_STABILIZATION_TIME_MS,
                           Y504_MEASUREMENT_TIME_MS) {}
    YosemitechY504(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y504,
                           "YosemitechY504", Y504_NUM_VARIABLES,
                           Y504_WARM_UP_TIME_MS, Y504_STABILIZATION_TIME_MS,
                           Y504_MEASUREMENT_TIME_MS) {}
    ~YosemitechY504() {}
};


// Defines the Dissolved Oxygen Percent Saturation
class YosemitechY504_DOpct : public Variable {
 public:
    explicit YosemitechY504_DOpct(YosemitechY504* parentSense,
                                  const char*     uuid    = "",
                                  const char*     varCode = "Y504DOpct")
        : Variable(parentSense, (const uint8_t)Y504_DOPCT_VAR_NUM,
                   (uint8_t)Y504_DOPCT_RESOLUTION,
                   "oxygenDissolvedPercentOfSaturation", "percent", varCode,
                   uuid) {}
    YosemitechY504_DOpct()
        : Variable(
              (const uint8_t)Y504_DOPCT_VAR_NUM, (uint8_t)Y504_DOPCT_RESOLUTION,
              "oxygenDissolvedPercentOfSaturation", "percent", "Y504DOpct") {}
    ~YosemitechY504_DOpct() {}
};


// Defines the Temperature Variable
class YosemitechY504_Temp : public Variable {
 public:
    explicit YosemitechY504_Temp(YosemitechY504* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y504Temp")
        : Variable(parentSense, (const uint8_t)Y504_TEMP_VAR_NUM,
                   (uint8_t)Y504_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    YosemitechY504_Temp()
        : Variable((const uint8_t)Y504_TEMP_VAR_NUM,
                   (uint8_t)Y504_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y504Temp") {}
    ~YosemitechY504_Temp() {}
};


// Defines the Dissolved Oxygen Concentration
class YosemitechY504_DOmgL : public Variable {
 public:
    explicit YosemitechY504_DOmgL(YosemitechY504* parentSense,
                                  const char*     uuid    = "",
                                  const char*     varCode = "Y504DOmgL")
        : Variable(parentSense, (const uint8_t)Y504_DOMGL_VAR_NUM,
                   (uint8_t)Y504_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", varCode, uuid) {}
    YosemitechY504_DOmgL()
        : Variable((const uint8_t)Y504_DOMGL_VAR_NUM,
                   (uint8_t)Y504_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", "Y504DOmgL") {}
    ~YosemitechY504_DOmgL() {}
};

#endif  // SRC_SENSORS_YOSEMITECHY504_H_
