/**
 * @file VariableBase.cpp
 * @copyright 2020 Stroud Water Research Center
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

    isCalculated = false;
    _calcFxn     = NULL;
    attachSensor(parentSense);

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;

    // MS_DBG(F("Measured Variable object created"));
}
Variable::Variable(const uint8_t sensorVarNum, uint8_t decimalResolution,
                   const char* varName, const char* varUnit,
                   const char* varCode)
    : _sensorVarNum(sensorVarNum) {
    _uuid = NULL;
    setVarCode(varCode);
    setVarUnit(varUnit);
    setVarName(varName);
    setResolution(decimalResolution);

    isCalculated = false;
    _calcFxn     = NULL;
    parentSensor = NULL;

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;

    // MS_DBG(F("Measured Variable object created"));
}


// The constructor for a calculated variable  - that is, one whose value is
// calculated by the calcFxn which returns a float.
Variable::Variable(float (*calcFxn)(), uint8_t decimalResolution,
                   const char* varName, const char* varUnit,
                   const char* varCode, const char* uuid)
    : _sensorVarNum(0) {
    setVarUUID(uuid);
    setVarCode(varCode);
    setVarUnit(varUnit);
    setVarName(varName);
    setResolution(decimalResolution);

    isCalculated = true;
    setCalculation(calcFxn);
    parentSensor = NULL;

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;

    // MS_DBG(F("Calculated Variable object created"));
}
Variable::Variable(float (*calcFxn)(), uint8_t decimalResolution,
                   const char* varName, const char* varUnit,
                   const char* varCode)
    : _sensorVarNum(0) {
    _uuid = NULL;
    setVarCode(varCode);
    setVarUnit(varUnit);
    setVarName(varName);
    setResolution(decimalResolution);

    isCalculated = true;
    setCalculation(calcFxn);
    parentSensor = NULL;

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;

    // MS_DBG(F("Calculated Variable object created"));
}
Variable::Variable() : _sensorVarNum(0), _decimalResolution(0) {
    _varName = NULL;
    _varUnit = NULL;
    _varCode = NULL;
    _uuid    = NULL;

    isCalculated = true;
    _calcFxn     = NULL;
    parentSensor = NULL;

    // When we create the variable, we also want to initialize it with a current
    // value of -9999 (ie, a bad result).
    _currentValue = -9999;

    // MS_DBG(F("Calculated Variable object created"));
}


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
        /*MS_DBG(F("Attempting to register"), getVarName(),
               F("as variable number"), _sensorVarNum, F("to"),
               parentSensor->getSensorName(), F("attached at"),
               parentSensor->getSensorLocation(), F("..."));*/
        parentSensor->registerVariable(_sensorVarNum, this);
    }
    // else
    // {
    //     MS_DBG(F("This is a calculated variable.  It cannot have a parent
    //     sensor!"));
    // }
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
    } else if (parentSensor == NULL) {
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
    } else if (parentSensor == NULL) {
        MS_DBG(F("ERROR! This variable is missing a parent sensor!"));
        return "";
    } else {
        return parentSensor->getSensorNameAndLocation();
    }
}


// This ties a calculated variable to its calculation function
void Variable::setCalculation(float (*calcFxn)()) {
    if (isCalculated) {
        // MS_DBG(F("Calculation function set"));
        _calcFxn = calcFxn;
    }
    // else
    // {
    //     MS_DBG(F("This is a measured variable.  It cannot have a calculation
    //     function!"));
    // }
}


// This sets up the variable (generally attaching it to its parent)
// bool Variable::setup(void)
// {
//     if (!isCalculated) attachSensor(_varNum, parentSensor);
//     return true;
// }


// This gets/sets the variable's resolution for value strings
uint8_t Variable::getResolution(void) {
    return _decimalResolution;
}
void Variable::setResolution(uint8_t decimalResolution) {
    _decimalResolution = decimalResolution;
    // MS_DBG(F("Variable resolution is"), _decimalResolution, F("decimal
    // places"));
}

