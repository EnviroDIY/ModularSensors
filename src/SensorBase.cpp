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

    _powerPin = powerPin;
    _dataPin = dataPin;
    _measurementsToAverage = measurementsToAverage;

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

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_VARS; i++)
    {
        variables[i] = NULL;
        sensorValues[i] = -9999;
        numberGoodMeasurementsMade[i] = 0;
    }

    // Reset the sensor status
    _sensorStatus = 0;
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


// This returns the 8-bit code for the current status of the sensor.
// Bit 0 - 0=Not powered, 1=Powered
// Bit 1 - 0=Has NOT been set up, 1=Has been setup
// Bit 2 - 0=Is NOT warmed up, 1=Is warmed up
// Bit 3 - 0=Not awake/actively measuring, 1=Is awake/actively measuring
// Bit 4 - 0=Readings not stable, 1=Readings should be stable
// bit 5 - 0=Measurement requested, 1=No measurements have been requested
// bit 6 - 0=Measurement complete (IFF bit 3 and 4 are set!)
//         1=Waiting for measurement completion (IFF bit 3 and 4 are set!),
// Bit 7 - 0=No known errors, 1=Some sort of error has occured
uint8_t Sensor::getStatus(void){return _sensorStatus;}


// This turns on sensor power
void Sensor::powerUp(void)
{
    if (_powerPin > 0)
    {
        MS_DBG(F("Powering "), getSensorName(), F(" at "), getSensorLocation(),
               F(" with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    // Set the status bit for sensor power (bit 0)
    _sensorStatus |= 0b00000001;
}


// This turns off sensor power
void Sensor::powerDown(void)
{
    if (_powerPin > 0)
    {
        MS_DBG(F("Turning off power to "), getSensorName(), F(" at "),
               getSensorLocation(), F(" with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
    }
    // Unset the status bits for sensor power (bit 0), warm-up (bit 2),
    // activation (bit 3), stability (bit 4), measurement request (bit 5), and
    // measurement completion (bit 6)
    _sensorStatus &= 0b10000010;
}


// The function to set up connection to a sensor.
// By default, sets pin modes and returns true
bool Sensor::setup(void)
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

    // Set the status bit marking that the sensor has been set up (bit 1)
    _sensorStatus |= 0b00000010;

    return true;
}


// The function to wake up a sensor
// By default, verifies the power is on and returns true
bool Sensor::wake(void)
{
    MS_DBG(F("Waking "), getSensorName(), F(" at "),
           getSensorLocation(), F("\n"));
    if(!checkPowerOn()){powerUp();}
    waitForWarmUp();
    // Mark the time that the sensor was activated
    _millisSensorActivated = millis();
    // Set the status bit for sensor activation (bit 3)
    _sensorStatus |= 0b00001000;
    return true;
}


// The function to put a sensor to sleep
// Does NOT power down the sensor!
bool Sensor::sleep(void)
{
    MS_DBG(F("Putting to "), getSensorName(), F(" at "),
           getSensorLocation(), F(" to sleep\n"));
    // Unset the activation time
    _millisSensorActivated = 0;
    // Unset the activated status bit (bit 3), stability (bit 4), measeurement
    // request (bit 5) and measurement completion (bit 6)
    _sensorStatus &= 0b10000111;
    return true;
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
    // Mark the time that a measurement was requested
    _millisMeasurementRequested = millis();
    // Set the status bits for measurement requested (bit 5)
    _sensorStatus |= 0b00100000;
    // Verify that the status bit for a single measurement completion is not set (bit 6)
    _sensorStatus &= 0b10111111;
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
            // Reset time of power on, in-case it was set to a value
            if (_millisPowerOn != 0) _millisPowerOn = 0;
            // Unset the status bits for sensor power (bit 0), warm-up (bit 2),
            // activation (bit 3), stability (bit 4), measurement start (bit 5),
            /// and measurement completion (bit 6)
            _sensorStatus &= 0b10000010;
            return false;
        }
        else
        {
            MS_DBG(F("Power to "), getSensorName(), F(" at "),
                   getSensorLocation(), (" was on.\n"));
            // Mark the power-on time, just in case it  had not been marked
            if (_millisPowerOn == 0) _millisPowerOn = millis();
            // Set the status bit for sensor power (bit 0)
            _sensorStatus |= 0b00000001;
            return true;
        }
    }
    else
    {
        MS_DBG(F("Power is not controlled for "), getSensorName(), F("\n"));
        // Mark the power-on time, just in case it  had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
        // Set the status bit for sensor power (bit 0)
        _sensorStatus |= 0b00000001;
        return true;
    }
}


// This checks to see if enough time has passed for warm-up
bool Sensor::isWarmedUp(void)
{
    if (_warmUpTime_ms != 0)
    {
        if (millis() > (_millisPowerOn + _warmUpTime_ms))  // already warmed up
        {
            MS_DBG(F("It's been "), (millis() - _millisPowerOn), F("ms, and "),
                  getSensorName(), F(" at "),    getSensorLocation(),
                  F(" should be warmed up!\n"));
            // Set the status bit for warm-up completion (bit 2)
            _sensorStatus |= 0b00000100;
            return true;
        }
        else if (millis() > _millisPowerOn)  // just in case millis() has rolled over
        {
            // Make sure the status bits for warm-up (bit 2), activation (bit 3),
            // stability (bit 4), measurement start (bit 5), and measurement
            // completion (bit 6) are not set
            _sensorStatus &= 0b10000011;
            return false;
        }
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            uint32_t millisDone = 4294967295L - _millisPowerOn;
            if (millis() > (millisDone + _warmUpTime_ms))
            {
                MS_DBG(F("It's been "), (millisDone + millis()), F("ms, and "),
                      getSensorName(), F(" at "), getSensorLocation(),
                      F(" should be warmed up! (Millis rollover during wait.)\n"));
                // Set the status bit for warm-up completion (bit 2)
                _sensorStatus |= 0b00000100;
                return true;
            }
            else
            {
                // Make sure the status bits for warm-up (bit 2), activation (bit 3),
                // stability (bit 4), measurement start (bit 5), and measurement
                // completion (bit 6) are not set
                _sensorStatus &= 0b10000011;
                return false;
            }
        }
    }
    else
    {
        MS_DBG(getSensorName(), F(" at "), getSensorLocation(),
               F(" needs no warm-up time.\n"));
        // Set the status bit for warm-up completion (bit 2)
        _sensorStatus |= 0b00000100;
        return true;
    }
}

// This delays until enough time has passed for the sensor to "warm up" - that
// is - to be ready to communicate and to be asked to take readings
void Sensor::waitForWarmUp(void){ while (!isWarmedUp()){} }


// This checks to see if enough time has passed for stability
bool Sensor::isStable(void)
{
    if (_stabilizationTime_ms != 0)
    {
        if (millis() > (_millisSensorActivated + _stabilizationTime_ms))
        {
            MS_DBG(F("It's been "), (millis() - _millisSensorActivated), F("ms, and "),
                   getSensorName(), F(" at "), getSensorLocation(),
                   F(" should be stable!\n"));
            // Set the status bit for stability completion (bit 4)
            _sensorStatus |= 0b00010000;
            return true;
        }
        else if (millis() > _millisSensorActivated)  // just in case millis() has rolled over
        {
            // Make sure the status bits for stability (bit 4), measurement
            // start (bit 5) and measurement completion (bit 6) are not set
            _sensorStatus &= 0b10001111;
            return false;
        }
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            uint32_t millisDone = 4294967295L - _millisSensorActivated;
            if (millis() > (millisDone + _stabilizationTime_ms))
            {
                MS_DBG(F("It's been "), (millisDone + millis()), F("ms, and "),
                       getSensorName(), F(" at "), getSensorLocation(),
                       F(" should be stable! (Millis rollover during wait.)\n"));
                // Set the status bit for stability completion (bit 4)
                _sensorStatus |= 0b00010000;
                return true;
            }
            else
            {
                // Make sure the status bits for stability (bit 4), measurement
                // start (bit 5) and measurement completion (bit 6) are not set
                _sensorStatus &= 0b10001111;
                return false;
            }
        }
    }
    else
    {
        MS_DBG(getSensorName(), F(" at "), getSensorLocation(),
              F(" needs no stabilization time.\n"));
       // Set the status bit for stability completion (bit 4)
       _sensorStatus |= 0b00010000;
        return true;
    }
}

