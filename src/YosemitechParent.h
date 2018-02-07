/*
 *Yosemitech.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for all Yosemitech sensors that communicate via Modbus and are
 *set up in the EnviroDIY Yosemitech library.
 *
 *Documentation for the Yosemitech Protocol commands and responses, along with
 *information about the various variables, can be found
 *in the EnviroDIY Yosemitech library at:
 * https://github.com/EnviroDIY/Yosemitech
*/

#ifndef YosemitechParent_h
#define YosemitechParent_h

#include <Arduino.h>
#include <YosemitechModbus.h>
#include "SensorBase.h"

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

// The main class for the Yosemitech Sensors
class YosemitechParent : public Sensor
{
public:
    YosemitechParent(byte modbusAddress, Stream* stream,
                     int powerPin, int enablePin = -1, int measurementsToAverage = 1,
                     yosemitechModel model = UNKNOWN, String sensName = "Yosemitech-Sensor", int numVariables = 2,
                     int warmUpTime_ms = 1500, int stabilizationTime_ms = 20000, int remeasurementTime_ms = 2000);
    YosemitechParent(byte modbusAddress, Stream& stream,
                     int powerPin, int enablePin = -1, int measurementsToAverage = 1,
                     yosemitechModel model = UNKNOWN, String sensName = "Yosemitech-Sensor", int numVariables = 2,
                     int warmUpTime_ms = 1500, int stabilizationTime_ms = 20000, int remeasurementTime_ms = 2000);

    String getSensorLocation(void) override;

    virtual SENSOR_STATUS setup(void) override;
    virtual bool wake(void) override;
    virtual bool sleep(void) override;

    virtual bool startSingleMeasurement(void);
    virtual bool addSingleMeasurementResult(void);

private:
    yosemitechModel _model;
    byte _modbusAddress;
    Stream* _stream;
    int _RS485EnablePin;
    yosemitech sensor;
};

#endif
