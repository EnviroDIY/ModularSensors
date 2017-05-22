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

// #define SENSORS_DBG Serial

#ifdef SENSORS_DBG
namespace {
 template<typename T>
 static void DBGS(T last) {
   SENSORS_DBG.print(last);
 }

 template<typename T, typename... Args>
 static void DBGS(T head, Args... tail) {
   SENSORS_DBG.print(head);
   DBGS(tail...);
 }
}
#else
 #define DBGS(...)
#endif

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

    Sensor(int powerPin = -1, int dataPin = -1, String sensorName = "Unknown", int numReturnedVars = 1, int WarmUpTime_ms = 0);

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
    // This puts the sensor to sleep, if necessary.  Defaults to True.
    virtual bool sleep(void);
    // This wakes the sensor up, if necessary.  Defaults to True.
    virtual bool wake(void);

    // These next functions must be implemented for ever sensor
    // This updates the sensor's values
    virtual bool update(void) = 0;

    // These tie the variables to their parent sensor
    virtual void registerVariable(int varNum, Variable* var);
    virtual void notifyVariables(void);
    float sensorValues[MAX_NUMBER_VARS];

    // This just makes sure things are up-to-date
    bool checkForUpdate(unsigned long sensorLastUpdated);
    unsigned long sensorLastUpdated;

protected:
    bool checkPowerOn(void);
    void powerUp(void);
    void powerDown(void);
    void waitForWarmUp(void);
    void clearValues();
    int _dataPin;
    int _powerPin;
    String _sensorName;
    int _numReturnedVars;
    uint32_t _WarmUpTime_ms;
    uint32_t _millisPowerOn;
    SENSOR_STATUS sensorStatus;
    Variable *variables[MAX_NUMBER_VARS];
};

#endif
