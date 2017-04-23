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


#define AM2315_NUM_MEASUREMENTS 2
#define AM2315_HUMIDITY_RESOLUTION 1
#define AM2315_TEMP_RESOLUTION 1


typedef enum AM2315Vars
{
  humidity = 0,
  temp
} AM2315Vars;


#define MAX_NUMBER_VARIABLES 5

class Variable;  // Forward declaration

class Sensor
{
public:
    virtual bool update(void) = 0;
    virtual String getSensorName(void) = 0;
    virtual String getSensorLocation(void) = 0;
    virtual void registerVariable(int varNum, Variable* var) = 0;
    float sensorValues[MAX_NUMBER_VARIABLES];

protected:
    unsigned long sensorLastUpdated;
    Variable *variables[MAX_NUMBER_VARIABLES];
};

class Variable
{
public:
    void attachSensor(int varNum, Sensor *parentSense);
    virtual void onSensorUpdate(Sensor *parentSense) = 0;
    virtual void setup(void);
    float getValue(void);
    Sensor *parentSensor;
protected:
    float sensorValue;
};


// The main class for the AOSong AM2315
class AOSongAM2315 : public virtual Sensor
{
public:
    AOSongAM2315(void);

    String getSensorLocation(void) override;
    String getSensorName(void) override;

    void registerVariable(int varNum, Variable* var) override;

    bool update(void) override;
};


// Defines the "Humidity Sensor"
class AOSongAM2315_Humidity : public virtual Variable
{
public:
    AOSongAM2315_Humidity(Sensor *parentSense);
    void setup(void) override;
    void onSensorUpdate(Sensor *parentSense) override;
};


// Defines the "Temperature Sensor"
class AOSongAM2315_Temp : public virtual Variable
{
public:
    AOSongAM2315_Temp(Sensor *parentSense);
    void setup(void) override;
    void onSensorUpdate(Sensor *parentSense) override;
};

#endif
