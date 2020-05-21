/**
 * @file ZebraTechDOpto.h
 * @brief Contains the ZebraTechDOpto sensor subclass and the variable
 * subclasses ZebraTechDOpto_Temp, ZebraTechDOpto_DOpct, and
 * ZebraTechDOpto_DOmgL.
 *
 * These are for the ZebraTech D-Opto digital dissolved oxygen sensor.
 *
 * This depends on the SDI12Sensors parent class.
 *
 * The manual for this sensor is available at:
 * www.zebra-tech.co.nz/wp-content/uploads/2014/04/D-Opto-Sensor-manual-A4-ver-2.pdf
 *
 * For Dissolved Oxygen:
 *     Accuracy is 1% of reading or 0.02PPM, whichever is greater
 *     Resolution is 0.01% / 0.001 PPM
 *
 * For Temperature:
 *     Accuracy is ± 0.1°C
 *     Resolution is 0.01°C
 *
 * Maximum warm-up time in SDI-12 mode: ~250ms
 * Excitiation time before measurement: ~5225ms
 * Maximum measurement duration: ~110ms
 *
 * Obnoxiously, the sensor will not take a "concurrent" measurement and leave
 * the sensor powered on, so we must wait entire ~5200ms exitation time and the
 * 110ms measurement time each time.
 * There is the ability to do a non-concurrent measurement and leave the
 * sensor powered on, in which case the re-measurement takes ~110ms, but doing
 * it that way the sensor would send an interruption when it was finished,
 * possibly colliding with and confusing other sensor results
 *
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 */

// Header Guards
#ifndef SRC_SENSORS_ZEBRATECHDOPTO_H_
#define SRC_SENSORS_ZEBRATECHDOPTO_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines
#define DOPTO_NUM_VARIABLES 3
#define DOPTO_WARM_UP_TIME_MS 275
#define DOPTO_STABILIZATION_TIME_MS 0
#define DOPTO_MEASUREMENT_TIME_MS 5335

#define DOPTO_TEMP_RESOLUTION 2
#define DOPTO_TEMP_VAR_NUM 0

#define DOPTO_DOPCT_RESOLUTION 2
#define DOPTO_DOPCT_VAR_NUM 1

#define DOPTO_DOMGL_RESOLUTION 3
#define DOPTO_DOMGL_VAR_NUM 2

// The main class for the D-Opto
class ZebraTechDOpto : public SDI12Sensors {
 public:
    // Constructors with overloads
    ZebraTechDOpto(char SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                       DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS,
                       DOPTO_MEASUREMENT_TIME_MS) {}
    ZebraTechDOpto(char* SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                       DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS,
                       DOPTO_MEASUREMENT_TIME_MS) {}
    ZebraTechDOpto(int SDI12address, int8_t powerPin, int8_t dataPin,
                   uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                       DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS,
                       DOPTO_MEASUREMENT_TIME_MS) {}
    ~ZebraTechDOpto() {}
};


// Defines the Temperature Variable
class ZebraTechDOpto_Temp : public Variable {
 public:
    explicit ZebraTechDOpto_Temp(ZebraTechDOpto* parentSense,
                                 const char*     uuid    = "",
                                 const char*     varCode = "DOtempC")
        : Variable(parentSense, (const uint8_t)DOPTO_TEMP_VAR_NUM,
                   (uint8_t)DOPTO_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    ZebraTechDOpto_Temp()
        : Variable((const uint8_t)DOPTO_TEMP_VAR_NUM,
                   (uint8_t)DOPTO_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "DOtempC") {}
    ~ZebraTechDOpto_Temp() {}
};


// Defines the Dissolved Oxygen Percent Saturation
class ZebraTechDOpto_DOpct : public Variable {
 public:
    explicit ZebraTechDOpto_DOpct(ZebraTechDOpto* parentSense,
                                  const char*     uuid    = "",
                                  const char*     varCode = "DOpercent")
        : Variable(parentSense, (const uint8_t)DOPTO_DOPCT_VAR_NUM,
                   (uint8_t)DOPTO_DOPCT_RESOLUTION,
                   "oxygenDissolvedPercentOfSaturation", "percent", varCode,
                   uuid) {}
    ZebraTechDOpto_DOpct()
        : Variable((const uint8_t)DOPTO_DOPCT_VAR_NUM,
                   (uint8_t)DOPTO_DOPCT_RESOLUTION,
                   "oxygenDissolvedPercentOfSaturation", "percent",
                   "DOpercent") {}
    ~ZebraTechDOpto_DOpct() {}
};


// Defines the Dissolved Oxygen Concentration
class ZebraTechDOpto_DOmgL : public Variable {
 public:
    explicit ZebraTechDOpto_DOmgL(ZebraTechDOpto* parentSense,
                                  const char*     uuid    = "",
                                  const char*     varCode = "DOppm")
        : Variable(parentSense, (const uint8_t)DOPTO_DOMGL_VAR_NUM,
                   (uint8_t)DOPTO_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", varCode, uuid) {}
    ZebraTechDOpto_DOmgL()
        : Variable((const uint8_t)DOPTO_DOMGL_VAR_NUM,
                   (uint8_t)DOPTO_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", "DOppm") {}
    ~ZebraTechDOpto_DOmgL() {}
};

#endif  // SRC_SENSORS_ZEBRATECHDOPTO_H_
