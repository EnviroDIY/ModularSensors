/*
 *VariableBase.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the variable base class.
*/

#include "VariableBase.h"
#include "SensorBase.h"

// ============================================================================
//  The class and functions for interfacing with a specific variable.
// ============================================================================

// The constructor for a measured variable - that is, one whos values are
// updated by a sensor.
Variable::Variable(Sensor *parentSense, int varNum,
                   String varName, String varUnit,
                   unsigned int decimalResolution,
                   String defaultVarCode,
                   String UUID, String customVarCode)
{
    isCalculated = false;
    _calcFxn = NULL;
    parentSensor = parentSense;
    _varNum = varNum;
    _varName = varName;
    _varUnit = varUnit;
    _decimalResolution = decimalResolution;
    _defaultVarCode = defaultVarCode;
    _customCode = customVarCode;
    _UUID = UUID;

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;
}

// The constructor for a calculated variable  - that is, one whos value is
// calculated by the calcFxn which returns a float.
// NOTE:  ALL arguments are required!
Variable::Variable(float (*calcFxn)(),
                   String varName, String varUnit,
                   unsigned int decimalResolution,
                   String UUID, String customVarCode)
{
    isCalculated = true;
    _calcFxn = calcFxn;
    parentSensor = NULL;
    _varNum = 0;
    _varName = varName;
    _varUnit = varUnit;
    _decimalResolution = decimalResolution;
    _defaultVarCode = "";
    _customCode = customVarCode;
    _UUID = UUID;

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;
}


// This notifies the parent sensor that it has an observing variable
// This function should never be called for a calculated variable
void Variable::attachSensor(int varNum, Sensor *parentSense)
{
    if (!isCalculated)
    {
        MS_DBG(F("Attempting to register "), getVarName());
        MS_DBG(F(" to "), parentSense->getSensorName());
        MS_DBG(F(" attached at "), parentSense->getSensorLocation(), F("...   "));
        parentSense->registerVariable(varNum, this);
    }
}


// This is the function called by the parent sensor's notifyVariables() function
// This function should never be called for a calculated variable
void Variable::onSensorUpdate(Sensor *parentSense)
{
    if (!isCalculated)
    {
        _currentValue = parentSense->sensorValues[_varNum];
        MS_DBG(F("... received "), _currentValue, F("\n"));
    }
}


// This is a helper - it returns the name of the parent sensor, if applicable
// This is needed for dealing with variables in arrays
String Variable::getParentSensorLocation(void)
{
    if (!isCalculated) return parentSensor->getSensorLocation();
    else return "Calculated";
}


// This is a helper - it returns the name of the parent sensor, if applicable
// This is needed for dealing with variables in arrays
String Variable::getParentSensorName(void)
{
    if (!isCalculated) return parentSensor->getSensorName();
    else return "Calculated";
}


// This sets up the variable (generally attaching it to its parent)
bool Variable::setup(void)
{
    if (!isCalculated) attachSensor(_varNum, parentSensor);
    return true;
}

// This returns the variable's name using http://vocabulary.odm2.org/variablename/
String Variable::getVarName(void){return _varName;}

// This returns the variable's unit using http://vocabulary.odm2.org/units/
String Variable::getVarUnit(void){return _varUnit;}

// This returns a customized code for the variable, if one is given, and a default if not
String Variable::getVarCode(void)
{
    if (_customCode != "") return _customCode;
    else return _defaultVarCode;
}

// This returns the variable UUID, if one has been assigned
String Variable::getVarUUID(void) {return _UUID;}


// This returns the current value of the variable as a float
float Variable::getValue(bool updateValue)
{
    if (isCalculated)
    {
        // NOTE:  We cannot "update" the parent sensor's values before doing
        // the calculation because we don't know which sensors those are.
        // Make sure you update the parent sensors manually for a calculated
        // variable!!
        return _calcFxn();
    }
    else
    {
        if (updateValue) parentSensor->update();
        return _currentValue;
    }
}


// This returns the current value of the variable as a string
// with the correct number of significant figures
String Variable::getValueString(bool updateValue)
{
    // Need this because otherwise get extra spaces in strings from int
    if (_decimalResolution == 0)
    {
        int val = int(getValue(updateValue));
        return String(val);
    }
    else
    {return String(getValue(updateValue), _decimalResolution);}
}
