/*
 *VariableArray.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the variable array class.
*/

// Header Guards
#ifndef VariableArray_h
#define VariableArray_h

// Debugging Statement
// #define MS_VARIABLEARRAY_DEBUG
// #define MS_VARIABLEARRAY_DEBUG_DEEP

#ifdef MS_VARIABLEARRAY_DEBUG
#define MS_DEBUGGING_STD "VariableArray"
#endif

#ifdef MS_VARIABLEARRAY_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "VariableArray"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "VariableBase.h"
#include "SensorBase.h"

// Defines another class for interfacing with a list of pointers to sensor instances
class VariableArray
{
public:
    // Constructors
    VariableArray();
    VariableArray(uint8_t variableCount, Variable *variableList[]);
    ~VariableArray();

    // "Begins" the VariableArray - attaches the number and array of variables
    // Not doing this in the constructor because we expect the VariableArray to
    // be created in the "global scope" and we cannot control the order in which
    // objects in that global scope will be created.  That is, we cannot
    // guarantee that the variables and their pointers in the array will
    // actually have been created unless we wait until in the setup or loop
    // function of the main program.
    void begin(uint8_t variableCount, Variable *variableList[]);
    void begin();

    // Leave the internal variable list public
    Variable **arrayOfVars;

    // Functions to return information about the list

    // This just returns the number of variables (as input in the constructor)
    uint8_t getVariableCount(void){return _variableCount;}

    // This counts and returns the number of calculated variables
    uint8_t getCalculatedVariableCount(void);

    // This counts and returns the number of sensors
    uint8_t getSensorCount(void);

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

    // This function powers, wakes, updates values, sleeps and powers down.
    bool completeUpdate(void);

    // This function prints out the results for any connected sensors to a stream
    void printSensorData(Stream *stream = &Serial);

protected:
    uint8_t _variableCount;
    uint8_t _sensorCount;
    uint8_t _maxSamplestoAverage;

private:
    bool isLastVarFromSensor(int arrayIndex);
    uint8_t countMaxToAverage(void);
    bool checkVariableUUIDs(void);

#ifdef MS_VARIABLEARRAY_DEBUG_DEEP
    template<typename T>
    void prettyPrintArray(T arrayToPrint[])
    {
        DEEP_DEBUGGING_SERIAL_OUTPUT.print("[,\t");
        for (uint8_t i = 0; i < _variableCount; i++)
        {
            DEEP_DEBUGGING_SERIAL_OUTPUT.print(arrayToPrint[i]);
            DEEP_DEBUGGING_SERIAL_OUTPUT.print(",\t");
        }
        DEEP_DEBUGGING_SERIAL_OUTPUT.println("]");
    }
#else
    #define prettyPrintArray(...)
#endif  // DEEP_DEBUGGING_SERIAL_OUTPUT

};

#endif  // Header Guard