// This delays until enough time has passed for the sensor to stabilize before
// taking readings
void Sensor::waitForStability(void){ while (!isStable()){} }


// This checks to see if enough time has passed for measurement completion
bool Sensor::isMeasurementComplete(void)
{
    if (_measurementTime_ms != 0)
    {
        if (millis() > (_millisMeasurementRequested + _measurementTime_ms))
        {
            MS_DBG(F("It's been "), (millis() - _millisMeasurementRequested),
                   F("ms, and measurement by "), getSensorName(), F(" at "),
                   getSensorLocation(), F(" should be complete!\n"));
            // Set the status bit for measurement completion (bit 6)
            _sensorStatus |= 0b01000000;
            return true;
        }
        else if (millis() > _millisMeasurementRequested)  // just in case millis() has rolled over
        {
            // Make sure the status bit for measurement completion (bit 6) is not set
            _sensorStatus &= 0b10111111;
            return false;
        }
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            uint32_t millisDone = 4294967295L - _millisMeasurementRequested;
            if (millis() > (millisDone + _measurementTime_ms))
            {
                MS_DBG(F("It's been "), (millisDone + millis()),
                       F("ms, and measurement by "), getSensorName(), F(" at "),
                       getSensorLocation(), F(" should be complete! (Millis rollover during wait.)\n"));
                // Set the status bit for measurement completion (bit 6)
                _sensorStatus |= 0b01000000;
                return true;
            }
            else
            {
                // Make sure the status bit for measurement completion (bit 6) is not set
                _sensorStatus &= 0b10111111;
                return false;
            }
        }
    }
    else
    {
        MS_DBG(F("Measurement by "), getSensorName(), F(" at "),
               getSensorLocation(), F(" should be immediately complete!\n"));
        // Set the status bit for measurement completion (bit 6)
        _sensorStatus |= 0b01000000;
        return true;
    }
}

// This delays until enough time has passed for the sensor to give a new value
void Sensor::waitForMeasurementCompletion(void){ while (!isMeasurementComplete()){} }


void Sensor::updateStatusBits(void)
{
    // first check that there is power, just return if not
    if (!checkPowerOn()) return;
    // if the sensor isn't warmed-up, quit
    if (!isWarmedUp()) return;
    // if the sensor is not awake/active, quit
    if (!bitRead(_sensorStatus, 3)) return;
    // if the sensor hasn't stabilized, quit
    if (!isStable()) return;
    // if the sensor is not taking a measurement, quit
    if (!bitRead(_sensorStatus, 5)) return;
    // Last thing it to check if a measurement is finished
    isMeasurementComplete();
}
