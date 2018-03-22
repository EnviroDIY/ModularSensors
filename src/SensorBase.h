/*
 *SensorBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the sensor base class.
*/

#ifndef SensorBase_h
#define SensorBase_h

#include <Arduino.h>
#include <pins_arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

// The largest number of variables from a single sensor
#define MAX_NUMBER_VARS 4


class Variable;  // Forward declaration

// Defines the "Sensor" Class
class Sensor
{
public:

    Sensor(String sensorName = "Unknown", uint8_t numReturnedVars = 1,
           uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0,
           int8_t powerPin = -1, int8_t dataPin = -1, uint8_t measurementsToAverage = 1);

    // These functions are dependent on the constructor and return the constructor values
    // This gets the place the sensor is installed ON THE ARDUINO (ie, pin number)
    virtual String getSensorLocation(void);
    // This gets the name of the sensor.
    virtual String getSensorName(void);

    // These functions get and set the number of readings to average for a sensor
    // Generally these values should be set in the constructor
    void setNumberMeasurementsToAverage(int nReadings);
    int getNumberMeasurementsToAverage(void);

    // These next functions have defaults.

    // This turns on the sensor power, if applicable
    // This also sets the _millisPowerOn timestamp.
    virtual void powerUp(void);
    // This turns off the sensor power, if applicable
    // This also un-sets the _millisPowerOn timestamp.
    virtual void powerDown(void);
    // This wakes the sensor up, if necessary - that is, does whatever it takes to
    // get a sensor in the proper state to begin a measurement after the power is on.
    // This *may* require a waitForWarmUp() before wake commands can be sent.
    // This also sets the _millisSensorActivated timestamp.
    // By default, verifies the power is on and returns true
    virtual bool wake(void);
    // This puts the sensor to sleep, if necessary.
    // This also un-sets the _millisSensorActivated timestamp.
    // Does NOT power down the sensor!
    virtual bool sleep(void);

    // This sets up the sensor, if necessary.  Defaults to true.
    virtual bool setup(void);

    // This returns the 8-bit code for the current status of the sensor.
    // Bit 0 - 0=Not powered, 1=Powered
    // Bit 1 - 0=Has been setup, 1=Has NOT been set up
    // Bit 2 - 0=Is warmed up, 1=Is not warmed up
    // Bit 3 - 0=Is awake/actively measuring, 1=Not awake/actively measuring
    // Bit 4 - 0=Readings should be stable, 1=Readings not stable
    // Bit 5 - 0=Waiting for measurement completion (IFF bit 3 and 4 are set!),
    //         1=Measurement complete (IFF bit 3 and 4 are set!)
    // Bit 6 - 0=, 1=
    // Bit 7 - 0=No known errors, 1=Some sort of error has occured
    virtual uint8_t getStatus(void);

    // This tells the sensor to start a single measurement, if needed
    // This also sets the _millisMeasurementRequested timestamp.
    // This *may* require a waitForWarmUp() before measurement commands can be sent.
    // This *may* also require a waitForStability() before returned measurements will be any good.
    virtual bool startSingleMeasurement(void);

    // This next function must be implemented for ever sensor!!
    // This actually gets the results from a single measurement
    // This also un-sets the _millisMeasurementRequested timestamp.
    // This *may* also require a waitForStability() before returned measurements will be any good.
    // This will often require a waitForMeasurementCompletion() to ensure a measurement is done.
    virtual bool addSingleMeasurementResult(void) = 0;

    // This verifies that a measurement is OK before adding it to the array
    virtual void verifyAndAddMeasurementResult(int resultNumber, float resultValue);
    virtual void verifyAndAddMeasurementResult(int resultNumber, int resultValue);
    void averageMeasurements(void);

    // This updates the sensor's values
    // This includes clears the values array, starts and averages as many
    // measurement readings as requested, and then notifies the registerd
    // variables of the new resutls.
    virtual bool update(void);

    // These tie the variables to their parent sensor
    virtual void registerVariable(int varNum, Variable* var);
    // Clears the values array
    void clearValues();
    // Notifies attached variables of new values
    virtual void notifyVariables(void);
    float sensorValues[MAX_NUMBER_VARS];

    // This is the time that a value was last sent ot registered variables
    // It is set in the notifyVariables() function.
    // The "checkForUpdate()" function checks if values are older than 2 minutes.
    uint32_t _sensorLastUpdated;
    bool checkForUpdate(void);


protected:

    int8_t _dataPin;  // SIGNED int, to allow negative numbers for unused pins
    int8_t _powerPin;  // SIGNED int, to allow negative numbers for unused pins
    String _sensorName;
    uint8_t _numReturnedVars;
    uint8_t _measurementsToAverage;
    uint8_t numberGoodMeasurementsMade[MAX_NUMBER_VARS];

    // This is the time needed from the when a sensor has power until it's ready to talk
    // The _millisPowerOn value is set in the powerUp() function.  It is
    // un-set in the powerDown() function.
    // The "isWarmedUp()" function checks whether or not enough time has passed
    // and "waitForWarmUp()" function delays until the time passes.
    // "checkPowerOn()" checks if the power pin is currently high
    uint32_t _warmUpTime_ms;
    uint32_t _millisPowerOn;
    bool isWarmedUp(void);
    void waitForWarmUp(void);
    bool checkPowerOn(void);

    // This is the time needed from the when a sensor is activated until the readings are stable
    // The _millisSensorActivated value is *usually* set in the wake() function,
    // but may also be set in the startSingleMeasurement() function.  It is
    // generally un-set in the sleep() function.
    // The "isStable()" function checks whether or not enough time has passed
    // and "waitForStability()" function delays until the time passes.
    uint32_t _stabilizationTime_ms;
    uint32_t _millisSensorActivated;
    bool isStable(void);
    void waitForStability(void);

    // This is the time needed from the when a sensor is told to take a single
    // reading until that reading should be complete
    // The _millisMeasurementRequested value is set in the startSingleMeasurement() function.
    // It *may* be unset in the addSingleMeasurementResult() function.
    // The "isMeasurementComplete()" function checks whether or not enough time has passed
    // and "waitForMeasurementCompletion()" function delays until the time passes.
    uint32_t _measurementTime_ms;
    uint32_t _millisMeasurementRequested;
    bool isMeasurementComplete(void);
    void waitForMeasurementCompletion(void);

    // This is an 8-bit code for the sensor status
    uint8_t _sensorStatus;

    // This is an array for each sensor containing the variable objects tied
    // to that sensor.  The MAX_NUMBER_VARS cannot be determined on a per-sensor
    // basis, because of the way memory is used on an Arduino.  It must be
    // defined once for the whole class.
    Variable *variables[MAX_NUMBER_VARS];
};

#endif
