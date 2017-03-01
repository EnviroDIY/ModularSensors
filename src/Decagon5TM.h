/*
 *Decagon5TM.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Decagon Devices 5TM Soild Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library and the DecagonSDI12 super class.
*/

#ifndef Decagon5TM_h
#define Decagon5TM_h

#include "DecagonSDI12.h"

// The main class for the Decagon 5TM
class Decagon5TM : public virtual DecagonSDI12
{
public:
    Decagon5TM(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    bool update(void) override;

    virtual String getVarName(void) = 0;
    virtual String getVarUnit(void) = 0;
    virtual float getValue(void) = 0;
    virtual String getDreamHost(void) = 0;
protected:
    SENSOR_STATUS sensorStatus;
    String sensorName;
    String sensorLocation;
    static int numMeasurements;
    static unsigned long sensorLastUpdated;
    static float sensorValues[];
};


// Defines the "Ea/Matric Potential Sensor"
class Decagon5TM_Ea : public virtual Decagon5TM
{
public:
    Decagon5TM_Ea(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
private:
    String varName;
    String unit;
};


// Defines the "Temperature Sensor"
class Decagon5TM_Temp : public virtual Decagon5TM
{
public:
    Decagon5TM_Temp(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
private:
    String varName;
    String unit;
};


// Defines the "Volumetric Water Content Sensor"
class Decagon5TM_VWC : public virtual Decagon5TM
{
public:
    Decagon5TM_VWC(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
private:
    String varName;
    String unit;
    float ea;
    float sensorValue_VWC;
};

#endif
