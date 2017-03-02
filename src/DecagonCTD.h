/*
 *DecagonCTD.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Decagon Devices CTD-10
 *It is dependent on the EnviroDIY SDI-12 library and the DecagonSDI12 super class.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon CTD-10 can be found at:
 * http://manuals.decagon.com/Integration%20Guides/CTD%20Integrators%20Guide.pdf
*/

#ifndef DecagonCTD_h
#define DecagonCTD_h

#include "DecagonSDI12.h"

// The main class for the Decagon CTD
class DecagonCTD : public virtual DecagonSDI12
{
public:
    DecagonCTD(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

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
    static float sensorValue_depth;
    static float sensorValue_temp;
    static float sensorValue_cond;
};


// Defines the "Depth Sensor"
class DecagonCTD_Depth : public virtual DecagonCTD
{
public:
    DecagonCTD_Depth(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
private:
    String varName;
    String unit;
};


// Defines the "Temperature Sensor"
class DecagonCTD_Temp : public virtual DecagonCTD
{
public:
    DecagonCTD_Temp(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
private:
    String varName;
    String unit;
};


// Defines the "Conductivity Sensor"
class DecagonCTD_Cond : public virtual DecagonCTD
{
public:
    DecagonCTD_Cond(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
private:
    String varName;
    String unit;
};

#endif
