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

const char* VAR_BASE_UNKNOWN = "Unknown";

class Sensor;  // Forward declaration

class Variable
{
public:
    // The constructor for a measured variable - that is, one whose values are
    // updated by a sensor.
    Variable(Sensor *parentSense, int varNum,
             String varName = VAR_BASE_UNKNOWN, String varUnit = VAR_BASE_UNKNOWN,
             unsigned int decimalResolution = 0,
             String defaultVarCode = VAR_BASE_UNKNOWN,
             const char *UUID = "", const char *customVarCode = "");

     // The constructor for a measured variable - that is, one whose value is
     // calculated by the calcFxn which returns a float.
     // NOTE:  ALL arguments are required!
     Variable(float (*calcFxn)(),
              String& varName, String& varUnit,
              unsigned int decimalResolution,
              const char *UUID, const char *customVarCode);

    // These functions tie the variable and sensor together
    // They should never be called for a calculated variable
    // This notifies the parent sensor that it has an observing variable
    void attachSensor(int varNum, Sensor *parentSense);
    // This is the function called by the parent sensor's notifyVariables() function
    virtual void onSensorUpdate(Sensor *parentSense);
    // This is a helper - it returns the name of the parent sensor, if applicable
    // This is needed for dealing with variables in arrays
    String getParentSensorName(void);
    // This is needed for dealing with variables in arrays
    // This is a helper - it returns the "location" of the parent sensor, if applicable
    String getParentSensorLocation(void);

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
    bool isCalculated;

protected:
    float _currentValue;

private:
    float (*_calcFxn)(void);
    uint8_t _varNum;
    String _varName;
    String _varUnit;
    unsigned int _decimalResolution;
    String _defaultVarCode;
    const char *_customCode;
    const char *_UUID;
};

#endif
