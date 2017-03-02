/*
 *SensorBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the sensor base class.
*/

#ifndef SensorBase_h
#define SensorBase_h


#include <Arduino.h>
#include <pins_arduino.h>

enum SENSOR_STATUS
{
    SENSOR_ERROR,
    SENSOR_READY,
    SENSOR_WAITING,
    SENSOR_UNKNOWN
};


// Defines the "SensorBase" Class
class SensorBase
{
public:
    SensorBase(int dataPin = -1, int powerPin = -1,
               String sensorName = "Unknown", String varName = "Unknown",
               String varUnit = "Unknown", String dreamHost = "Unknown");

    // These functions are dependent on the constructor and return the constructor values
    // This gets the place the sensor is installed ON THE MAYFLY (ie, pin number)
    virtual String getSensorLocation(void);
    // This gets the name of the sensor.
    virtual String getSensorName(void);
    // This gets the variable's name using http://vocabulary.odm2.org/variablename/
    String getVarName(void);
    // This gets the variable's unit using http://vocabulary.odm2.org/units/
    String getVarUnit(void);
    // This returns the dreamhost PHP tag - for old SWRC dreamhost system
    String getDreamHost(void);

    // These next functions have defaults.
    // This sets up the sensor, if necessary.  Defaults to ready.
    virtual SENSOR_STATUS setup(void);
    // This returns the current status of the sensor.  Defaults to ready.
    virtual SENSOR_STATUS getStatus(void);
    // This puts the sensor to sleep, if necessary.  Defaults to True.
    virtual bool sleep(void);
    // This wakes the sensor up, if necessary.  Defaults to True.
    virtual bool wake(void);

    // These next functions must be implemented for ever sensor
    // This updates the sensor's values
    virtual bool update(void) = 0;

    // These next functions must be implemented for ever variable
    // This returns the current value of the variable as a float
    virtual float getValue(void) = 0;
protected:
    bool checkForUpdate(unsigned long sensorLastUpdated);
    bool checkPowerOn(void);
    void powerUp(void);
    void powerDown(void);
    int _dataPin;
    int _powerPin;
    SENSOR_STATUS sensorStatus;
private:
    String _sensorName;
    String _varName;
    String _varUnit;
    String _dreamHost;
};

#endif