// This gets/sets the variable's name using
// http://vocabulary.odm2.org/variablename/
String Variable::getVarName(void) {
    return _varName;
}
void Variable::setVarName(const char* varName) {
    _varName = varName;
    // MS_DBG(F("Variable name is"), _varName);
}

// This gets/sets the variable's unit using http://vocabulary.odm2.org/units/
String Variable::getVarUnit(void) {
    return _varUnit;
}
void Variable::setVarUnit(const char* varUnit) {
    _varUnit = varUnit;
    // MS_DBG(F("Variable unit is"), _varUnit);
}

// This returns a customized code for the variable
String Variable::getVarCode(void) {
    return _varCode;
}
// This sets the variable code to a new custom value
void Variable::setVarCode(const char* varCode) {
    _varCode = varCode;
    // MS_DBG(F("Variable code is"), _varCode);
}

// This returns the variable UUID, if one has been assigned
String Variable::getVarUUID(void) {
    return _uuid;
}
// This sets the UUID
void Variable::setVarUUID(const char* uuid) {
    _uuid = uuid;
    // if (strlen(_uuid) == 0)
    // {
    //     MS_DBG(F("No UUID assigned"));
    // }
    // else
    // {
    //     MS_DBG(F("Variable UUID is"), _uuid);
    // }
}
// This checks that the UUID is properly formatted
bool Variable::checkUUIDFormat(void) {
    // If no UUID, move on
    if (strlen(_uuid) == 0) {
        // MS_DBG(F("No UUID assigned to"), getVarCode());
        return true;
    }

    // MS_DBG(F("Variable UUID for"), getVarCode(), F("is"), _uuid);
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
    for (uint8_t i = 0; i < 36; i++) {
        bool isAcceptable = false;
        for (uint8_t j = 0; !isAcceptable && j < 23; j++) {
            if (_uuid[i] == acceptableChars[j]) {
                isAcceptable = true;
                j            = 23;  // Stop the inner loop
            }
        }
        if (!isAcceptable) {
            MS_DBG(F("UUID for"), getVarCode(), '(', _uuid, ')',
                   F("has a bad character"), _uuid[i], F("at"), i + 1);
            return false;
        }
    }
    return true;
}

// Set the variable UUID.
#if defined(__AVR__)
#define freeRamCalc() \
    (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval)
#elif defined(ARDUINO_ARCH_SAMD)
extern "C" char* sbrk(int i);

int16_t freeRamCalc() {
    char stack_dummy = 0;
    return &stack_dummy - sbrk(0);
}
#endif
void Variable::setVarUUID_atl(const char* newUUID, bool copyUid,
                              uint8_t uuidSize) {
#ifdef DEBUGGING_SERIAL_OUTPUT
#if defined __AVR__
    extern int16_t __heap_start, *__brkval;
    int16_t        v;
    int            ramStart = freeRamCalc();

#elif defined(ARDUINO_ARCH_SAMD)
    int ramStart = freeRamCalc();

#else
    int ramStart = -9999;
#endif
#endif  // DEBUGGING_SERIAL_OUTPUT
    //_UUID_atl =  newUUID;
    _uuid = newUUID;

    if (copyUid) {
        /* This allocate a memory for the UUID and copy it into there
         * before assigning reference to it
         */

        if (NULL == _UUID_buf_atl) { _UUID_buf_atl = new char[uuidSize]; }
        if (NULL == _UUID_buf_atl) {
            // Major problem
            PRINTOUT(F("setVarUUID error - new didn't work for"), newUUID);
        } else {
            memcpy(_UUID_buf_atl, newUUID, uuidSize);
            //_UUID =  _UUID_buf_atl;
            _uuid = _UUID_buf_atl;
        }
        MS_DBG(F("setVarUUID cp "), newUUID);
    }
#ifdef DEBUGGING_SERIAL_OUTPUT
    int ramEnd = freeRamCalc();
    MS_DBG(F("setVarUUID ramUsed "), ramStart - ramEnd, " left:", ramEnd);
#endif  // DEBUGGING_SERIAL_OUTPUT
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
        int16_t val = static_cast<int16_t>(getValue(updateValue));
        return String(val);
    } else {
        return String(getValue(updateValue), _decimalResolution);
    }
}
