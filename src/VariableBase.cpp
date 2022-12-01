/**
 * @file VariableBase.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the Variable class.
 */

#include "VariableBase.h"
#include "SensorBase.h"

// ============================================================================
//  The class and functions for interfacing with a specific variable.
// ============================================================================

// The constructor for a measured variable - that is, one whose values are
// updated by a sensor.
Variable::Variable(Sensor* parentSense, const uint8_t sensorVarNum,
                   uint8_t decimalResolution, const char* varName,
                   const char* varUnit, const char* varCode, const char* uuid)
    : _sensorVarNum(sensorVarNum) {
    setVarUUID(uuid);
    setVarCode(varCode);
    setVarUnit(varUnit);
    setVarName(varName);
    setResolution(decimalResolution);

    attachSensor(parentSense);
}
Variable::Variable(const uint8_t sensorVarNum, uint8_t decimalResolution,
                   const char* varName, const char* varUnit,
                   const char* varCode)
    : _sensorVarNum(sensorVarNum) {
    setVarCode(varCode);
    setVarUnit(varUnit);
    setVarName(varName);
    setResolution(decimalResolution);
}


// The constructor for a calculated variable  - that is, one whose value is
// calculated by the calcFxn which returns a float.
Variable::Variable(float (*calcFxn)(), uint8_t decimalResolution,
                   const char* varName, const char* varUnit,
                   const char* varCode, const char* uuid)
    : isCalculated(true) {
    setVarUUID(uuid);
    setVarCode(varCode);
    setVarUnit(varUnit);
    setVarName(varName);
    setResolution(decimalResolution);

    setCalculation(calcFxn);
}
Variable::Variable(float (*calcFxn)(), uint8_t decimalResolution,
                   const char* varName, const char* varUnit,
                   const char* varCode)
    : isCalculated(true) {
    setVarCode(varCode);
    setVarUnit(varUnit);
    setVarName(varName);
    setResolution(decimalResolution);

    setCalculation(calcFxn);
}

// constructor with no arguments
Variable::Variable() : isCalculated(true) {}
// Destructor
Variable::~Variable() {}


// This does all of the setup that can't happen in the constructors
// That is, anything that depends on another object having been created
// first or anything that requires the actual processor/MCU to do something.
Variable* Variable::begin(Sensor* parentSense, const char* uuid,
                          const char* customVarCode) {
    setVarCode(customVarCode);
    return begin(parentSense, uuid);
}
Variable* Variable::begin(Sensor* parentSense, const char* uuid) {
    setVarUUID(uuid);
    return begin(parentSense);
}
Variable* Variable::begin(Sensor* parentSense) {
    attachSensor(parentSense);
    return this;
}


// Begin functions for calculated variables
Variable* Variable::begin(float (*calcFxn)(), uint8_t decimalResolution,
                          const char* varName, const char* varUnit,
                          const char* varCode, const char* uuid) {
    setVarUUID(uuid);
    return begin(calcFxn, decimalResolution, varName, varUnit, varCode);
}
Variable* Variable::begin(float (*calcFxn)(), uint8_t decimalResolution,
                          const char* varName, const char* varUnit,
                          const char* varCode) {
    setVarCode(varCode);
    setVarUnit(varUnit);
    setVarName(varName);
    setResolution(decimalResolution);
    setCalculation(calcFxn);
    return this;
}


// This notifies the parent sensor that it has an observing variable
// This function should never be called for a calculated variable
void Variable::attachSensor(Sensor* parentSense) {
    if (!isCalculated) {
        parentSensor = parentSense;
        parentSensor->registerVariable(_sensorVarNum, this);
    }
}


// This is the function called by the parent sensor's notifyVariables() function
// This function should never be called for a calculated variable
void Variable::onSensorUpdate(Sensor* parentSense) {
    if (!isCalculated) {
        _currentValue = parentSense->sensorValues[_sensorVarNum];
        MS_DBG(F("... received"), _currentValue);
    }
}


