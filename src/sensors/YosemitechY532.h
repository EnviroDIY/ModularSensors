/**
 * @file YosemitechY532.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY532 sensor subclass and the variable
 * subclasses YosemitechY532_pH, YosemitechY532_Temp, and
 * YosemitechY532_Voltage.
 *
 * These are for the Yosemitech Y532 pH sensor.
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
 * For pH:
 *     Resolution is 0.01 pH units
 *     Accuracy is ± 0.1 pH units
 *     Range is 2 to 12 pH units
 *
 * For Temperature:
 *     Resolution is 0.1 °C
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 500ms
 * Time between "StartMeasurement" command and stable reading - 4.5sec
 */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY532_H_
#define SRC_SENSORS_YOSEMITECHY532_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
#define Y532_NUM_VARIABLES 3
#define Y532_WARM_UP_TIME_MS 500
#define Y532_STABILIZATION_TIME_MS 4500
#define Y532_MEASUREMENT_TIME_MS 1800

#define Y532_PH_RESOLUTION 2
#define Y532_PH_VAR_NUM 0

#define Y532_TEMP_RESOLUTION 1
#define Y532_TEMP_VAR_NUM 1

#define Y532_VOLT_RESOLUTION 0
#define Y532_VOLT_VAR_NUM 2

// The main class for the Decagon Y532
class YosemitechY532 : public YosemitechParent {
 public:
    // Constructors with overloads
    YosemitechY532(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y532,
                           "YosemitechY532", Y532_NUM_VARIABLES,
                           Y532_WARM_UP_TIME_MS, Y532_STABILIZATION_TIME_MS,
                           Y532_MEASUREMENT_TIME_MS) {}
    YosemitechY532(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y532,
                           "YosemitechY532", Y532_NUM_VARIABLES,
                           Y532_WARM_UP_TIME_MS, Y532_STABILIZATION_TIME_MS,
                           Y532_MEASUREMENT_TIME_MS) {}
    ~YosemitechY532() {}
};


// Defines the pH
class YosemitechY532_pH : public Variable {
 public:
    explicit YosemitechY532_pH(YosemitechY532* parentSense,
                               const char*     uuid    = "",
                               const char*     varCode = "Y532pH")
        : Variable(parentSense, (const uint8_t)Y532_PH_VAR_NUM,
                   (uint8_t)Y532_PH_RESOLUTION, "pH", "pH", varCode, uuid) {}
    YosemitechY532_pH()
        : Variable((const uint8_t)Y532_PH_VAR_NUM, (uint8_t)Y532_PH_RESOLUTION,
                   "pH", "pH", "Y532pH") {}
    ~YosemitechY532_pH() {}
};


// Defines the Temperature Variable
class YosemitechY532_Temp : public Variable {
 public:
    explicit YosemitechY532_Temp(YosemitechY532* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y532Temp")
        : Variable(parentSense, (const uint8_t)Y532_TEMP_VAR_NUM,
                   (uint8_t)Y532_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    YosemitechY532_Temp()
        : Variable((const uint8_t)Y532_TEMP_VAR_NUM,
                   (uint8_t)Y532_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y532Temp") {}
    ~YosemitechY532_Temp() {}
};


// Defines the Electrode Electrical Potential
class YosemitechY532_Voltage : public Variable {
 public:
    explicit YosemitechY532_Voltage(YosemitechY532* parentSense,
                                    const char*     uuid    = "",
                                    const char*     varCode = "Y532Potential")
        : Variable(parentSense, (const uint8_t)Y532_VOLT_VAR_NUM,
                   (uint8_t)Y532_VOLT_RESOLUTION, "voltage", "millivolt",
                   varCode, uuid) {}
    YosemitechY532_Voltage()
        : Variable((const uint8_t)Y532_VOLT_VAR_NUM,
                   (uint8_t)Y532_VOLT_RESOLUTION, "voltage", "millivolt",
                   "Y532Potential") {}
    ~YosemitechY532_Voltage() {}
};

#endif  // SRC_SENSORS_YOSEMITECHY532_H_
