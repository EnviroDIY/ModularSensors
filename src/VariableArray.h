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

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"

// Defines another class for interfacing with a list of pointers to sensor instances
class VariableArray
{
public:
    // Initialization - cannot do this in constructor arduino has issues creating
    // instances of classes with non-empty constructors
    virtual void init(int variableCount, Variable *variableList[]);

    // Functions to return information about the list

    // This just returns the number of variables
    int getVariableCount(void){return _variableCount;}

    // This counts and returns the number of sensors
    int getSensorCount(void);

    // Public functions for interfacing with a list of sensors
    // This sets up all of the sensors in the list
    bool setupSensors(void);

    // This gives power to each sensor
    void sensorsPowerUp(void);

    // This verifies sensors have power and sends a wake command, if necesary
    bool sensorsWake(void);

    // This sends sensors a sleep command, but does not power them down
    bool sensorsSleep(void);

    // This cuts sensor power
    void sensorsPowerDown(void);

    // This function updates the values for any connected sensors.
    bool updateAllSensors(void);

    // This function prints out the results for any connected sensors to a stream
    void printSensorData(Stream *stream = &Serial);

    // This generates a comma separated list of sensor values WITHOUT TIME STAMP
    String generateSensorDataCSV(void);

protected:
    uint8_t _variableCount;
    uint8_t _sensorCount;
    Variable **_variableList;
    uint8_t _maxSamplestoAverage;

private:
    bool isLastVarFromSensor(int arrayIndex);
    uint8_t countMaxToAverage(void);
};

#endif
