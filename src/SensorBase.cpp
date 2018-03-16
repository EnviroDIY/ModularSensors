/*
 *SensorBase.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the sensor base class.
*/

#include "SensorBase.h"
#include "VariableBase.h"

// ============================================================================
//  The class and functions for interfacing with a sensor
// ============================================================================

// The constructor
Sensor::Sensor(String sensorName, uint8_t numReturnedVars,
               uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t measurementTime_ms,
               int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage)
{
    _sensorName = sensorName;
    _numReturnedVars = numReturnedVars;

    // This is the time that a value was last sent ot registered variables
    // It is set in the notifyVariables() function.
    // The "checkForUpdate()" function checks if values are older than 2 minutes.
    _sensorLastUpdated = 0;

    // This is the time needed from the when a sensor has power until it's ready to talk
    // The _millisPowerOn value is set in the powerUp() function.  It is
    // un-set in the powerDown() function.
    // The "waitForWarmUp()" function verifies that enough time has passed.
    _warmUpTime_ms = warmUpTime_ms;
    _millisPowerOn = 0;

    // This is the time needed from the when a sensor is activated until the readings are stable
    // The _millisSensorActivated value is *usually* set in the wake() function,
    // but may also be set in the startSingleMeasurement() function.  It is
    // generally un-set in the sleep() function.
    // The "waitForStability()" function verifies that enough time has passed.
    _stabilizationTime_ms = stabilizationTime_ms;
    _millisSensorActivated = 0;

    // This is the time needed from the when a sensor is told to take a single
    // reading until that reading should be complete
    // The _millisMeasurementRequested value is set in the startSingleMeasurement() function.
    // It *may* be unset in the addSingleMeasurementResult() function.
    // The "waitForMeasurementCompletion()" function verifies that enough time has passed.
    _measurementTime_ms = measurementTime_ms;
    _millisMeasurementRequested = 0;

    _powerPin = powerPin;
    _dataPin = dataPin;
    _measurementsToAverage = measurementsToAverage;

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_VARS; i++)
    {
        variables[i] = NULL;
        sensorValues[i] = -9999;
        numberGoodMeasurementsMade[i] = 0;
    }
}


// This gets the place the sensor is installed ON THE MAYFLY (ie, pin number)
String Sensor::getSensorLocation(void)
{
    String senseLoc = F("Pin");
    senseLoc +=String(_dataPin);
    return senseLoc;
}


// This returns the name of the sensor.
String Sensor::getSensorName(void){return _sensorName;}


// These functions get and set the number of readings to average for a sensor
// Generally these values should be set in the constructor
void Sensor::setNumberMeasurementsToAverage(int nReadings)
{
    _measurementsToAverage = nReadings;
}
int Sensor::getNumberMeasurementsToAverage(void){return _measurementsToAverage;}


