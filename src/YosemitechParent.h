/*
 *YosemitechParent.h
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
 * https://github.com/EnviroDIY/YosemitechModbus
*/

#ifndef YosemitechParent_h
#define YosemitechParent_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
// #define DEEP_DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include <YosemitechModbus.h>

// The main class for the Yosemitech Sensors
class YosemitechParent : public Sensor
{
public:
    YosemitechParent(byte modbusAddress, Stream* stream,
                     int8_t powerPin, int8_t powerPin2, int8_t enablePin = -1, uint8_t measurementsToAverage = 1,
                     yosemitechModel model = UNKNOWN, const char *sensName = "Yosemitech-Sensor", int numVariables = 2,
                     uint32_t warmUpTime_ms = 1500, uint32_t stabilizationTime_ms = 20000, uint32_t measurementTime_ms = 2000);
    YosemitechParent(byte modbusAddress, Stream& stream,
                     int8_t powerPin, int8_t powerPin2, int8_t enablePin = -1, uint8_t measurementsToAverage = 1,
                     yosemitechModel model = UNKNOWN, const char *sensName = "Yosemitech-Sensor", int numVariables = 2,
                     uint32_t warmUpTime_ms = 1500, uint32_t stabilizationTime_ms = 20000, uint32_t measurementTime_ms = 2000);

    String getSensorLocation(void) override;

    virtual bool setup(void) override;
    virtual bool wake(void) override;
    virtual bool sleep(void) override;

    // Override these to use two power pins
    virtual void powerUp(void) override;
    virtual void powerDown(void) override;

    virtual bool addSingleMeasurementResult(void);

private:
    yosemitech sensor;
    yosemitechModel _model;
    byte _modbusAddress;
    Stream* _stream;
    int _RS485EnablePin;
    int8_t _powerPin2;
};

#endif
