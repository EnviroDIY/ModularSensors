/**
 * @file YosemitechParent.h
 * @brief Contains the YosemitechParent sensor subclass, itself a parent class
 * for all Yosemitech sensors that communicate via Modbus and are supported by
 * the EnviroDIY Yosemitech library.
 *
 * Documentation for the Yosemitech Protocol commands and responses, along with
 * information about the various variables, can be found in the EnviroDIY
 * Yosemitech library at: https://github.com/EnviroDIY/YosemitechModbus
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHPARENT_H_
#define SRC_SENSORS_YOSEMITECHPARENT_H_

// Debugging Statement
// #define MS_YOSEMITECHPARENT_DEBUG
// #define MS_YOSEMITECHPARENT_DEBUG_DEEP

#ifdef MS_YOSEMITECHPARENT_DEBUG
#define MS_DEBUGGING_STD "YosemitechParent"
#endif

#ifdef MS_YOSEMITECHPARENT_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "YosemitechParent"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "VariableBase.h"
#include "SensorBase.h"
#include <YosemitechModbus.h>

// The main class for the Yosemitech Sensors
class YosemitechParent : public Sensor {
 public:
    YosemitechParent(byte modbusAddress, Stream* stream, int8_t powerPin,
                     int8_t powerPin2, int8_t enablePin = -1,
                     uint8_t         measurementsToAverage = 1,
                     yosemitechModel model                 = UNKNOWN,
                     const char*     sensName = "Yosemitech-Sensor",
                     uint8_t numVariables = 2, uint32_t warmUpTime_ms = 1500,
                     uint32_t stabilizationTime_ms = 20000,
                     uint32_t measurementTime_ms   = 2000);
    YosemitechParent(byte modbusAddress, Stream& stream, int8_t powerPin,
                     int8_t powerPin2, int8_t enablePin = -1,
                     uint8_t         measurementsToAverage = 1,
                     yosemitechModel model                 = UNKNOWN,
                     const char*     sensName = "Yosemitech-Sensor",
                     uint8_t numVariables = 2, uint32_t warmUpTime_ms = 1500,
                     uint32_t stabilizationTime_ms = 20000,
                     uint32_t measurementTime_ms   = 2000);
    virtual ~YosemitechParent();

    String getSensorLocation(void) override;

    bool setup(void) override;
    bool wake(void) override;
    bool sleep(void) override;

    // Override these to use two power pins
    void powerUp(void) override;
    void powerDown(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    yosemitech      _ysensor;
    yosemitechModel _model;
    byte            _modbusAddress;
    Stream*         _stream;
    int8_t          _RS485EnablePin;
    int8_t          _powerPin2;
};

#endif  // SRC_SENSORS_YOSEMITECHPARENT_H_
