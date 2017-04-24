/*
 *AOSongAM2315.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the AOSong AM2315 Capacitive Humidity and Temperature sensor
 *It is dependent on the Adafruit AM2315 Library
 *
 *Documentation for the sensor can be found at: www.aosong.com/asp_bin/Products/en/AM2315.pdf
 *
 * For Relative Humidity:
 *  Resolution is 0.1 % RH (16 bit)
 *  Accuracy is ± 2 % RH
 *
 * For Temperature:
 *  Resolution is 0.1°C (16 bit)
 *  Accuracy is ±0.1°C
 *  Range is -40°C to +125°C
*/

#ifndef AOSongAM2315_h
#define AOSongAM2315_h

#include "SensorBase.h"
#include <Adafruit_AM2315.h>


#define AM2315_NUM_MEASUREMENTS 2
#define AM2315_HUMIDITY_RESOLUTION 1
#define AM2315_TEMP_RESOLUTION 1


// The main class for the AOSong AM2315
class AOSongAM2315 : public virtual Sensor
{
public:
    // The constructor - because this is I2C, only need the power pin
    AOSongAM2315(int powerPin) : Sensor(-1, powerPin){}

    String getSensorLocation(void) override {return F("I2C_0xB8");}

    bool update(void) override
    {
        Adafruit_AM2315 am2315;  // create a sensor object
        Wire.begin();  // Start the wire library

        float temp_val, humid_val;
        bool ret_val = am2315.readTemperatureAndHumidity(temp_val, humid_val);
        sensorValues[temp] = temp_val;
        sensorValues[humidity] = humid_val;

        Serial.print(F("Temp is: "));  // for debugging
        Serial.print(sensorValues[temp]);  // for debugging
        Serial.print(F("°C and humidity is: "));  // for debugging
        Serial.print(sensorValues[humidity]);  // for debugging
        Serial.println(F("%"));  // for debugging

        notifyVariables();

        return ret_val;
    }
};


// Defines the "Humidity Sensor"
class AOSongAM2315_Humidity : public virtual Variable
{
public:
    AOSongAM2315_Humidity(Sensor *parentSense) :
      Variable(parentSense, humidity, F("relativeHumidity"), F("percent"),
               AM2315_HUMIDITY_RESOLUTION, F("AM2315Humidity"))
    {}
};


// Defines the "Temperature Sensor"
class AOSongAM2315_Temp : public virtual Variable
{
public:
    AOSongAM2315_Temp(Sensor *parentSense) :
      Variable(parentSense, temp, F("temperature"), F("degreeCelsius"),
               AM2315_TEMP_RESOLUTION, F("AM2315YTemp"))
    {}
};

#endif
