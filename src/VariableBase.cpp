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

// The constructor for a measured variable - that is, one whose values are
// updated by a sensor.
Variable::Variable(const uint8_t sensorVarNum,
                   uint8_t decimalResolution,
                   const char *varName,
                   const char *varUnit,
                   const char *varCode,
                   const char *uuid,
                   Sensor *parentSense)
  : _sensorVarNum(sensorVarNum),
    _decimalResolution(decimalResolution),
    _varName(varName),
    _varUnit(varUnit),
    _varCode(varCode),
    _uuid(uuid)
{
    isCalculated = false;
    _calcFxn = NULL;
    attachSensor(parentSensor);

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;

    MS_DBG(F("Measured variable object created"));
}
Variable::Variable(const uint8_t sensorVarNum,
                   uint8_t decimalResolution,
                   const char *varName,
                   const char *varUnit,
                   const char *varCode,
                   Sensor *parentSense)
  : _sensorVarNum(sensorVarNum),
    _decimalResolution(decimalResolution),
    _varName(varName),
    _varUnit(varUnit),
    _varCode(varCode),
    _uuid("")
{
    isCalculated = false;
    _calcFxn = NULL;
    attachSensor(parentSensor);

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;

    MS_DBG(F("Measured variable object created"));
}
Variable::Variable(const uint8_t sensorVarNum,
                   uint8_t decimalResolution,
                   const char *varName,
                   const char *varUnit,
                   const char *varCode)
  : _sensorVarNum(sensorVarNum),
    _decimalResolution(decimalResolution),
    _varName(varName),
    _varUnit(varUnit),
    _varCode(varCode),
    _uuid("")
{
    isCalculated = false;
    _calcFxn = NULL;
    parentSensor = NULL;

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;

    MS_DBG(F("Measured variable object created"));
}

// The constructor for a calculated variable  - that is, one whose value is
// calculated by the calcFxn which returns a float.
Variable::Variable(uint8_t decimalResolution,
                   const char *varName,
                   const char *varUnit,
                   const char *varCode,
                   const char *uuid,
                   float (*calcFxn)())
  : _sensorVarNum(0),
    _decimalResolution(decimalResolution),
    _varName(varName),
    _varUnit(varUnit),
    _varCode(varCode),
    _uuid(uuid)
{
    isCalculated = true;
    setCalculation(calcFxn);
    parentSensor = NULL;

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;

    MS_DBG(F("Calculated variable object created"));
}
Variable::Variable(uint8_t decimalResolution,
                   const char *varName,
                   const char *varUnit,
                   const char *varCode,
                   float (*calcFxn)())
  : _sensorVarNum(0),
    _decimalResolution(decimalResolution),
    _varName(varName),
    _varUnit(varUnit),
    _varCode(varCode),
    _uuid("")
{
    isCalculated = true;
    setCalculation(calcFxn);
    parentSensor = NULL;

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;

    MS_DBG(F("Calculated variable object created"));
}
Variable::Variable()
  : _sensorVarNum(0),
    _decimalResolution(0),
    _varName(""),
    _varUnit(""),
    _varCode(""),
    _uuid("")
{
    isCalculated = true;
    _calcFxn = NULL;
    parentSensor = NULL;

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;

    MS_DBG(F("Calculated variable object created"));
}


// Destructor
Variable::~Variable(){}


// This does all of the setup that can't happen in the constructors
// That is, anything that is dependent on another object having been created
// first or anything that requires the actual processor/MCU to do something.
Variable *Variable::begin(Sensor *parentSense, const char *uuid,
                          const char *customVarCode)
{
    setVarCode(customVarCode);
    return begin(parentSense, uuid);
}
Variable *Variable::begin(Sensor *parentSense, const char *uuid)
{
    setVarUUID(uuid);
    return begin(parentSense);
}
Variable *Variable::begin(Sensor *parentSense)
{
    attachSensor(parentSense);
    return this;
}
Variable *Variable::begin(float (*calcFxn)(),
                          uint8_t decimalResolution,
                          const char *varName,
                          const char *varUnit,
                          const char *varCode,
                          const char *uuid)
{
    setVarUUID(uuid);
    return begin(calcFxn, decimalResolution, varName, varUnit, varCode);
}
Variable *Variable::begin(float (*calcFxn)(),
                          uint8_t decimalResolution,
                          const char *varName,
                          const char *varUnit,
                          const char *varCode)
{
    setCalculation(calcFxn);
    setVarCode(varCode);
    _calcFxn = calcFxn;
    _varName = varName;
    _varUnit = varUnit;
    _decimalResolution = decimalResolution;
    return this;
}


// This notifies the parent sensor that it has an observing variable
// This function should never be called for a calculated variable
void Variable::attachSensor(Sensor *parentSense)
{
    if (!isCalculated)
    {
        parentSensor = parentSense;
        MS_DBG(F("Attempting to register"), getVarName(), F("to"),
               parentSensor->getSensorName(), F("attached at"),
               parentSensor->getSensorLocation(), F("..."));
        parentSensor->registerVariable(_sensorVarNum, this);
    }
    else MS_DBG(F("This is a calculated variable.  It cannot have a parent sensor!"));
}


// This is the function called by the parent sensor's notifyVariables() function
// This function should never be called for a calculated variable
void Variable::onSensorUpdate(Sensor *parentSense)
{
    if (!isCalculated)
    {
        _currentValue = parentSensor->sensorValues[_sensorVarNum];
        MS_DBG(F("... received"), _currentValue);
    }
}


// This is a helper - it returns the name of the parent sensor, if applicable
// This is needed for dealing with variables in arrays
String Variable::getParentSensorName(void)
{
    if (!isCalculated) return parentSensor->getSensorName();
    else return "Calculated";
}


// This is a helper - it returns the name and location of the parent sensor, if applicable
// This is needed for dealing with variables in arrays
String Variable::getParentSensorNameAndLocation(void)
{
    if (!isCalculated) return parentSensor->getSensorNameAndLocation();
    else return "Calculated";
}


// This ties a calculated variable to its calculation function
void Variable::setCalculation(float (*calcFxn)())
{
    if (isCalculated)
    {
        _calcFxn = calcFxn;
    }
    else MS_DBG(F("This is a measured variable.  It cannot have a calculation function!"));
}


// This sets up the variable (generally attaching it to its parent)
// bool Variable::setup(void)
// {
//     if (!isCalculated) attachSensor(_varNum, parentSensor);
//     return true;
// }


// This gets/sets the variable's resolution for value strings
uint8_t Variable::getResolution(void){return _decimalResolution;}
void Variable::setResolution(uint8_t decimalResolution)
{_decimalResolution = decimalResolution;}

// This gets/sets the variable's name using http://vocabulary.odm2.org/variablename/
String Variable::getVarName(void){return _varName;}
void Variable::setVarName(const char *varName){_varName = varName;}

// This gets/sets the variable's unit using http://vocabulary.odm2.org/units/
String Variable::getVarUnit(void){return _varUnit;}
void Variable::setVarUnit(const char *varUnit){_varUnit = varUnit;}

// This returns a customized code for the variable
String Variable::getVarCode(void){return _varCode;}
// This sets the variable code to a new custom value
void Variable::setVarCode(const char *varCode){_varCode = varCode;}

// This returns the variable UUID, if one has been assigned
String Variable::getVarUUID(void){return _uuid;}
// This sets the UUID
void Variable::setVarUUID(const char *uuid){_uuid = uuid;}


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
        int16_t val = int(getValue(updateValue));
        return String(val);
    }
    else
    {return String(getValue(updateValue), _decimalResolution);}
}
