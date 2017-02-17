// Sensor.h

#ifndef _SENSOR_h
#define _SENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

enum SENSOR_STATUS
{
    SENSOR_ERROR,
    SENSOR_READY,
    SENSOR_WAITING,
    SENSOR_UNKNOWN
};


// Defines the "SensorBase" Class
class SensorBase
{
public:
    SensorBase(void);
    virtual ~SensorBase(void);
    virtual SENSOR_STATUS setup(void);
    virtual SENSOR_STATUS getStatus(void);
    virtual bool sleep(void);
    virtual bool wake(void);

    virtual bool update(void) = 0;
    virtual String getSensorName(void) = 0;

    virtual String getVarName(void) = 0;
    virtual String getVarUnit(void) = 0;
    virtual float getValue(void) = 0;
protected:
    SENSOR_STATUS sensorStatus;
    String sensorName;
    String varName;
    String unit;
    float sensorValue;
};

// The constructor
SensorBase::SensorBase(void)
{
    setup();
}

// The destructor
SensorBase::~SensorBase(void) {}

// The function to set up connection to a sensor.
// By default, returns ready
SENSOR_STATUS SensorBase::setup(void)
{
    return SENSOR_READY;
}

// The function to return the status of a sensor
// By default, returns ready
SENSOR_STATUS SensorBase::getStatus(void)
{
    return sensorStatus;
}

// The function to put a sensor to sleep
// By default, returns true
bool SensorBase::sleep(void)
{
    return true;
}

// The function to wake up a sensor
// By default, returns true
bool SensorBase::wake(void)
{
    return true;
}
#endif
