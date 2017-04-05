/*
 *SensorBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the sensor base class.
*/

#ifndef SensorBase_h
#define SensorBase_h

#include <Arduino.h>
#include <pins_arduino.h>

typedef enum SENSOR_STATUS
{
    SENSOR_ERROR,
    SENSOR_READY,
    SENSOR_WAITING,
    SENSOR_UNKNOWN
} SENSOR_STATUS;

// Defines the "SensorBase" Class
class SensorBase
{
public:

    SensorBase(int dataPin = -1, int powerPin = -1, unsigned int decimalResolution = 0,
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
    // This returns the current value of the variable as a string with the correct number of significant figures
    String getValueString(void);

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
    unsigned int _decimalResolution;
    String _sensorName;
    String _varName;
    String _varUnit;
    String _dreamHost;
};


// Defines another class for interfacing with a list of pointers to sensor instances
class SensorArray
{
public:
    virtual void init(int sensorCount, SensorBase *SENSOR_LIST[]);

    // Functions to return information about the list
    int getSensorCount(void);

    // Public functions for interfacing with a list of sensors
    bool setupSensors(void);  // This sets up all of the sensors in the list
    bool sensorsSleep(void);  // This puts sensors to sleep (ie, cuts power)
    bool sensorsWake(void);  // This wakes sensors (ie, gives power)
    bool updateAllSensors(void);  // This updates all sensor values
    void printSensorData(Stream *stream = &Serial);  // This prints the sensor info
    virtual String generateSensorDataCSV(void);  // This generates a comma separated list of values
protected:
    uint8_t _sensorCount;
    SensorBase **_sensorList;
};

#endif
