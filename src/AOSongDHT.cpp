/*
 *AOSongDHT.cpp
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
*/

#include "AOSongDHT.h"


// The constructor - need the power pin, data pin, and type of DHT
AOSongDHT::AOSongDHT(int8_t powerPin, int8_t dataPin, DHTtype type, uint8_t measurementsToAverage)
    : Sensor(F("AOSongDHT"), DHT_NUM_VARIABLES,
             DHT_WARM_UP_TIME_MS, DHT_STABILIZATION_TIME_MS, DHT_MEASUREMENT_TIME_MS,
             powerPin, dataPin, measurementsToAverage),
    dht_internal(dataPin, type)
{
    _dhtType = type;
}


bool AOSongDHT::setup(void)
{
    bool retVal = Sensor::setup();
    dht_internal.begin();  // Start up the sensor
    return retVal;
}


String AOSongDHT::getSensorName(void)
{
    switch (_dhtType)
    {
        case 11: return "AOSongDHT11";
        case 21: return "AOSongDHT21";
        default: return "AOSongDHT22";
     }
}


bool AOSongDHT::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a new reading to be available
    waitForMeasurementCompletion();

    // Reading temperature or humidity takes about 250 milliseconds!
    float humid_val = -9999;
    float temp_val = -9999;
    float hi_val = -9999;
    for (uint8_t i = 0; i < 5; i++)  // Make 5 attempts to get a decent reading
    {
        // First read the humidity
        humid_val = dht_internal.readHumidity();
        // Read temperature as Celsius (the default)
        temp_val = dht_internal.readTemperature();
        // Check if any reads failed
        // If they are NaN (not a number) then something went wrong
        if (!isnan(humid_val) && !isnan(temp_val))
        {
            // Compute heat index in Celsius (isFahreheit = false)
            hi_val = dht_internal.computeHeatIndex(temp_val, humid_val, false);
            MS_DBG(F("Temp is: "), temp_val, F("°C"));
            MS_DBG(F(" Humidity is: "), humid_val, F("%"));
            MS_DBG(F(" Calculated Heat Index is: "), hi_val, F("°C\n"));
            break;
        }
        else
        {
            if (i < 4) {
                MS_DBG(F("Failed to read from DHT sensor, Retrying...\n"));
                delay(100);
            }
            else {
                MS_DBG(F("Failed to read from DHT sensor!\n"));
                if (isnan(humid_val)) humid_val = -9999;
                if (isnan(temp_val)) temp_val = -9999;
            }
        }
    }

    // Store the results in the sensorValues array
    verifyAndAddMeasurementResult(DHT_TEMP_VAR_NUM, temp_val);
    verifyAndAddMeasurementResult(DHT_HUMIDITY_VAR_NUM, humid_val);
    verifyAndAddMeasurementResult(DHT_HI_VAR_NUM, hi_val);

    // Mark that we've already recorded the result of the measurement
    _millisMeasurementRequested = 0;

    return true;
}
