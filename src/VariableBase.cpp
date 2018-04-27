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

// The constructor
Variable::Variable(Sensor *parentSense, int varNum,
                   String varName, String varUnit,
                   unsigned int decimalResolution,
                   String defaultVarCode,
                   String UUID, String customVarCode)
{
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

void Variable::attachSensor(int varNum, Sensor *parentSense) {
    MS_DBG(F("Attempting to register "), getVarName());
    MS_DBG(F(" to "), parentSense->getSensorName());
    MS_DBG(F(" attached at "), parentSense->getSensorLocation(), F("...   "));
    parentSense->registerVariable(varNum, this);
}

bool Variable::setup(void)
{
    attachSensor(_varNum, parentSensor);
    return true;
}

void Variable::onSensorUpdate(Sensor *parentSense)
{
    _currentValue = parentSense->sensorValues[_varNum];
    MS_DBG(F("... received "), sensorValue, F("\n"));
}

String Variable::getVarUUID(void) {return _UUID;}

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

// This returns the current value of the variable as a float
float Variable::getValue(bool updateValue)
{
    if (updateValue) parentSensor->update();
    return _currentValue;
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