// This is a helper - it returns the name of the parent sensor, if applicable
// This is needed for dealing with variables in arrays
String Variable::getParentSensorName(void) {
    if (isCalculated) {
        return "Calculated";
    } else if (parentSensor == nullptr) {
        MS_DBG(F("ERROR! This variable is missing a parent sensor!"));
        return "";
    } else {
        return parentSensor->getSensorName();
    }
}


// This is a helper - it returns the name and location of the parent sensor, if
// applicable This is needed for dealing with variables in arrays
String Variable::getParentSensorNameAndLocation(void) {
    if (isCalculated) {
        return "Calculated";
    } else if (parentSensor == nullptr) {
        MS_DBG(F("ERROR! This variable is missing a parent sensor!"));
        return "";
    } else {
        return parentSensor->getSensorNameAndLocation();
    }
}


// This ties a calculated variable to its calculation function
void Variable::setCalculation(float (*calcFxn)()) {
    if (isCalculated) { _calcFxn = calcFxn; }
}


// This gets/sets the variable's resolution for value strings
uint8_t Variable::getResolution(void) {
    return _decimalResolution;
}
void Variable::setResolution(uint8_t decimalResolution) {
    _decimalResolution = decimalResolution;
}

// This gets/sets the variable's name using
// http://vocabulary.odm2.org/variablename/
String Variable::getVarName(void) {
    return _varName;
}
void Variable::setVarName(const char* varName) {
    _varName = varName;
}

// This gets/sets the variable's unit using http://vocabulary.odm2.org/units/
String Variable::getVarUnit(void) {
    return _varUnit;
}
void Variable::setVarUnit(const char* varUnit) {
    _varUnit = varUnit;
}

// This returns a customized code for the variable
String Variable::getVarCode(void) {
    return _varCode;
}
// This sets the variable code to a new custom value
void Variable::setVarCode(const char* varCode) {
    _varCode = varCode;
}

// This returns the variable UUID, if one has been assigned
String Variable::getVarUUID(void) {
    return _uuid;
}
// This sets the UUID
void Variable::setVarUUID(const char* uuid) {
    _uuid = uuid;
}
// This checks that the UUID is properly formatted
bool Variable::checkUUIDFormat(void) {
    // If no UUID, move on
    if (strlen(_uuid) == 0) { return true; }

    // Should be 36 characters long with dashes
    if (strlen(_uuid) != 36) {
        MS_DBG(F("UUID length for"), getVarCode(), '(', _uuid, ')',
               F("is incorrect, should be 36 characters not"), strlen(_uuid));
        return false;
    }

    // "12345678-abcd-1234-ef00-1234567890ab"
    const char* acceptableChars = "0123456789abcdefABCDEF-";
    if (_uuid[8] != '-' || _uuid[13] != '-' || _uuid[18] != '-' ||
        _uuid[23] != '-') {
        MS_DBG(F("UUID format for"), getVarCode(), '(', _uuid, ')',
               F("is incorrect, expecting dashes at positions 9, 14, 19, and "
                 "24."));
        return false;
    }
    int first_invalid = strspn(_uuid, acceptableChars);
    if (first_invalid != 36) {
        MS_DBG(F("UUID for"), getVarCode(), '(', _uuid, ')',
               F("has a bad character"), _uuid[first_invalid], F("at"), first_invalid);
        return false;
    }
    return true;
}


// This returns the current value of the variable as a float
float Variable::getValue(bool updateValue) {
    if (isCalculated) {
        // NOTE:  We cannot "update" the parent sensor's values before doing
        // the calculation because we don't know which sensors those are.
        // Make sure you update the parent sensors manually for a calculated
        // variable!!
        return _calcFxn();
    } else {
        if (updateValue) parentSensor->update();
        return _currentValue;
    }
}


// This returns the current value of the variable as a string
// with the correct number of significant figures
String Variable::getValueString(bool updateValue) {
    // Need this because otherwise get extra spaces in strings from int
    if (_decimalResolution == 0) {
        auto val = static_cast<int16_t>(getValue(updateValue));
        return String(val);
    } else {
        return String(getValue(updateValue), _decimalResolution);
    }
}
