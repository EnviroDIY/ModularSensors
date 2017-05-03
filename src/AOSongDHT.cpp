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
*/

#include "AOSongDHT.h"
#include <DHT.h>

// The constructor - because this is I2C, only need the power pin
AOSongDHT::AOSongDHT(int powerPin, int dataPin, DHTtype type)
: Sensor(dataPin, powerPin, F("AOSongDHT"), DHT_NUM_MEASUREMENTS)
{
    _DHTtype = type;
}

bool AOSongDHT::update(void)
{
    DHT dht(_dataPin, _DHTtype);  // create a sensor object

    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}

    // Clear values before starting loop
    clearValues();

    // Start up the sensor
    dht.begin();

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float temp_val, humid_val, hi_val;
    temp_val = dht.readTemperature(false, true);  // Get in °C, force reading if necessary
    humid_val = dht.readHumidity(true);  // Force reading if necessary

    // check if returns are valid, if they are NaN (not a number) then something went wrong
    if (isnan(temp_val) || isnan(humid_val))
    {
            DBGM(F("Failed to read from DHT!\n"));
    }
    else
    {
        hi_val = dht.computeHeatIndex(temp_val, humid_val, false);  // Get in °C
        sensorValues[DHT_TEMP_VAR_NUM] = temp_val;
        sensorValues[DHT_HUMIDITY_VAR_NUM] = humid_val;
        sensorValues[DHT_HI_VAR_NUM] = hi_val;

        DBGM(F("Temp is: "), sensorValues[DHT_TEMP_VAR_NUM], F("°C"));
        DBGM(F(" Humidity is: "), sensorValues[DHT_HUMIDITY_VAR_NUM], F("%"));
        DBGM(F(" Calculated Heat Index is: "), sensorValues[DHT_HI_VAR_NUM], F("°C\n"));
    }

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Update the registered variables with the new values
    notifyVariables();

    return true;
}