// This turns on sensor power
void Sensor::powerUp(void)
{
    if (_powerPin > 0)
    {
        MS_DBG(F("Powering "), getSensorName(), F(" at "), getSensorLocation(),
               F(" with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, HIGH);
        // Mark that the sensor now has power
        _millisPowerOn = millis();
    }
}


// The function to wake up a sensor
// By default, verifies the power is on and returns true
bool Sensor::wake(void)
{
    MS_DBG(F("Waking "), getSensorName(), F(" at "),
           getSensorLocation(), F("\n"));
    if(!checkPowerOn()){powerUp();}
    // Mark that the sensor has been activated
    _millisSensorActivated = millis();
    return true;
}


// The function to put a sensor to sleep
// Does NOT power down the sensor!
bool Sensor::sleep(void)
{
    MS_DBG(F("Putting to "), getSensorName(), F(" at "),
           getSensorLocation(), F(" to sleep\n"));
    // Mark that the sensor is no longer activated
    _millisSensorActivated = 0;
    return true;
}


// This is a helper function to turn off sensor power
void Sensor::powerDown(void)
{
    if (_powerPin > 0)
    {
        MS_DBG(F("Turning off power to "), getSensorName(), F(" at "),
               getSensorLocation(), F(" with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, LOW);
        // Mark that the sensor no longer has power
        _millisPowerOn = 0;
    }
}


// The function to set up connection to a sensor.
// By default, sets pin modes and returns ready
SENSOR_STATUS Sensor::setup(void)
{
    MS_DBG(F("Setting up "));
    MS_DBG(getSensorName());
    MS_DBG(F(" attached at "));
    MS_DBG(getSensorLocation());
    MS_DBG(F(" which can return up to "));
    MS_DBG(_numReturnedVars);
    MS_DBG(F(" variable[s].\n"));

    if (_powerPin > 0) pinMode(_powerPin, OUTPUT);
    if (_dataPin > 0) pinMode(_dataPin, INPUT_PULLUP);

    return SENSOR_READY;
}


// The function to return the status of a sensor
// By default, simply returns ready
SENSOR_STATUS Sensor::getStatus(void){return SENSOR_READY;}


String Sensor::printStatus(SENSOR_STATUS stat)
{
    String status;
    switch(stat)
    {
        case SENSOR_ERROR: status = F("Errored"); break;
        case SENSOR_READY: status = F("Ready"); break;
        case SENSOR_WAITING: status = F("Waiting"); break;
        case SENSOR_UNKNOWN: status = F("Unknown"); break;
    }
    return status;
}


void Sensor::registerVariable(int varNum, Variable* var)
{
    variables[varNum] = var;
    MS_DBG(F("... Registration from "));
    MS_DBG(getSensorName(), F(" at "), getSensorLocation());
    MS_DBG(F(" for "));
    MS_DBG(var->getVarName());
    MS_DBG(F(" accepted.\n"));
}


void Sensor::notifyVariables(void)
{
    MS_DBG(F("Notifying variables registered to "), getSensorName(), F(" at "),
           getSensorLocation(), F(".\n"));
    // Make note of the last time updated
    _sensorLastUpdated = millis();

    // Notify variables of update
    for (int i = 0; i < _numReturnedVars; i++)
    {
        if (variables[i] != NULL)  // Bad things happen if try to update nullptr
        {
            MS_DBG(F("Sending value update from "));
            MS_DBG(getSensorName(), F(" at "), getSensorLocation());
            MS_DBG(F(" to variable "));
            MS_DBG(i);
            MS_DBG(F(" which is "));
            MS_DBG(variables[i]->getVarName());
            MS_DBG(F("...   "));
            MS_DBG(F("\n"));
            variables[i]->onSensorUpdate(this);
        }
        else MS_DBG(F("Null pointer\n"));
    }
}


// This function checks if a sensor needs to be updated or not
bool Sensor::checkForUpdate(void)
{
    // MS_DBG(F("It has been "), (millis() - _sensorLastUpdated)/1000);
    // MS_DBG(F(" seconds since "));
    // MS_DBG(getSensorName(), F(" at "), getSensorLocation());
    // MS_DBG(F(" was updated.\n"));
    if ((millis() > 120000L and millis() > (_sensorLastUpdated + 120000L))
        or _sensorLastUpdated == 0)
    {
        MS_DBG(F("Value out of date, updating\n"));
        return(update());
    }
    else return(true);
}


// This function just empties the value array
void Sensor::clearValues(void)
{
    MS_DBG(F("Clearing value array for "), getSensorName(), F(" at "),
           getSensorLocation(), F(".\n"));
    for (int i = 0; i < _numReturnedVars; i++)
    {
        sensorValues[i] =  -9999;
        numberGoodMeasurementsMade[i] = 0;
    }
}


// This is a place holder for starting a single measurment, for those sensors
// that need no instructions to start a measurement.
bool Sensor::startSingleMeasurement(void)
{
    waitForWarmUp();
    waitForStability();
    _millisMeasurementRequested = millis();
    return true;
}


// This verifies that a measurement is good before adding it to the values to be averaged
void Sensor::verifyAndAddMeasurementResult(int resultNumber, float resultValue)
{
    // If the new result is good and there was were only bad results, set the
    // result value as the new result and add 1 to the good result total
    if (sensorValues[resultNumber] == -9999 and resultValue != -9999)
    {
        MS_DBG(F("Putting "), resultValue, F(" in result array for variable "),
               resultNumber, F(" from "), getSensorName(), F(" at "),
               getSensorLocation(), F(".\n"));
        sensorValues[resultNumber] =  resultValue;
        numberGoodMeasurementsMade[resultNumber] += 1;
    }
    // If the new result is good and there were already good results in place
    // add the new results to the total and add 1 to the good result total
    else if (sensorValues[resultNumber] != -9999 and resultValue != -9999)
    {
        MS_DBG(F("Adding "), resultValue, F(" to result array for variable "),
               resultNumber, F(" from "), getSensorName(), F(" at "),
               getSensorLocation(), F(".\n"));
        sensorValues[resultNumber] +=  resultValue;
        numberGoodMeasurementsMade[resultNumber] += 1;
    }
    // If the new result is bad and there were only bad results, do nothing
    else if (sensorValues[resultNumber] == -9999 and resultValue == -9999)
        MS_DBG(F("Ignoring bad result for variable "),
               resultNumber, F(" from "), getSensorName(), F(" at "),
               getSensorLocation(), F("; no good results yet.\n"));
    // If the new result is bad and there were already good results, do nothing
    else if (sensorValues[resultNumber] != -9999 and resultValue == -9999)
        MS_DBG(F("Ignoring bad result for variable "),
               resultNumber, F(" from "), getSensorName(), F(" at "),
               getSensorLocation(), F("; good results already in array.\n"));
}
void Sensor::verifyAndAddMeasurementResult(int resultNumber, int resultValue)
{
    float float_val = resultValue;
    verifyAndAddMeasurementResult(resultNumber, float_val);
}


void Sensor::averageMeasurements(void)
{
    MS_DBG(F("Averaging results from "), getSensorName(), F(" at "),
           getSensorLocation(), F(" over "), _measurementsToAverage,
           F(" reading[s]\n"));
    for (int i = 0; i < _numReturnedVars; i++)
    {
        if (numberGoodMeasurementsMade[i] > 0)
            sensorValues[i] /=  numberGoodMeasurementsMade[i];
        MS_DBG(F("    ->Result #"), i, F(": "), sensorValues[i], F("\n"));
    }
}


// This updates a sensor value by checking it's power, waking it, taking as many
// readings as requested, then putting the sensor to sleep and powering down.
bool Sensor::update(void)
{
    bool ret_val = true;

    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){ret_val += wake();}
    // Wait until the sensor is warmed up and stable
    waitForWarmUp();
    waitForStability();

    // Clear values before starting loop
    clearValues();

    for (int j = 0; j < _measurementsToAverage; j++)
    {
        ret_val += startSingleMeasurement();
        ret_val += addSingleMeasurementResult();
    }

    averageMeasurements();

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Update the registered variables with the new values
    notifyVariables();

    return ret_val;
}


// This is a helper function to check if the power needs to be turned on
bool Sensor::checkPowerOn(void)
{
    MS_DBG(F("Checking power status:  "));
    if (_powerPin > 0)
    {
        int powerBitNumber = log(digitalPinToBitMask(_powerPin))/log(2);
        if (bitRead(*portInputRegister(digitalPinToPort(_powerPin)), powerBitNumber) == LOW)
        {
            MS_DBG(F("Power to "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" was off.\n"));
            if (_millisPowerOn != 0) _millisPowerOn = 0;
            return false;
        }
        else
        {
            MS_DBG(F("Power to "), getSensorName(), F(" at "),
                   getSensorLocation(), (" was on.\n"));
            if (_millisPowerOn == 0) _millisPowerOn = millis();
            return true;
        }
    }
    else
    {
        MS_DBG(F("Power is not controlled for "), getSensorName(), F("\n"));
        if (_millisPowerOn == 0) _millisPowerOn = millis();
        return true;
    }
}

// This is a helper function to wait that enough time has passed for the sensor
// to warm up before taking readings
void Sensor::waitForWarmUp(void)
{
    if (_warmUpTime_ms != 0)
    {
        if (millis() > (_millisPowerOn + _warmUpTime_ms))  // already ready
        {
            MS_DBG(F("It's been "), (millis() - _millisPowerOn), F("ms, and "),
                  getSensorName(), F(" at "),    getSensorLocation(),
                  F(" should be warmed up!\n"));
        }
        else if (millis() > _millisPowerOn)  // just in case millis() has rolled over
        {
            MS_DBG(F("Waiting "), (_warmUpTime_ms - (millis() - _millisPowerOn)),
                   F("ms for "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" warm-up\n"));
            while((millis() - _millisPowerOn) < _warmUpTime_ms){}
        }
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            MS_DBG(F("Millis() rolled over during warmup for "), getSensorName(),
                   F(" at "), getSensorLocation(), '\n');
            uint32_t millisDone = 4294967295L - _millisPowerOn;
            MS_DBG(F("Waiting "), ((_warmUpTime_ms - millisDone) - (millis() - _millisPowerOn)),
                   F("ms for "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" warm-up\n"));
            while((millis() - _millisPowerOn) < (_warmUpTime_ms - millisDone)){}
        }
    }
}

// This just checks to see if enough time has passed for warm-up
bool Sensor::isWarmedUp(void)
{
    if (_warmUpTime_ms != 0)
    {
        if (millis() > (_millisPowerOn + _warmUpTime_ms)) return true;
        else if (millis() > _millisPowerOn)  // just in case millis() has rolled over
            return false;
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            uint32_t millisDone = 4294967295L - _millisPowerOn;
            if (millis() > (millisDone + _warmUpTime_ms)) return true;
            else return false;
        }
    }
    else return true;
}


// This is a helper function to wait that enough time has passed for the sensor
// to stabilize before taking readings
void Sensor::waitForStability(void)
{
    if (_stabilizationTime_ms != 0)
    {
        if (millis() > _millisSensorActivated + _stabilizationTime_ms)  // already ready
        {
            MS_DBG(F("It's been "), (millis() - _millisSensorActivated), F("ms, and "),
                   getSensorName(), F(" at "),    getSensorLocation(),
                   F(" should be stable!\n"));
        }
        else if (millis() > _millisSensorActivated)  // just in case millis() has rolled over
        {
            MS_DBG(F("Waiting "), (_stabilizationTime_ms - (millis() - _millisSensorActivated)),
                   F("ms for "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" to stabilize\n"));
            while((millis() - _millisSensorActivated) < _stabilizationTime_ms){}
        }
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            MS_DBG(F("Millis() rolled over during stabilization of "), getSensorName(),
                   F(" at "), getSensorLocation(), '\n');
            uint32_t millisDone = 4294967295L - _millisSensorActivated;
            MS_DBG(F("Waiting "), ((_stabilizationTime_ms - millisDone) - (millis() - _millisSensorActivated)),
                   F("ms for "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" to stabilize\n"));
            while((millis() - _millisSensorActivated) < (_stabilizationTime_ms - millisDone)){}
        }
    }
}

