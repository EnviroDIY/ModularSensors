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

#ifndef AOSongDHT_h
#define AOSongDHT_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"

#include <DHT.h>
// Undefine these macros so I can use a typedef instead
#undef DHT11
#undef DHT21
#undef AM2301
#undef DHT22
#undef AM2302

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

    SENSOR_STATUS setup(void) override;
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
                       String UUID = "", String customVarCode = "")
      : Variable(parentSense, DHT_HUMIDITY_VAR_NUM,
               F("relativeHumidity"), F("percent"),
               DHT_HUMIDITY_RESOLUTION,
               F("DHTHumidity"), UUID, customVarCode)
    {}
};


// Defines the Temperature Variable
class AOSongDHT_Temp : public Variable
{
public:
    AOSongDHT_Temp(Sensor *parentSense,
                   String UUID = "", String customVarCode = "")
      : Variable(parentSense, DHT_TEMP_VAR_NUM,
               F("temperature"), F("degreeCelsius"),
               DHT_TEMP_RESOLUTION,
               F("DHTTemp"), UUID, customVarCode)
    {}
};


// Defines the Heat Index Variable
class AOSongDHT_HI : public Variable
{
public:
    AOSongDHT_HI(Sensor *parentSense,
                 String UUID = "", String customVarCode = "")
      : Variable(parentSense, DHT_HI_VAR_NUM,
               F("heatIndex"), F("degreeCelsius"),
               DHT_HI_RESOLUTION,
               F("DHTHI"), UUID, customVarCode)
    {}
};

#endif
