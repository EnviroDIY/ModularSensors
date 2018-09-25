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
#define MAX_NUMBER_VARS 8


class Variable;  // Forward declaration

// Defines the "Sensor" Class
class Sensor
{
public:

    Sensor(const char *sensorName = "Unknown", uint8_t numReturnedVars = 1,
           uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0,
           int8_t powerPin = -1, int8_t dataPin = -1, uint8_t measurementsToAverage = 1);

    // These functions are dependent on the constructor and return the constructor values
    // This gets the place the sensor is installed ON THE ARDUINO (ie, pin number)
    virtual String getSensorLocation(void);
    // This gets the name of the sensor.
    virtual String getSensorName(void);
    // This gets the pin number for the power pin.
    virtual int8_t getPowerPin(void);

    // These functions get and set the number of readings to average for a sensor
    // Generally these values should be set in the constructor
    void setNumberMeasurementsToAverage(int nReadings);
    int getNumberMeasurementsToAverage(void);

    // This returns the 8-bit code for the current status of the sensor.
    // Bit 0 - 0=Not powered, 1=Powered
    // Bit 1 - 0=Has NOT been set up, 1=Has been setup
    // Bit 2 - 0=Is NOT warmed up, 1=Is warmed up
    // Bit 3 - 0=Not awake/actively measuring, 1=Is awake/actively measuring
    // Bit 4 - 0=Readings not stable, 1=Readings should be stable
    // Bit 5 - 0=Measurement requested, 1=No measurements have been requested
    // Bit 6 - 0=Waiting for measurement completion (IFF bit 3 and 4 are set!),
    //         1=Measurement complete (IFF bit 3 and 4 are set!)
    // Bit 7 - 0=No known errors, 1=Some sort of error has occurred
    uint8_t getStatus(void);
    // This function checks the current status
    void updateStatusBits(bool debug=false);

    // This sets up the sensor, if necessary.  Defaults to true.
    // Generally, the sensor must be powered on for setup.
    virtual bool setup(void);

    // This updates the sensor's values
    // This clears the values array, starts and averages as many measurement
    // readings as requested, and then notifies the registered variables
    // of the new results.  All possible waits are included in this function!
    virtual bool update(void);

    // This turns on the sensor power, if applicable
    // This also sets the _millisPowerOn timestamp.
    virtual void powerUp(void);
    // This turns off the sensor power, if applicable
    // This also un-sets the _millisPowerOn timestamp.
    virtual void powerDown(void);

    // This wakes the sensor up, if necessary - that is, does whatever it takes to
    // get a sensor in the proper state to begin a measurement after the power is on.
    // This *may* require a waitForWarmUp() before wake commands can be sent.
    // The wait is NOT included in this function!
    // This also sets the _millisSensorActivated timestamp.
    // By default, verifies the power is on and returns true
    virtual bool wake(void);
    // This puts the sensor to sleep, if necessary.
    // This also un-sets the _millisSensorActivated timestamp.
    // Does NOT power down the sensor!
    virtual bool sleep(void);

    // This tells the sensor to start a single measurement, if needed
    // This also sets the _millisMeasurementRequested timestamp.
    // This *may* require a waitForWarmUp() before measurement commands can be sent.
    // This *may* also require a waitForStability() before returned measurements will be any good.
    // The waits are NOT included in this function!
    virtual bool startSingleMeasurement(void);

    // This next function must be implemented for ever sensor!!
    // This actually gets the results from a single measurement
    // This also un-sets the _millisMeasurementRequested timestamp.
    // This *may* also require a waitForStability() before returned measurements will be any good.
    // This will often require a waitForMeasurementCompletion() to ensure a measurement is done.
    // The waits are NOT included in this function!
    virtual bool addSingleMeasurementResult(void) = 0;

    // This is the array of result values for each sensor
    float sensorValues[MAX_NUMBER_VARS];
    // Clears the values array
    void clearValues();
    // This verifies that a measurement is OK (ie, not -9999) before adding it to the array
    void verifyAndAddMeasurementResult(int resultNumber, float resultValue);
    void verifyAndAddMeasurementResult(int resultNumber, int resultValue);
    void averageMeasurements(void);

    // These tie the variables to their parent sensor
    void registerVariable(int varNum, Variable* var);
    // Notifies attached variables of new values
    void notifyVariables(void);

    // // This is the time that a value was last sent ot registered variables
    // // It is set in the notifyVariables() function.
    // // The "checkForUpdate()" function checks if values are older than 2 minutes.
    // uint32_t _sensorLastUpdated;
    // bool checkForUpdate(void);

    // The "isWarmedUp()" function checks whether or not enough time has passed
    // between the sensor receiving power and being ready to respond to logger
    // commands.  The "waitForWarmUp()" function delays until the time passes.
    // "checkPowerOn()" checks if the power pin is currently high
    bool checkPowerOn(bool debug=false);
    virtual bool isWarmedUp(bool debug=false);
    void waitForWarmUp(void);

    // The "isStable()" function checks whether or not enough time has passed
    // between the sensor being awoken/activated and being ready to output stable
    // values.  The "waitForStability()" function delays until the time passes.
    virtual bool isStable(bool debug=false);
    void waitForStability(void);

    // The "isMeasurementComplete()" function checks whether or not enough time
    // has passed between when the sensor was asked to take a single measurement
    // and when that measurement should be complete.  The
    // "waitForMeasurementCompletion()" function delays until the time passes.
    virtual bool isMeasurementComplete(bool debug=false);
    void waitForMeasurementCompletion(void);


protected:

    int8_t _dataPin;  // SIGNED int, to allow negative numbers for unused pins
    int8_t _powerPin;  // SIGNED int, to allow negative numbers for unused pins
    const char *_sensorName;
    uint8_t _numReturnedVars;
    uint8_t _measurementsToAverage;
    uint8_t numberGoodMeasurementsMade[MAX_NUMBER_VARS];

    // This is the time needed from the when a sensor has power until it's ready to talk
    // The _millisPowerOn value is set in the powerUp() function.  It is
    // un-set in the powerDown() function.
    uint32_t _warmUpTime_ms;
    uint32_t _millisPowerOn;

    // This is the time needed from the when a sensor is activated until the readings are stable
    // The _millisSensorActivated value is *usually* set in the wake() function,
    // but may also be set in the startSingleMeasurement() function.  It is
    // generally un-set in the sleep() function.
    uint32_t _stabilizationTime_ms;
    uint32_t _millisSensorActivated;

    // This is the time needed from the when a sensor is told to take a single
    // reading until that reading should be complete
    // The _millisMeasurementRequested value is set in the startSingleMeasurement() function.
    // It *may* be unset in the addSingleMeasurementResult() function.
    uint32_t _measurementTime_ms;
    uint32_t _millisMeasurementRequested;

    // This is an 8-bit code for the sensor status
    uint8_t _sensorStatus;

    // This is an array for each sensor containing the variable objects tied
    // to that sensor.  The MAX_NUMBER_VARS cannot be determined on a per-sensor
    // basis, because of the way memory is used on an Arduino.  It must be
    // defined once for the whole class.
    Variable *variables[MAX_NUMBER_VARS];
};

#endif
