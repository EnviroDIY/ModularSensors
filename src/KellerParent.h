/*
 *KellerParent.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 *This file is for Modbus communication to  Keller pressure and water level sensors
 *of Series 30, Class 5, Group 20 (such as the KellerAcculevel)
 *that are Software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week)
 *Only tested the Acculevel
 *
 *Documentation for the Keller commands and responses, along with
 *information about the various variables, can be found
 *in the EnviroDIY KellerModbus library at:
 * https://github.com/EnviroDIY/KellerModbus
*/

#ifndef KellerParent_h
#define KellerParent_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
// #define DEEP_DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include <KellerModbus.h>

// The main class for the Keller Sensors
class KellerParent : public Sensor
{
public:
    KellerParent(byte modbusAddress, Stream* stream,
             int8_t powerPin, int8_t enablePin = -1, uint8_t measurementsToAverage = 1,
             kellerModel model = UNKNOWN, String sensName = "Keller-Sensor", int numVariables = 3,
             int warmUpTime_ms = 500, int stabilizationTime_ms = 5000, int measurementTime_ms = 1500);
    KellerParent(byte modbusAddress, Stream& stream,
             int8_t powerPin, int8_t enablePin = -1, uint8_t measurementsToAverage = 1,
             kellerModel model = UNKNOWN, String sensName = "Keller-Sensor", int numVariables = 3,
             int warmUpTime_ms = 500, int stabilizationTime_ms = 5000, int measurementTime_ms = 1500);

    String getSensorLocation(void) override;

    virtual bool setup(void) override;
    virtual bool wake(void) override;
    virtual bool sleep(void) override;

    virtual bool addSingleMeasurementResult(void);

private:
    kellerModel _model;
    byte _modbusAddress;
    Stream* _stream;
    int _RS485EnablePin;
    keller sensor;
};

#endif
