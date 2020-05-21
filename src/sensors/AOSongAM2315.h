/**
 * @file AOSongAM2315.h
 * @brief This file contains the AOSongAM2315 sensor subclass and the variable
 * subclasses AOSongAM2315_Humidity and AOSongAM2315_Temp.  These are used for
 * the AOSong AM2315 capacitive humidity and temperature sensor.  This file is
 * dependent on the Adafruit AM2315 Library.
 *
 * Documentation for the sensor can be found at:
 * www.aosong.com/asp_bin/Products/en/AM2315.pdf
 *
 * For Relative Humidity:
 *  Resolution is 0.1 % RH (16 bit)
 *  Accuracy is ± 2 % RH
 *
 * For Temperature:
 *  Resolution is 0.1°C (16 bit)
 *  Accuracy is ±0.1°C
 *  Range is -40°C to +125°C
 *
 * Warm up/stability/re-sampling time: 2sec
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 */

// Header Guards
#ifndef SRC_SENSORS_AOSONGAM2315_H_
#define SRC_SENSORS_AOSONGAM2315_H_

// Debugging Statement
// #define MS_AOSONGAM2315_DEBUG

#ifdef MS_AOSONGAM2315_DEBUG
#define MS_DEBUGGING_STD "AOSongAM2315"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
#define AM2315_NUM_VARIABLES 2
#define AM2315_WARM_UP_TIME_MS 500
#define AM2315_STABILIZATION_TIME_MS 500
#define AM2315_MEASUREMENT_TIME_MS 2000

#define AM2315_HUMIDITY_RESOLUTION 1
#define AM2315_HUMIDITY_VAR_NUM 0

#define AM2315_TEMP_RESOLUTION 1
#define AM2315_TEMP_VAR_NUM 1


// The main class for the AOSong AM2315
class AOSongAM2315 : public Sensor {
 public:
    // The constructor - because this is I2C, only need the power pin
    explicit AOSongAM2315(int8_t powerPin, uint8_t measurementsToAverage = 1);
    // Destructor
    ~AOSongAM2315();

    String getSensorLocation(void) override;

    bool setup(void) override;

    bool addSingleMeasurementResult(void) override;
};


// Defines the Humidity Variable
class AOSongAM2315_Humidity : public Variable {
 public:
    explicit AOSongAM2315_Humidity(AOSongAM2315* parentSense,
                                   const char*   uuid    = "",
                                   const char*   varCode = "AM2315Humidity")
        : Variable(parentSense, (const uint8_t)AM2315_HUMIDITY_VAR_NUM,
                   (uint8_t)AM2315_HUMIDITY_RESOLUTION, "relativeHumidity",
                   "percent", varCode, uuid) {}
    AOSongAM2315_Humidity()
        : Variable((const uint8_t)AM2315_HUMIDITY_VAR_NUM,
                   (uint8_t)AM2315_HUMIDITY_RESOLUTION, "relativeHumidity",
                   "percent", "AM2315Humidity") {}
    ~AOSongAM2315_Humidity() {}
};


// Defines the Temperature Variable
class AOSongAM2315_Temp : public Variable {
 public:
    explicit AOSongAM2315_Temp(AOSongAM2315* parentSense, const char* uuid = "",
                               const char* varCode = "AM2315Temp")
        : Variable(parentSense, (const uint8_t)AM2315_TEMP_VAR_NUM,
                   (uint8_t)AM2315_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    AOSongAM2315_Temp()
        : Variable((const uint8_t)AM2315_TEMP_VAR_NUM,
                   (uint8_t)AM2315_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "AM2315Temp") {}
    ~AOSongAM2315_Temp() {}
};

#endif  // SRC_SENSORS_AOSONGAM2315_H_
