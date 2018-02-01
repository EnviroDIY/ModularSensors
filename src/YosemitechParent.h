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

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"

// The main class for the Decagon CTD
class YosemitechParent : public Sensor
{
public:
    YosemitechParent(byte modbusAddress, int powerPin,
                     Stream* stream, int enablePin = -1, int numReadings = 1,
                     String sensName = "Yosemitech-Sensor", int numMeasurements = 2,
                     yosemitechModel model = UNKNOWN, int WarmUpTime_ms = 1500,
                     int StabilizationTime_ms = 20000, int remeasurementTime_ms = 2000);
    YosemitechParent(byte modbusAddress, int powerPin,
                     Stream& stream, int enablePin = -1, int numReadings = 1,
                     String sensName = "Yosemitech-Sensor", int numMeasurements = 2,
                     yosemitechModel model = UNKNOWN, int WarmUpTime_ms = 1500,
                     int StabilizationTime_ms = 20000, int remeasurementTime_ms = 2000);

    String getSensorLocation(void) override;

    virtual SENSOR_STATUS setup(void) override;
    virtual bool wake(void) override;

    virtual bool update(void);

private:
    void waitForStability(void);
    yosemitechModel _model;
    byte _modbusAddress;
    Stream* _stream;
    int _enablePin;
    int _numReadings;
    yosemitech sensor;
    int _remeasurementTime_ms;
    bool _isTakingMeasurements;
    uint32_t _millisMeasurementStarted;
    uint32_t _StabilizationTime_ms;
};

#endif
