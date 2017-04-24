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

// Define the largest number of variables from a single sensor
#define MAX_NUMBER_VARIABLES 5

typedef enum SENSOR_STATUS
{
    SENSOR_ERROR,
    SENSOR_READY,
    SENSOR_WAITING,
    SENSOR_UNKNOWN
} SENSOR_STATUS;


class Variable;  // Forward declaration

// Defines the "Sensor" Class
class Sensor
{
public:

    Sensor(int dataPin = -1, int powerPin = -1, String sensorName = "Unknown");

    // These functions are dependent on the constructor and return the constructor values
    // This gets the place the sensor is installed ON THE MAYFLY (ie, pin number)
    virtual String getSensorLocation(void);
    // This gets the name of the sensor.
    virtual String getSensorName(void);

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

    // These tie the variables to their parent sensor
    virtual void registerVariable(int varNum, Variable* var);
    virtual void notifyVariables(void);
    float sensorValues[MAX_NUMBER_VARIABLES];

    // This just makes sure things are up-to-date
    bool checkForUpdate(unsigned long sensorLastUpdated);
    unsigned long sensorLastUpdated;

protected:
    bool checkPowerOn(void);
    void powerUp(void);
    void powerDown(void);
    int _dataPin;
    int _powerPin;
    SENSOR_STATUS sensorStatus;
    Variable *variables[MAX_NUMBER_VARIABLES];

private:
    String _sensorName;
};


class Variable
{
public:
    Variable(Sensor *parentSense, int varNum, String varName = "Unknown", String varUnit = "Unknown",
             unsigned int decimalResolution = 0, String dreamHost = "Unknown");

    // These functions tie the variable and sensor together
    void attachSensor(int varNum, Sensor *parentSense);
    virtual void onSensorUpdate(Sensor *parentSense);

    // This sets up the sensor (generally attaching it to it's parent)
    virtual bool setup(void);

    // This gets the variable's name using http://vocabulary.odm2.org/variablename/
    String getVarName(void);
    // This gets the variable's unit using http://vocabulary.odm2.org/units/
    String getVarUnit(void);
    // This returns the dreamhost PHP tag - for old SWRC dreamhost system
    String getDreamHost(void);

    // This returns the current value of the variable as a float
    float getValue(void);
    // This returns the current value of the variable as a string with the correct number of significant figures
    String getValueString(void);

    // This is the parent sensor for the variable
    Sensor *parentSensor;

protected:
    float sensorValue;

private:
    int _varNum;
    String _varName;
    String _varUnit;
    unsigned int _decimalResolution;
    String _dreamHost;
};


// Defines another class for interfacing with a list of pointers to sensor instances
class VariableArray
{
public:
    virtual void init(int variableCount, Variable *variableList[]);

    // Functions to return information about the list
    int getVariableCount(void);
    void findUniqueSensors(void);
    int getSensorCount(void);

    // Public functions for interfacing with a list of sensors
    bool setupSensors(void);  // This sets up all of the sensors in the list
    bool sensorsSleep(void);  // This puts sensors to sleep (ie, cuts power)
    bool sensorsWake(void);  // This wakes sensors (ie, gives power)
    bool updateAllSensors(void);  // This updates all sensor values
    void printSensorData(Stream *stream = &Serial);  // This prints the sensor info
    virtual String generateSensorDataCSV(void);  // This generates a comma separated list of values

protected:
    uint8_t _variableCount;
    Variable **_variableList;
    bool isUniqueSensor[];
};

#endif
