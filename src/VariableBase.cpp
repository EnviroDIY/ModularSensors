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
Variable::Variable(Sensor *parentSense, int varNum, String varName, String varUnit,
                   unsigned int decimalResolution, String dreamHost,
                   String customVarCode)
{
    parentSensor = parentSense;
    _varNum = varNum;
    _varName = varName;
    _varUnit = varUnit;
    _decimalResolution = decimalResolution;
    _dreamHost = dreamHost;
    _customCode = customVarCode;
}

void Variable::attachSensor(int varNum, Sensor *parentSense) {
    DBGV(F("Attempting to register to "));
    DBGV(parentSense->getSensorName());
    DBGV(F(" attached at "));
    DBGV(parentSense->getSensorLocation());
    DBGV(F("...   "));
    parentSense->registerVariable(varNum, this);
}

bool Variable::setup(void)
{
    attachSensor(_varNum, parentSensor);
    return true;
}

void Variable::onSensorUpdate(Sensor *parentSense)
{
    sensorValue = parentSense->sensorValues[_varNum];
    DBGV(F("... received "));
    DBGV(sensorValue, F("\n"));
}

// This returns the variable's name using http://vocabulary.odm2.org/variablename/
String Variable::getVarName(void){return _varName;}

// This returns the variable's unit using http://vocabulary.odm2.org/units/
String Variable::getVarUnit(void){return _varUnit;}

// This returns the dreamhost PHP tag - for old SWRC dreamhost system
String Variable::getDreamHost(void){return _dreamHost;}

// This returns a customized code for the variable, if one is given
String Variable::getVarCode(void)
{
    if (_customCode != "") return _customCode;
    else return _dreamHost;
}

// This returns the current value of the variable as a float
float Variable::getValue(void)
{
    parentSensor->checkForUpdate(parentSensor->sensorLastUpdated);
    return sensorValue;
}

// This returns the current value of the variable as a string
// with the correct number of significant figures
String Variable::getValueString(void)
{
    // Need this because otherwise get extra spaces in strings from int
    if (_decimalResolution == 0)
    {
        int val = int(getValue());
        return String(val);
    }
    else
    {return String(getValue(), _decimalResolution);}
}
