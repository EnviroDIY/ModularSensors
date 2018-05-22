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
    // The constructor for a measured variable - that is, one whos values are
    // updated by a sensor.
    Variable(Sensor *parentSense, int varNum,
             String varName = "Unknown", String varUnit = "Unknown",
             unsigned int decimalResolution = 0,
             String defaultVarCode = "Unknown",
             String UUID = "", String customVarCode = "");

     // The constructor for a measured variable - that is, one whos value is
     // calculated by the calcFxn which returns a float.
     // NOTE:  ALL arguments are required!
     Variable(float (*calcFxn)(),
              String varName, String varUnit,
              unsigned int decimalResolution,
              String UUID, String customVarCode);

    // These functions tie the variable and sensor together
    // They should never be called for a calculated variable
    // This notifies the parent sensor that it has an observing variable
    void attachSensor(int varNum, Sensor *parentSense);
    // This is the function called by the parent sensor's notifyVariables() function
    virtual void onSensorUpdate(Sensor *parentSense);

    // This sets up the variable (generally attaching it to its parent)
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
    float getValue(bool updateValue = false);
    // This returns the current value of the variable as a string with the
    // correct number of significant figures
    String getValueString(bool updateValue = false);

    // This is the parent sensor for the variable
    Sensor *parentSensor;

protected:
    float _currentValue;

private:
    bool _isCalculated;
    float (*_calcFxn)(void);
    uint8_t _varNum;
    String _varName;
    String _varUnit;
    unsigned int _decimalResolution;
    String _defaultVarCode;
    String _customCode;
    String _UUID;
};

#endif
