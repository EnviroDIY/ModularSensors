/*
 *AOSongDHT.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the AOSong Digital-output relative humidity & temperature sensor/modules:
 *DHT11, DHT21(AM2301), and DHT 22 (AM2302).  It is dependent on the Adafruit DHT Library
 *
 *Documentation for the sensor can be found at:
 *http://www.aosong.com/en/products/details.asp?id=117
 *
 * For Relative Humidity:
 *  Resolution is 0.1 % RH for DHT22 and 1 % RH for DHT11
 *  Accuracy is ± 2 % RH for DHT22 and ± 5 % RH for DHT11
 *  Range is 0 to 100 % RH
 *
 * For Temperature:
 *  Resolution is 0.1°C
 *  Accuracy is ±0.5°C for DHT22 and ± ±2°C for DHT11
 *  Range is -40°C to +80°C
 *
 * Warm up time: 1.7sec; assume stable on warm-up
 * Re-sampling time: 2.0sec
*/

// Header Guards
#ifndef AOSongDHT_h
#define AOSongDHT_h

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
typedef enum DHTtype
{
  DHT11 = 11,
  DHT21 = 21,
  AM2301 = 21,
  DHT22 = 22,
  AM2302 = 22
} DHTtype;

// The main class for the AOSong DHT
class AOSongDHT : public Sensor
{
public:
    // The constructor - need the power pin, the data pin, and the sensor type
    AOSongDHT(int8_t powerPin, int8_t dataPin, DHTtype type, uint8_t measurementsToAverage = 1);
    // Destructor
    ~AOSongDHT();

    bool setup(void) override;
    String getSensorName(void) override;

    bool addSingleMeasurementResult(void) override;

private:
    DHT dht_internal;
    DHTtype _dhtType;
};


// Defines the Humidity Variable
class AOSongDHT_Humidity : public Variable
{
public:
    AOSongDHT_Humidity(Sensor *parentSense,
                       const char *uuid = "",
                       const char *varCode = "DHTHumidity")
      : Variable(parentSense,
                 (const uint8_t)DHT_HUMIDITY_VAR_NUM,
                 (uint8_t)DHT_HUMIDITY_RESOLUTION,
                 "relativeHumidity", "percent",
                 varCode, uuid)
    {}
    AOSongDHT_Humidity()
      : Variable((const uint8_t)DHT_HUMIDITY_VAR_NUM,
                 (uint8_t)DHT_HUMIDITY_RESOLUTION,
                 "relativeHumidity", "percent", "DHTHumidity")
    {}
    ~AOSongDHT_Humidity(){};
};


// Defines the Temperature Variable
class AOSongDHT_Temp : public Variable
{
public:
    AOSongDHT_Temp(Sensor *parentSense,
                   const char *uuid = "",
                   const char *varCode = "DHTTemp")
      : Variable(parentSense,
                 (const uint8_t)DHT_TEMP_VAR_NUM,
                 (uint8_t)DHT_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius",
                 varCode, uuid)
    {}
    AOSongDHT_Temp()
      : Variable((const uint8_t)DHT_TEMP_VAR_NUM,
                 (uint8_t)DHT_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius", "DHTTemp")
    {}
    ~AOSongDHT_Temp(){};
};


// Defines the Heat Index Variable
class AOSongDHT_HI : public Variable
{
public:
    AOSongDHT_HI(Sensor *parentSense,
                 const char *uuid = "",
                 const char *varCode = "DHTHI")
      : Variable(parentSense,
                 (const uint8_t)DHT_HI_VAR_NUM,
                 (uint8_t)DHT_HI_RESOLUTION,
                 "heatIndex", "degreeCelsius",
                 varCode, uuid)
    {}
    AOSongDHT_HI()
      : Variable((const uint8_t)DHT_HI_VAR_NUM,
                 (uint8_t)DHT_HI_RESOLUTION,
                 "heatIndex", "degreeCelsius", "DHTHI")
    {}
    ~AOSongDHT_HI(){};
};

#endif  // Header Guard
