/*
 *VariableArray.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the variable array class.
*/

#ifndef VariableArray_h
#define VariableArray_h

#include "SensorBase.h"
#include "VariableBase.h"

// Defines another class for interfacing with a list of pointers to sensor instances
class VariableArray
{
public:
    virtual void init(int variableCount, Variable *variableList[]);

    // Functions to return information about the list
    int getVariableCount(void);
    int getSensorCount(void);

    // Public functions for interfacing with a list of sensors
    bool setupSensors(void);  // This sets up all of the sensors in the list
    bool sensorsSleep(void);  // This puts sensors to sleep (ie, cuts power)
    bool sensorsWake(void);  // This wakes sensors (ie, gives power)
    bool updateAllSensors(void);  // This updates all sensor values
    void printSensorData(Stream *stream = &Serial);  // This prints the sensor info
    virtual String generateSensorDataCSV(void);  // This generates a comma separated list of values

protected:
    bool isLastVarFromSensor(int arrayIndex);
    uint8_t _variableCount;
    Variable **_variableList;
};

#endif
