/*
 *DecagonCTD.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Decagon Devices CTD-10
 *It is dependent on the EnviroDIY SDI-12 library.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon CTD-10 can be found at:
 *http://manuals.decagon.com/Integration%20Guides/CTD%20Integrators%20Guide.pdf
*/

#ifndef DecagonCTD_h
#define DecagonCTD_h

#include <Arduino.h>
#include "SensorBase.h"

// The main class for the Mayfly
class DecagonCTD : public virtual SensorBase
{
public:
    DecagonCTD(char CTDaddress, int numReadings, int dataPin);

    bool update(void) override;
    String getSensorName(void) override;
    String getSensorLocation(void) override;

    virtual String getVarName(void) = 0;
    virtual String getVarUnit(void) = 0;
    virtual float getValue(void) = 0;
protected:
    SENSOR_STATUS sensorStatus;
    String sensorName;
    String sensorLocation;
    String varName;
    String unit;
    char _CTDaddress;
    int _numReadings;
    int _dataPin;
    static float sensorValue_cond;
    static float sensorValue_temp;
    static float sensorValue_depth;
};


// Defines the "Conductivity Sensor"
class DecagonCTD_Cond : public virtual DecagonCTD
{
public:
    using DecagonCTD::DecagonCTD;

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
};


// Defines the "Temperature Sensor"
class DecagonCTD_Temp : public virtual DecagonCTD
{
public:
    using DecagonCTD::DecagonCTD;

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
};


// Defines the "Depth Sensor"
class DecagonCTD_Depth : public virtual DecagonCTD
{
public:
    using DecagonCTD::DecagonCTD;

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
};

#endif
