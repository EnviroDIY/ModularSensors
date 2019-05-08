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

// Header Guards
#ifndef KellerParent_h
#define KellerParent_h

// Debugging Statement
// #define MS_KELLERPARENT_DEBUG
// #define MS_KELLERPARENT_DEBUG_DEEP

#ifdef MS_KELLERPARENT_DEBUG
#define MS_DEBUGGING_STD
#endif

#ifdef MS_KELLERPARENT_DEBUG_DEEP
#define MS_DEBUGGING_DEEP
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#include "SensorBase.h"
#include <KellerModbus.h>

// Sensor Specific Defines
#define KELLER_NUM_VARIABLES 3
#define KELLER_PRESSURE_VAR_NUM 0
#define KELLER_TEMP_VAR_NUM 1
#define KELLER_HEIGHT_VAR_NUM 2

// The main class for the Keller Sensors
class KellerParent : public Sensor
{
public:
    KellerParent(byte modbusAddress, Stream* stream,
                 int8_t powerPin, int8_t powerPin2, int8_t enablePin = -1, uint8_t measurementsToAverage = 1,
                 kellerModel model = OTHER, const char *sensName = "Keller-Sensor", uint8_t numVariables = 3,
                 uint32_t warmUpTime_ms = 500, uint32_t stabilizationTime_ms = 5000, uint32_t measurementTime_ms = 1500);
    KellerParent(byte modbusAddress, Stream& stream,
                 int8_t powerPin, int8_t powerPin2, int8_t enablePin = -1, uint8_t measurementsToAverage = 1,
                 kellerModel model = OTHER, const char *sensName = "Keller-Sensor", uint8_t numVariables = 3,
                 uint32_t warmUpTime_ms = 500, uint32_t stabilizationTime_ms = 5000, uint32_t measurementTime_ms = 1500);
    virtual ~KellerParent();

    String getSensorLocation(void) override;

    virtual bool setup(void) override;

    // Override these to use two power pins
    virtual void powerUp(void) override;
    virtual void powerDown(void) override;

    virtual bool addSingleMeasurementResult(void);

private:
    keller sensor;
    kellerModel _model;
    byte _modbusAddress;
    Stream* _stream;
    int8_t _RS485EnablePin;
    int8_t _powerPin2;
};

#endif  // Header Guard
