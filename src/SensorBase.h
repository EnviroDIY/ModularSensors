/*
 *SensorBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the sensor base class.
*/

// Header Guards
#ifndef SensorBase_h
#define SensorBase_h

// Debugging Statement
// #define MS_SENSORBASE_DEBUG

#ifdef MS_SENSORBASE_DEBUG
#define MS_DEBUGGING_STD "SensorBase"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include <pins_arduino.h>

// The largest number of variables from a single sensor
#define MAX_NUMBER_VARS 8


class Variable;  // Forward declaration
#include "PortExpanderB031.h"
class PortExpanderB031;

// Defines the "Sensor" Class
class Sensor
{
public:

    Sensor(const char *sensorName = "Unknown", const uint8_t numReturnedVars = 1,
           uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0,
           int8_t powerPin = -1, int8_t dataPin = -1, uint8_t measurementsToAverage = 1);
    virtual ~Sensor();

    // These functions are dependent on the constructor and return the constructor values
    // This gets the place the sensor is installed ON THE ARDUINO (ie, pin number)
    virtual String getSensorLocation(void);
    // This gets the name of the sensor.
    virtual String getSensorName(void);
    // This concatentates and returns the name and location.
    String getSensorNameAndLocation(void);
    // This gets the pin number for the power pin.
    virtual int8_t getPowerPin(void);

    // These functions get and set the number of readings to average for a sensor
    // Generally these values should be set in the constructor
    void setNumberMeasurementsToAverage(int nReadings);
    uint8_t getNumberMeasurementsToAverage(void);

    // This returns the 8-bit code for the current status of the sensor.
    // Bit 0 - 0=Has NOT been successfully set up, 1=Has been setup
    // Bit 1 - 0=No attempt made to power sensor, 1=Attempt made to power sensor
    // Bit 2 - 0=Power up attampt failed, 1=Power up attempt succeeded
    //       - Use the isWarmedUp() function to check if enough time has passed
    //         to be ready for sensor communication.
    // Bit 3 - 0=Activation/wake attempt made, 1=No activation/wake attempt made
    //       - check _millisSensorActivated or bit 4 to see if wake() attempt was successful
    //       - a failed activation attempt will give _millisSensorActivated = 0
    // Bit 4 - 0=Wake/Activate failed, 1=Is awake/actively measuring
    //       - Use the isStable() function to check if enough time has passed
    //         to begin a measurement.
    // Bit 5 - 0=Start measurement requested attempt made, 1=No measurements have been requested
    //       - check _millisMeasurementRequested or bit 6 to see if startSingleMeasurement() attempt was successful
    //       - a failed request attempt will give _millisMeasurementRequested = 0
    // Bit 6 - 0=Measurement start failed, 1=Measurement attempt succeeded
    //       - Use the isMeasurementComplete() to check if enough time has passed
    //         for a measurement to have been completed.
    // Bit 7 - 0=No known errors, 1=Some sort of error has occurred
    uint8_t getStatus(void);

    // This does any one-time preparations needed before the sensor will be able
    // to take readings.  May not require any action.
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
    // This is a string with a pretty-print of the values array
    // String getStringValueArray(void);
    // Clears the values array
    void clearValues();
    // This verifies that a measurement is OK (ie, not -9999) before adding it to the array
    void verifyAndAddMeasurementResult(uint8_t resultNumber, float resultValue);
    void verifyAndAddMeasurementResult(uint8_t resultNumber, int16_t resultValue);
    void averageMeasurements(void);

    // These tie the variables to their parent sensor
    void registerVariable(int sensorVarNum, Variable* var);
    // Notifies attached variables of new values
    void notifyVariables(void);

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
    const uint8_t _numReturnedVars;
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

#endif  // Header Guard
