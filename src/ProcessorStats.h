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

#ifndef ProcessorStats_h
#define ProcessorStats_h

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"

#define PROCESSOR_NUM_VARIABLES 2
#define PROCESSOR_WARM_UP_TIME_MS 0
#define PROCESSOR_STABILIZATION_TIME_MS 0
#define PROCESSOR_MEASUREMENT_TIME_MS 0

#define PROCESSOR_BATTERY_RESOLUTION 3
#define PROCESSOR_BATTERY_VAR_NUM 0

#define PROCESSOR_RAM_RESOLUTION 0
#define PROCESSOR_RAM_VAR_NUM 1


// The "Main" class for the Processor
// Only need a sleep and wake since these DON'T use the default of powering up and down
class ProcessorStats : public Sensor
{
public:
    // Need to know the Mayfly version because the battery resistor depends on it
    ProcessorStats(const char *version);

    String getSensorLocation(void) override;

    // Do nothing for the power down and sleep functions
    // We don't want the processor to go to sleep or power down with the sensors
    bool sleep(void) override;
    void powerDown(void) override;

    bool addSingleMeasurementResult(void) override;

private:
    const char *_version;
    int _batteryPin;
};


// For the battery supplying power to the processor
class ProcessorStats_Batt : public Variable
{
public:
    ProcessorStats_Batt(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, PROCESSOR_BATTERY_VAR_NUM,
                 F("batteryVoltage"), F("Volt"),
                 PROCESSOR_BATTERY_RESOLUTION,
                 F("Battery"), UUID, customVarCode)
    {}
};


// Defines the "Free Ram" This is a board diagnostic
class ProcessorStats_FreeRam : public Variable
{
public:
    ProcessorStats_FreeRam(Sensor *parentSense,
                           const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, PROCESSOR_RAM_VAR_NUM,
                 F("Free SRAM"), F("Bit"),
                 PROCESSOR_RAM_RESOLUTION,
                 F("FreeRam"), UUID, customVarCode)
    {}
};

#endif
