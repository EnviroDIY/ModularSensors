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

typedef enum SENSOR_STATUS
{
    SENSOR_ERROR,
    SENSOR_READY,
    SENSOR_WAITING,
    SENSOR_UNKNOWN
} SENSOR_STATUS;


class Variable;  // Forward declaration

// Defines the "Sensor" Class
class Sensor
{
public:

    Sensor(String sensorName = "Unknown", int numReturnedVars = 1,
           uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t remeasurementTime_ms = 0,
           int powerPin = -1, int dataPin = -1, int readingsToAverage = 1);

    // These functions are dependent on the constructor and return the constructor values
    // This gets the place the sensor is installed ON THE MAYFLY (ie, pin number)
    virtual String getSensorLocation(void);
    // This gets the name of the sensor.
    virtual String getSensorName(void);

    // These next functions have defaults.
    // This sets up the sensor, if necessary.  Defaults to ready.
    virtual SENSOR_STATUS setup(void);
    // This returns the current status of the sensor.  Defaults to ready.
    virtual SENSOR_STATUS getStatus(void);
    // This returns a string for the sensor status
    static String printStatus(SENSOR_STATUS stat);

    // This turns on the sensor power, if applicable
    virtual void powerUp(void);
    // This wakes the sensor up, if necessary.  Defaults is to power up.
    virtual bool wake(void);
    // This puts the sensor to sleep, if necessary.
    // Does NOT power down the sensor!
    virtual bool sleep(void);
    // This turns off the sensor power, if applicable
    virtual void powerDown(void);

    // These next functions must be implemented for ever sensor
    // This tells the sensor to start a single measurement
    virtual bool startSingleMeasurement(void) = 0;
    // This gets the results from a single measurement
    virtual bool addSingleMeasurementResult(void) = 0;

    // This updates the sensor's values
    // This includes clears the values array, starts and averages as many
    // measurement readings as requested, and then notifies the registerd
    // variables of the new resutls.
    virtual bool update(void);

    // These tie the variables to their parent sensor
    virtual void registerVariable(int varNum, Variable* var);
    virtual void notifyVariables(void);
    float sensorValues[MAX_NUMBER_VARS];

    // This just makes sure things are up-to-date
    bool checkForUpdate(uint32_t _sensorLastUpdated);
    uint32_t _sensorLastUpdated;

protected:
    // A helper to check if the power is already on
    bool checkPowerOn(void);
    // A helper to ensure that the sensor has had power long enough to communicate
    void waitForWarmUp(void);
    // A helper to ensure that the sensor is giving stable readings
    void waitForStability(void);
    // A helper to ensure that the sensor is ready to give a new value
    void waitForNextMeasurement(void);
    // Clears the values array
    void clearValues();

    int _dataPin;
    int _powerPin;
    String _sensorName;
    int _numReturnedVars;
    int _readingsToAverage;

    uint32_t _warmUpTime_ms;
    uint32_t _millisPowerOn;

    uint32_t _stabilizationTime_ms;
    bool _isTakingMeasurements;
    uint32_t _millisMeasurementStarted;

    uint32_t _remeasurementTime_ms;
    uint32_t _lastMeasurementRequested;

    SENSOR_STATUS sensorStatus;
    Variable *variables[MAX_NUMBER_VARS];
};

#endif