// This just checks to see if enough time has passed for stability
bool Sensor::isStable(void)
{
    if (_stabilizationTime_ms != 0)
    {
        if (millis() > (_millisSensorActivated + _stabilizationTime_ms)) return true;
        else if (millis() > _millisSensorActivated)  // just in case millis() has rolled over
            return false;
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            uint32_t millisDone = 4294967295L - _millisSensorActivated;
            if (millis() > (millisDone + _stabilizationTime_ms)) return true;
            else return false;
        }
    }
    else return true;
}


// This is a helper function to wait that enough time has passed for the sensor
// to give a new value
void Sensor::waitForMeasurementCompletion(void)
{
    if (_measurementTime_ms != 0)
    {
        if (millis() > _millisMeasurementRequested + _measurementTime_ms)  // already ready
        {
            MS_DBG(F("It's been "), (millis() - _millisMeasurementRequested),
                  F("ms, and measurement by "), getSensorName(), F(" at "),
                  getSensorLocation(), F(" should be complete!\n"));
        }
        else if (millis() > _millisMeasurementRequested)  // just in case millis() has rolled over
        {
            MS_DBG(F("Waiting "), (_measurementTime_ms - (millis() - _millisMeasurementRequested)),
                   F("ms for "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" to complete measurement\n"));
            while((millis() - _millisMeasurementRequested) < _measurementTime_ms){}
        }
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            MS_DBG(F("Millis() rolled over during measurement on "), getSensorName(),
                   F(" at "), getSensorLocation(), '\n');
            uint32_t millisDone = 4294967295L - _millisMeasurementRequested;
            MS_DBG(F("Waiting "), ((_measurementTime_ms - millisDone) - (millis() - _millisMeasurementRequested)),
                   F("ms for "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" to stabilize\n"));
            while((millis() - _millisMeasurementRequested) < (_measurementTime_ms - millisDone)){}
        }
    }
}

// This just checks to see if enough time has passed for measurement completion
bool Sensor::isMeasurementComplete(void)
{
    if (_measurementTime_ms != 0)
    {
        if (millis() > (_millisMeasurementRequested + _measurementTime_ms)) return true;
        else if (millis() > _millisMeasurementRequested)  // just in case millis() has rolled over
            return false;
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            uint32_t millisDone = 4294967295L - _millisMeasurementRequested;
            if (millis() > (millisDone + _measurementTime_ms)) return true;
            else return false;
        }
    }
    else return true;
}
