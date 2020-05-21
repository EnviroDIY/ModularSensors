/**
 * @file AtlasParent.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AtlasParent sensor subclass which is itself the
 * parent class for all Atlas sensors.
 *
 * This depends on the Arduino core Wire library.
 *
 * @note Most Atlas I2C commands have a 300ms processing time from the time the
 * command is written until it is possible to request a response or result,
 * except for the commands to take a calibration point or a reading which have a
 * 600ms processing/response time.
 */

// Header Guards
#ifndef SRC_SENSORS_ATLASPARENT_H_
#define SRC_SENSORS_ATLASPARENT_H_

// Debugging Statement
// #define MS_ATLASPARENT_DEBUG

#ifdef MS_ATLASPARENT_DEBUG
#define MS_DEBUGGING_STD "AtlasParent"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

// A parent class for Atlas sensors
class AtlasParent : public Sensor {
 public:
    AtlasParent(int8_t powerPin, uint8_t i2cAddressHex,
                uint8_t       measurementsToAverage = 1,
                const char*   sensorName            = "AtlasSensor",
                const uint8_t numReturnedVars = 1, uint32_t warmUpTime_ms = 0,
                uint32_t stabilizationTime_ms = 0,
                uint32_t measurementTime_ms   = 0);
    virtual ~AtlasParent();

    String getSensorLocation(void) override;

    bool setup(void) override;
    // NOTE:  The sensor should wake as soon as any command is sent.
    // I assume that means we can use the command to take a reading to both
    // wake it and ask for a reading.
    // bool wake(void) override;
    bool sleep(void) override;

    bool startSingleMeasurement(void) override;
    bool addSingleMeasurementResult(void) override;

 protected:
    int8_t _i2cAddressHex;
    // Wait for a command to process
    // NOTE:  This should ONLY be used as a wait when no response is
    // expected except a status code - the response will be "consumed"
    // and become unavailable.
    bool waitForProcessing(uint32_t timeout = 1000L);
};

#endif  // SRC_SENSORS_ATLASPARENT_H_
