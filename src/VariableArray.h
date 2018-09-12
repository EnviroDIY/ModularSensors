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
    // Constructor
    VariableArray(int variableCount, Variable *variableList[]);

    // Leave the internal variable list public
    Variable **arrayOfVars;

    // Functions to return information about the list

    // This just returns the number of variables (as input in the constructor)
    int getVariableCount(void){return _variableCount;}

    // This counts and returns the number of calculated variables
    int getCalculatedVariableCount(void);

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

    // These generate some helpful comma-separated lists of variable information
    // This generates a comma separated list of sensor values WITHOUT TIME STAMP
    // String generateSensorDataCSV(void);
    // This generates a comma separated list of parent sensor names
    // String listParentSensorNames(void);
    // This generates a comma separated list of variable names
    // String listVariableNames(void);
    // This generates a comma separated list of variable units
    // String listVariableUnits(void);
    // This generates a comma separated list of variable codes
    // String listVariableCodes(void);
    // This generates a comma separated list of variable UUID's
    // String listVariableUUIDs(void);
    // These are identical to the above, except they directly send the data to
    // an arduino stream to avoid passing around long strings.
    // void streamSensorDataCSV(Stream *stream);
    // void streamParentSensorNames(Stream *stream);
    // void streamVariableNames(Stream *stream);
    // void streamVariableUnits(Stream *stream);
    // void streamVariableCodes(Stream *stream);
    // void streamVariableUUIDs(Stream *stream);

protected:
    uint8_t _variableCount;
    uint8_t _sensorCount;
    uint8_t _maxSamplestoAverage;

private:
    bool isLastVarFromSensor(int arrayIndex);
    uint8_t countMaxToAverage(void);
};

#endif
