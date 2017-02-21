/*
 *Decagon5TM.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Decagon Devices 5TM Soild Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library.
*/

#ifndef Decagon5TM_h
#define Decagon5TM_h

#include <Arduino.h>
#include "SensorBase.h"

// The main class for the Decagon 5TM
class Decagon5TM : public virtual SensorBase
{
public:
    Decagon5TM(char TMaddress, int powerPin, int dataPin);
    SENSOR_STATUS setup(void) override;

    bool update(void) override;
    String getSensorName(void) override;
    String getSensorLocation(void) override;
    bool sleep(void) override;
    bool wake(void) override;

    virtual String getVarName(void) = 0;
    virtual String getVarUnit(void) = 0;
    virtual float getValue(void) = 0;
    virtual String getDreamHost(void) = 0;
protected:
    SENSOR_STATUS sensorStatus;
    String sensorName;
    String sensorLocation;
    String varName;
    String unit;
    char _TMaddress;
    int _powerPin;
    int _dataPin;
    static float sensorValue_Ea;
    static float sensorValue_temp;
    static float sensorValue_VWC;
};


// Defines the "Ea/Matric Potential Sensor"
class Decagon5TM_Ea : public virtual Decagon5TM
{
public:
    Decagon5TM_Ea(char TMaddress, int powerPin, int dataPin);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
};


// Defines the "Temperature Sensor"
class Decagon5TM_Temp : public virtual Decagon5TM
{
public:
    Decagon5TM_Temp(char TMaddress, int powerPin, int dataPin);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
};


// Defines the "Volumetric Water Content Sensor"
class Decagon5TM_VWC : public virtual Decagon5TM
{
public:
    Decagon5TM_VWC(char TMaddress, int powerPin, int dataPin);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
};

#endif
