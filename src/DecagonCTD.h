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

#ifndef _DECAGONCTD_h
#define _DECAGONCTD_h
#include "Arduino.h"

#include "Sensor.h"

// The main class for the Mayfly
class DecagonCTD : public virtual SensorBase
{
public:
    DecagonCTD(char CTDaddress, int numReadings, int dataPin);
    virtual ~DecagonCTD(void);

    bool update(void) override;
    String getSensorName(void) override;

    virtual String getVarName(void) = 0;
    virtual String getVarUnit(void) = 0;
    virtual float getValue(void) = 0;
protected:
    SENSOR_STATUS sensorStatus;
    String sensorName;
    String varName;
    String unit;
    char _CTDaddress;
    int _numReadings;
    int _dataPin;
    float sensorValue_cond;
    float sensorValue_temp;
    float sensorValue_depth;
};


// Defines the "Conductivity Sensor"
class DecagonCTD_Cond : public virtual DecagonCTD
{
public:
    DecagonCTD_Cond(void);
    ~DecagonCTD_Cond(void);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
};


// Defines the "Temperature Sensor"
class DecagonCTD_Temp : public virtual DecagonCTD
{
public:
    DecagonCTD_Temp(void);
    ~DecagonCTD_Temp(void);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
};


// Defines the "Depth Sensor"
class DecagonCTD_Depth : public virtual DecagonCTD
{
public:
    DecagonCTD_Depth(void);
    ~DecagonCTD_Depth(void);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
};

#endif
