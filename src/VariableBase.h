/*
 *VariableBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the variable base class.
*/

#ifndef VariableBase_h
#define VariableBase_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

class Sensor;  // Forward declaration

class Variable
{
public:
    Variable(Sensor *parentSense, int varNum,
             String varName = "Unknown", String varUnit = "Unknown",
             unsigned int decimalResolution = 0,
             String defaultVarCode = "Unknown",
             String UUID = "", String customVarCode = "");

    // These functions tie the variable and sensor together
    void attachSensor(int varNum, Sensor *parentSense);
    virtual void onSensorUpdate(Sensor *parentSense);

    // This sets up the sensor (generally attaching it to it's parent)
    virtual bool setup(void);

    // This gets the variable's name using http://vocabulary.odm2.org/variablename/
    String getVarName(void);
    // This gets the variable's unit using http://vocabulary.odm2.org/units/
    String getVarUnit(void);
    // This returns a customized code for the variable, if one is given, and a default if not
    String getVarCode(void);
    // This returns the variable UUID, if one has been assigned
    String getVarUUID(void);

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
    String _defaultVarCode;
    String _customCode;
    String _UUID;
};

#endif
