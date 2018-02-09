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
Sensor::Sensor(String sensorName, int numReturnedVars,
               uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t remeasurementTime_ms,
               int powerPin, int dataPin, int measurementsToAverage)
{
    _sensorName = sensorName;
    _numReturnedVars = numReturnedVars;
    _sensorLastUpdated = 0;

    _warmUpTime_ms = warmUpTime_ms;
    _millisPowerOn = 0;

    _stabilizationTime_ms = stabilizationTime_ms;
    _millisMeasurementStarted = 0;

    _remeasurementTime_ms = remeasurementTime_ms;
    _lastMeasurementRequested = 0;

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


// This is a helper function to turn on sensor power
void Sensor::powerUp(void)
{
    if (_powerPin > 0)
    {
        MS_DBG(F("Powering "), getSensorName(), F(" at "), getSensorLocation(),
               F(" with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, HIGH);
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
    _millisMeasurementStarted = millis();
    return true;
}


// The function to put a sensor to sleep
// Does NOT power down the sensor!
bool Sensor::sleep(void)
{
    MS_DBG(F("Putting to "), getSensorName(), F(" at "),
           getSensorLocation(), F(" to sleep\n"));
    _millisMeasurementStarted = 0;
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
        _millisPowerOn = 0;
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
            MS_DBG(getSensorName(), F(" at "), getSensorLocation(),
                   F(" already warmed up!\n"));
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
            MS_DBG(F("Giving a safety of 2000ms for "), getSensorName(),
                   F(" at "), getSensorLocation(), F(" warm-up\n"));
            while(millis() < 2000){}
        }
    }
}


// This is a helper function to wait that enough time has passed for the sensor
// to stabilize before taking readings
void Sensor::waitForStability(void)
{
    if (_stabilizationTime_ms != 0)
    {
        if (millis() > _millisMeasurementStarted + _stabilizationTime_ms)  // already ready
        {
            MS_DBG(getSensorName(), F(" at "), getSensorLocation(), F(" should be stable!\n"));
        }
        else if (millis() > _millisMeasurementStarted)  // just in case millis() has rolled over
        {
            MS_DBG(F("Waiting "), (_stabilizationTime_ms - (millis() - _millisMeasurementStarted)),
                   F("ms for "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" to stabilize\n"));
            while((millis() - _millisMeasurementStarted) < _stabilizationTime_ms){}
        }
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            MS_DBG(F("Giving a safety of 20s for "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" stability\n"));
            while(millis() < 20000){}
        }
    }
}


// This is a helper function to wait that enough time has passed for the sensor
// to give a new value
void Sensor::waitForMeasurementCompletion(void)
{
    if (_remeasurementTime_ms != 0)
    {
        if (millis() > _lastMeasurementRequested + _remeasurementTime_ms)  // already ready
        {
            MS_DBG(F("Measurement by "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" should be complete!\n"));
        }
        else if (millis() > _lastMeasurementRequested)  // just in case millis() has rolled over
        {
            MS_DBG(F("Waiting "), (_remeasurementTime_ms - (millis() - _lastMeasurementRequested)),
                   F("ms for "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" to complete measurement\n"));
            while((millis() - _lastMeasurementRequested) < _remeasurementTime_ms){}
        }
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            MS_DBG(F("Giving a safety of 2s for "), getSensorName(),
                   F(" measurement completion\n"));
            while(millis() < 2000){}
        }
    }
}


// The function to set up connection to a sensor.
// By default, sets pin modes and returns ready
SENSOR_STATUS Sensor::setup(void)
{
    if (_powerPin > 0) pinMode(_powerPin, OUTPUT);
    if (_dataPin > 0) pinMode(_dataPin, INPUT_PULLUP);

    MS_DBG(F("Set up "));
    MS_DBG(getSensorName(), F(" at "), getSensorLocation());
    MS_DBG(F(" attached at "));
    MS_DBG(getSensorLocation());
    MS_DBG(F(" which can return up to "));
    MS_DBG(_numReturnedVars);
    MS_DBG(F(" variable[s].\n"));

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
            variables[i]->onSensorUpdate(this);
        }
        else MS_DBG(F("Null pointer\n"));
    }
}


// This function checks if a sensor needs to be updated or not
bool Sensor::checkForUpdate(uint32_t sensorLastUpdated)
{
    MS_DBG(F("It has been "), (millis() - sensorLastUpdated)/1000);
    MS_DBG(F(" seconds since "));
    MS_DBG(getSensorName(), F(" at "), getSensorLocation());
    MS_DBG(F(" was updated.\n"));
    if ((millis() > 120000 and millis() > (sensorLastUpdated + 120000))
        or sensorLastUpdated == 0)
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
    _lastMeasurementRequested = millis();
    return true;
}


// This verifies that a measurement is good before adding it to the values to be averaged
void Sensor::verifyAndAddMeasurementResult(int resultNumber, float resultValue)
{
    // If the new result is good and there was were only bad results, set the
    // result value as the new result and add 1 to the good result total
    if (sensorValues[resultNumber] == -9999 and resultValue != -9999)
    {
        sensorValues[resultNumber] =  resultValue;
        numberGoodMeasurementsMade[resultNumber] += 1;
    }
    // If the new result is good and there were already good results in place
    // add the new results to the total and add 1 to the good result total
    if (sensorValues[resultNumber] != -9999 and resultValue != -9999)
    {
        sensorValues[resultNumber] +=  resultValue;
        numberGoodMeasurementsMade[resultNumber] += 1;
    }
    // If the new result is bad and there were only bad results, do nothing
    if (sensorValues[resultNumber] == -9999 and resultValue == -9999){}
    // If the new result is bad and there were already good results, do nothing
    if (sensorValues[resultNumber] == -9999 and resultValue == -9999){}
}
void Sensor::verifyAndAddMeasurementResult(int resultNumber, int resultValue)
{
    verifyAndAddMeasurementResult(resultNumber, resultValue);
}


void Sensor::averageMeasurements(void)
{
    MS_DBG(F("Averaging over "), _measurementsToAverage, F(" readings\n"));
    for (int i = 0; i < _numReturnedVars; i++)
    {
        sensorValues[i] /=  numberGoodMeasurementsMade[i];
        MS_DBG(F("Result #"), i, F(": "), sensorValues[i], F("\n"));
    }
}


// This updates a sensor value by checking it's power, waking it, taking as many
// readings as requested, then putting the sensor to sleep and powering down.
bool Sensor::update(void)
{
    bool ret_val;

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
