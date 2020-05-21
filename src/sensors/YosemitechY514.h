/**
 * @file YosemitechY514.h
 * @brief Contains the YosemitechY514 sensor subclass and the variable
 * subclasses YosemitechY514_Chlorophyll and YosemitechY514_Temp.
 *
 * These are for the Yosemitech Y514 Chlorophyll Sensor with Wiper.
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
 * For Chlorophyll:
 *     Resolution is 0.1 µg/L / 0.1 RFU
 *     Accuracy is ± 1 %
 *     Range is 0 to 400 µg/L or 0 to 100 RFU
 *
 * For Temperature:
 *     Resolution is 0.1 °C
 *     Accuracy is ± 0.2°C
 *     Range is 0°C to + 50°C
 *
 * Time before sensor responds after power - 1.2 seconds
 * Time between "StartMeasurement" command and stable reading - 8sec
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY514_H_
#define SRC_SENSORS_YOSEMITECHY514_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
#define Y514_NUM_VARIABLES 2
#define Y514_WARM_UP_TIME_MS 1300
#define Y514_STABILIZATION_TIME_MS 8000
#define Y514_MEASUREMENT_TIME_MS 2000

#define Y514_CHLORO_RESOLUTION 1
#define Y514_CHLORO_VAR_NUM 0

#define Y514_TEMP_RESOLUTION 1
#define Y514_TEMP_VAR_NUM 1

// The main class for the Decagon Y514
class YosemitechY514 : public YosemitechParent {
 public:
    // Constructors with overloads
    YosemitechY514(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y514,
                           "YosemitechY514", Y514_NUM_VARIABLES,
                           Y514_WARM_UP_TIME_MS, Y514_STABILIZATION_TIME_MS,
                           Y514_MEASUREMENT_TIME_MS) {}
    YosemitechY514(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y514,
                           "YosemitechY514", Y514_NUM_VARIABLES,
                           Y514_WARM_UP_TIME_MS, Y514_STABILIZATION_TIME_MS,
                           Y514_MEASUREMENT_TIME_MS) {}
    ~YosemitechY514() {}
};


// Defines the Chlorophyll Concentration
class YosemitechY514_Chlorophyll : public Variable {
 public:
    explicit YosemitechY514_Chlorophyll(YosemitechY514* parentSense,
                                        const char*     uuid    = "",
                                        const char*     varCode = "Y514Chloro")
        : Variable(parentSense, (const uint8_t)Y514_CHLORO_VAR_NUM,
                   (uint8_t)Y514_CHLORO_RESOLUTION, "chlorophyllFluorescence",
                   "microgramPerLiter", varCode, uuid) {}
    YosemitechY514_Chlorophyll()
        : Variable((const uint8_t)Y514_CHLORO_VAR_NUM,
                   (uint8_t)Y514_CHLORO_RESOLUTION, "chlorophyllFluorescence",
                   "microgramPerLiter", "Y514Chloro") {}
    ~YosemitechY514_Chlorophyll() {}
};


// Defines the Temperature Variable
class YosemitechY514_Temp : public Variable {
 public:
    explicit YosemitechY514_Temp(YosemitechY514* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "Y514Temp")
        : Variable(parentSense, (const uint8_t)Y514_TEMP_VAR_NUM,
                   (uint8_t)Y514_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    YosemitechY514_Temp()
        : Variable((const uint8_t)Y514_TEMP_VAR_NUM,
                   (uint8_t)Y514_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "Y514Temp") {}
    ~YosemitechY514_Temp() {}
};
#endif  // SRC_SENSORS_YOSEMITECHY514_H_
