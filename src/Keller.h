/*
 *Keller.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 *This file is for Modbus communication to  Keller Series 30, Class 5, Group 20 sensors,
 *that are Software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week)
 *Only tested the Acculevel
 *
 *Documentation for the Yosemitech Protocol commands and responses, along with
 *information about the various variables, can be found
 *in the EnviroDIY KellerModbus library at:
 * https://github.com/EnviroDIY/KellerModbus
*/

#ifndef Keller_h
#define Keller_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
// #define DEEP_DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include <KellerModbus.h>

#define Keller_NUM_VARIABLES 3
#define Keller_WARM_UP_TIME_MS 500
#define Keller_STABILIZATION_TIME_MS 5000
#define Keller_MEASUREMENT_TIME_MS 1500

#define Keller_PRESSURE_RESOLUTION 5
#define Keller_PRESSURE_VAR_NUM 0

#define Keller_TEMP_RESOLUTION 2
#define Keller_TEMP_VAR_NUM 1

#define Keller_HEIGHT_RESOLUTION 4
#define Keller_HEIGHT_VAR_NUM 2


// The main class for the Keller Sensors
class Keller : public Sensor
{
public:
    Keller(byte modbusAddress, Stream* stream,
             int8_t powerPin, int8_t enablePin = -1, uint8_t measurementsToAverage = 1,
             String sensName = "Keller-Sensor", int numVariables,
             int warmUpTime_ms = 1500, int stabilizationTime_ms = 5000, int measurementTime_ms = 2000);
    Keller(byte modbusAddress, Stream& stream, int numVariables,
             int8_t powerPin, int8_t enablePin = -1, uint8_t measurementsToAverage = 1,
             String sensName = "Keller-Sensor",
             int warmUpTime_ms = 1500, int stabilizationTime_ms = 5000, int measurementTime_ms = 2000);



    String getSensorLocation(void) override;

    virtual bool setup(void) override;
    virtual bool wake(void) override;
    virtual bool sleep(void) override;

    virtual bool addSingleMeasurementResult(void);

private:
    byte _modbusAddress;
    Stream* _stream;
    int _RS485EnablePin;
    keller sensor;
};


// Defines the PressureGauge (vented & barometricPressure corrected) variable
class Keller_Pressure : public Variable
{
public:
    Keller_Pressure(Sensor *parentSense, String UUID = "", String customVarCode = "")
     : Variable(parentSense, Keller_PRESSURE_VAR_NUM,
                F("pressureGauge"), F("millibar"),
                Keller_PRESSURE_RESOLUTION,
                F("kellerPress"), UUID, customVarCode)
    {}
};


// Defines the Temperature Variable
class Keller_Temp : public Variable
{
public:
    Keller_Temp(Sensor *parentSense, String UUID = "", String customVarCode = "")
     : Variable(parentSense, Keller_TEMP_VAR_NUM,
                F("temperature"), F("degreeCelsius"),
                Keller_TEMP_RESOLUTION,
                F("kellerTemp"), UUID, customVarCode)
    {}
};

// Defines the gageHeight (Water level with regard to an arbitrary gage datum) Variable
class Keller_Height : public Variable
{
public:
    Keller_Height(Sensor *parentSense, String UUID = "", String customVarCode = "")
     : Variable(parentSense, Keller_HEIGHT_VAR_NUM,
                F("gageHeight"), F("meter"),
                Keller_HEIGHT_RESOLUTION,
                F("kellerHeight"), UUID, customVarCode)
    {}
};

#endif
