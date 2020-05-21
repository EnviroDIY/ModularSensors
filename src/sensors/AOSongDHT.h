/**
 * @file AOSongDHT.cpp
 * @brief Contains the AOSongDHT sensor subclass and the variable subclasses
 * AOSongDHT_Humidity, AOSongDHT_Temp, and AOSongDHT_HI.
 *
 * These are used for the AOSong digital-output relative humidity & temperature
 * sensors/modules: DHT11, DHT21(AM2301), and DHT 22 (AM2302).
 *
 * This file is dependent on the Adafruit DHT Library.
 *
 * Documentation for the sensor can be found at:
 * http://www.aosong.com/en/products/details.asp?id=117
 *
 * For Relative Humidity:
 *  Resolution is 0.1 % RH
 *  Accuracy is ± 2 % RH
 *  Range is 0 to 100 % RH
 *
 * For Temperature:
 *  Resolution is 0.1°C
 *  Accuracy is ±0.5°C
 *  Range is -40°C to +80°C
 *
 * Warm up/sampling time: 1.7sec
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 */

// Header Guards
#ifndef SRC_SENSORS_AOSONGDHT_H_
#define SRC_SENSORS_AOSONGDHT_H_

// Debugging Statement
// #define MS_AOSONGDHT_DEBUG

#ifdef MS_AOSONGDHT_DEBUG
#define MS_DEBUGGING_STD "AOSongDHT"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <DHT.h>

// Undefine these macros so I can use a typedef instead
#undef DHT11
#undef DHT21
#undef AM2301
#undef DHT22
#undef AM2302

// Sensor Specific Defines
#define DHT_NUM_VARIABLES 3
#define DHT_WARM_UP_TIME_MS 1700
#define DHT_STABILIZATION_TIME_MS 0
#define DHT_MEASUREMENT_TIME_MS 2000

#define DHT_HUMIDITY_RESOLUTION 1
#define DHT_HUMIDITY_VAR_NUM 0

#define DHT_TEMP_RESOLUTION 1
#define DHT_TEMP_VAR_NUM 1

#define DHT_HI_RESOLUTION 1
#define DHT_HI_VAR_NUM 2

// For the various communication devices"
typedef enum DHTtype {
    DHT11  = 11,
    DHT21  = 21,
    AM2301 = 21,
    DHT22  = 22,
    AM2302 = 22
} DHTtype;

// The main class for the AOSong DHT
class AOSongDHT : public Sensor {
 public:
    // The constructor - need the power pin, the data pin, and the sensor type
    AOSongDHT(int8_t powerPin, int8_t dataPin, DHTtype type,
              uint8_t measurementsToAverage = 1);
    // Destructor
    ~AOSongDHT();

    bool   setup(void) override;
    String getSensorName(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    DHT     dht_internal;
    DHTtype _dhtType;
};


// Defines the Humidity Variable
class AOSongDHT_Humidity : public Variable {
 public:
    explicit AOSongDHT_Humidity(AOSongDHT* parentSense, const char* uuid = "",
                                const char* varCode = "DHTHumidity")
        : Variable(parentSense, (const uint8_t)DHT_HUMIDITY_VAR_NUM,
                   (uint8_t)DHT_HUMIDITY_RESOLUTION, "relativeHumidity",
                   "percent", varCode, uuid) {}
    AOSongDHT_Humidity()
        : Variable((const uint8_t)DHT_HUMIDITY_VAR_NUM,
                   (uint8_t)DHT_HUMIDITY_RESOLUTION, "relativeHumidity",
                   "percent", "DHTHumidity") {}
    ~AOSongDHT_Humidity() {}
};


// Defines the Temperature Variable
class AOSongDHT_Temp : public Variable {
 public:
    explicit AOSongDHT_Temp(AOSongDHT* parentSense, const char* uuid = "",
                            const char* varCode = "DHTTemp")
        : Variable(parentSense, (const uint8_t)DHT_TEMP_VAR_NUM,
                   (uint8_t)DHT_TEMP_RESOLUTION, "temperature", "degreeCelsius",
                   varCode, uuid) {}
    AOSongDHT_Temp()
        : Variable((const uint8_t)DHT_TEMP_VAR_NUM,
                   (uint8_t)DHT_TEMP_RESOLUTION, "temperature", "degreeCelsius",
                   "DHTTemp") {}
    ~AOSongDHT_Temp() {}
};


// Defines the Heat Index Variable
class AOSongDHT_HI : public Variable {
 public:
    explicit AOSongDHT_HI(AOSongDHT* parentSense, const char* uuid = "",
                          const char* varCode = "DHTHI")
        : Variable(parentSense, (const uint8_t)DHT_HI_VAR_NUM,
                   (uint8_t)DHT_HI_RESOLUTION, "heatIndex", "degreeCelsius",
                   varCode, uuid) {}
    AOSongDHT_HI()
        : Variable((const uint8_t)DHT_HI_VAR_NUM, (uint8_t)DHT_HI_RESOLUTION,
                   "heatIndex", "degreeCelsius", "DHTHI") {}
    ~AOSongDHT_HI() {}
};

#endif  // SRC_SENSORS_AOSONGDHT_H_
