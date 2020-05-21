/**
 * @file YosemitechY520.h
 * @brief This file contains the YosemitechY520 sensor subclass and the variable
 * subclasses YosemitechY520_Cond and YosemitechY520_Temp.  These are for the
 * Yosemitech Y520 4-pole Conductivity sensor.  This depends on the
 * YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 *
 * These devices output very high "resolution" (32bits) so the resolutions are
 * based on their accuracy, not the resolution of the sensor.
 *
 * For Conductivity:
 *     Resolution is 0.1 µS/cm
 *     Accuracy is ± 1 % Full Scale
 *     Range is 1 µS/cm to 200 mS/cm
 *
 * For Temperature:
 *     Resolution is 0.1 °C
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 1200 ms
 * Time between "StartMeasurement" command and stable reading - 10sec
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY520_H_
#define SRC_SENSORS_YOSEMITECHY520_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
#define Y520_NUM_VARIABLES 2
#define Y520_WARM_UP_TIME_MS 1600
#define Y520_STABILIZATION_TIME_MS 10000
#define Y520_MEASUREMENT_TIME_MS 2700

#define Y520_COND_RESOLUTION 1
#define Y520_COND_VAR_NUM 0

#define Y520_TEMP_RESOLUTION 1
#define Y520_TEMP_VAR_NUM 1

// The main class for the Decagon Y520
class YosemitechY520 : public YosemitechParent {
 public:
    // Constructors with overloads
    YosemitechY520(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y520,
                           "YosemitechY520", Y520_NUM_VARIABLES,
                           Y520_WARM_UP_TIME_MS, Y520_STABILIZATION_TIME_MS,
                           Y520_MEASUREMENT_TIME_MS) {}
    YosemitechY520(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y520,
                           "YosemitechY520", Y520_NUM_VARIABLES,
                           Y520_WARM_UP_TIME_MS, Y520_STABILIZATION_TIME_MS,
                           Y520_MEASUREMENT_TIME_MS) {}
    ~YosemitechY520() {}
};


// Defines the Conductivity
class YosemitechY520_Cond : public Variable {
 public:
    explicit YosemitechY520_Cond(YosemitechY520* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y520Cond")
        : Variable(parentSense, (const uint8_t)Y520_COND_VAR_NUM,
                   (uint8_t)Y520_COND_RESOLUTION, "specificConductance",
                   "microsiemenPerCentimeter", varCode, uuid) {}
    YosemitechY520_Cond()
        : Variable((const uint8_t)Y520_COND_VAR_NUM,
                   (uint8_t)Y520_COND_RESOLUTION, "specificConductance",
                   "microsiemenPerCentimeter", "Y520Cond") {}
    ~YosemitechY520_Cond() {}
};


// Defines the Temperature Variable
class YosemitechY520_Temp : public Variable {
 public:
    explicit YosemitechY520_Temp(YosemitechY520* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y520Temp")
        : Variable(parentSense, (const uint8_t)Y520_TEMP_VAR_NUM,
                   (uint8_t)Y520_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    YosemitechY520_Temp()
        : Variable((const uint8_t)Y520_TEMP_VAR_NUM,
                   (uint8_t)Y520_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y520Temp") {}
    ~YosemitechY520_Temp() {}
};

#endif  // SRC_SENSORS_YOSEMITECHY520_H_
