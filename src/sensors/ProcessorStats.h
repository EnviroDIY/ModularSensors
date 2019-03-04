/*
 *ProcessorStats.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for metadata on the processor functionality.
 *
 *For battery voltage:
 *  Range of 0-5V with 10bit ADC - resolution of 0.005
 *
 * If the processor is awake, it's ready to take a reading.
*/

// Header Guards
#ifndef ProcessorStats_h
#define ProcessorStats_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
#define PROCESSOR_NUM_VARIABLES 3
#define PROCESSOR_WARM_UP_TIME_MS 0
#define PROCESSOR_STABILIZATION_TIME_MS 0
#define PROCESSOR_MEASUREMENT_TIME_MS 0

#define PROCESSOR_BATTERY_RESOLUTION 3
#define PROCESSOR_BATTERY_VAR_NUM 0

#define PROCESSOR_RAM_RESOLUTION 0
#define PROCESSOR_RAM_VAR_NUM 1

#define PROCESSOR_SAMPNUM_RESOLUTION 0
#define PROCESSOR_SAMPNUM_VAR_NUM 2


// The "Main" class for the Processor
// Only need a sleep and wake since these DON'T use the default of powering up and down
class ProcessorStats : public Sensor
{
public:
    // Need to know the Mayfly version because the battery resistor depends on it
    ProcessorStats(const char *version);
    ~ProcessorStats();

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

private:
    const char *_version;
    int8_t _batteryPin;
    int16_t sampNum;
};


// For the battery supplying power to the processor
class ProcessorStats_Batt : public Variable
{
public:
    ProcessorStats_Batt(Sensor *parentSense,
                        const char *uuid = "",
                        const char *varCode = "Battery")
      : Variable(parentSense,
                 (const uint8_t)PROCESSOR_BATTERY_VAR_NUM,
                 (uint8_t)PROCESSOR_BATTERY_RESOLUTION,
                 "batteryVoltage", "volt",
                 varCode, uuid)
    {}
    ProcessorStats_Batt()
      : Variable((const uint8_t)PROCESSOR_BATTERY_VAR_NUM,
                 (uint8_t)PROCESSOR_BATTERY_RESOLUTION,
                 "batteryVoltage", "volt", "Battery")
    {}
    ~ProcessorStats_Batt(){}
};


// Defines the "Free Ram" This is a board diagnostic
class ProcessorStats_FreeRam : public Variable
{
public:
    ProcessorStats_FreeRam(Sensor *parentSense,
                           const char *uuid = "",
                           const char *varCode = "FreeRam")
      : Variable(parentSense,
                 (const uint8_t)PROCESSOR_RAM_VAR_NUM,
                 (uint8_t)PROCESSOR_RAM_RESOLUTION,
                 "freeSRAM", "Bit",
                 varCode, uuid)
    {}
    ProcessorStats_FreeRam()
      : Variable((const uint8_t)PROCESSOR_RAM_VAR_NUM,
                 (uint8_t)PROCESSOR_RAM_RESOLUTION,
                 "freeSRAM", "Bit", "FreeRam")
    {}
    ~ProcessorStats_FreeRam(){}
};


// Defines the "Sample Number" This is a board diagnostic
class ProcessorStats_SampleNumber : public Variable
{
public:
    ProcessorStats_SampleNumber(Sensor *parentSense,
                                const char *uuid = "",
                                const char *varCode = "SampNum")
      : Variable(parentSense,
                 (const uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
                 (uint8_t)PROCESSOR_SAMPNUM_RESOLUTION,
                 "sequenceNumber", "Dimensionless",
                 varCode, uuid)
    {}
    ProcessorStats_SampleNumber()
      : Variable((const uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
                 (uint8_t)PROCESSOR_SAMPNUM_RESOLUTION,
                 "sequenceNumber", "Dimensionless", "SampNum")
    {}
    ~ProcessorStats_SampleNumber(){}
};

#endif  // Header Guard
