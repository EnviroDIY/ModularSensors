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
AOSongDHT::AOSongDHT(int powerPin, int dataPin, DHTtype type)
: Sensor(powerPin, dataPin, F("AOSongDHT"), DHT_NUM_MEASUREMENTS, DHT_WARM_UP),
  dht_internal(dataPin, type)
{
    _dhtType = type;
}

SENSOR_STATUS AOSongDHT::setup(void)
{
    if (_powerPin > 0) pinMode(_powerPin, OUTPUT);
    pinMode(_dataPin, INPUT_PULLUP);

    // Start up the sensor
    dht_internal.begin();

    MS_DBG(F("Set up "), getSensorName(), F(" attached at "), getSensorLocation());
    MS_DBG(F(" which can return up to "), _numReturnedVars, F(" variable[s].\n"));

    return SENSOR_READY;
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

bool AOSongDHT::update(void)
{
    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    // Wait until the sensor is warmed up
    waitForWarmUp();

    // Clear values before starting loop
    clearValues();

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)

    float humid_val, temp_val, hi_val = 0;
    for (uint8_t i = 0; i < 5; i++)
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
                if (isnan(humid_val)) humid_val = 0;
                if (isnan(temp_val)) temp_val = 0;
            }
        }
    }

    // Store the results in the sensorValues array
    sensorValues[DHT_TEMP_VAR_NUM] = temp_val;
    sensorValues[DHT_HUMIDITY_VAR_NUM] = humid_val;
    sensorValues[DHT_HI_VAR_NUM] = hi_val;

    // MS_DBG(F("Temp is: "), sensorValues[DHT_TEMP_VAR_NUM], F("°C"));
    // MS_DBG(F(" Humidity is: "), sensorValues[DHT_HUMIDITY_VAR_NUM], F("%"));
    // MS_DBG(F(" Calculated Heat Index is: "), sensorValues[DHT_HI_VAR_NUM], F("°C\n"));

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Update the registered variables with the new values
    notifyVariables();

    return true;
}
