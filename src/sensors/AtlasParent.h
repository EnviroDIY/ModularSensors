/*
 *AtlasParent.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial developement for Atlas Sensors was done by Adam Gold
 *Files were edited by Sara Damiano
 *
 */

// Header Guards
#ifndef AtlasParent_h
#define AtlasParent_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

// A parent class for Atlas sensors
class AtlasParent : public Sensor
{
public:
    AtlasParent(int8_t powerPin, uint8_t i2cAddressHex = ATLAS_COND_I2C_ADDR, uint8_t measurementsToAverage = 1,
                const char *sensorName = "AtlasSensor", uint8_t numReturnedVars = 1,
                uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0);
    virtual ~AtlasParent();

    String getSensorLocation(void) override;

    bool setup(void) override;

    bool addSingleMeasurementResult(void) override;

protected:
    uint8_t _i2cAddressHex;
};

#endif  // Header Guard
