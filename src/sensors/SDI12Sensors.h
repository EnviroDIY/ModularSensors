/**
 * @file SDI12Sensors.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SDI12Sensors sensor subclass, itself a parent class for
 * all devices that communicate over SDI-12.
 *
 * This depends on the EnviroDIY SDI-12 library.
 *
 * Documentation for the SDI-12 Protocol commands and responses can be found at:
 * http://www.sdi-12.org/
 */

// Header Guards
#ifndef SRC_SENSORS_SDI12SENSORS_H_
#define SRC_SENSORS_SDI12SENSORS_H_

// Debugging Statement
// #define MS_SDI12SENSORS_DEBUG

#ifdef MS_SDI12SENSORS_DEBUG
#define MS_DEBUGGING_STD "SDI12Sensors"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#ifdef SDI12_EXTERNAL_PCINT
#include <SDI12.h>
#else
#include <SDI12_ExtInts.h>
#endif
// NOTE:  Can use the "regular" sdi-12 library with build flag -D
// SDI12_EXTERNAL_PCINT Unfortunately, that is not compatible with the Arduino
// IDE

// The main class for SDI-12 Sensors
class SDI12Sensors : public Sensor {
 public:
    SDI12Sensors(char SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t       measurementsToAverage = 1,
                 const char*   sensorName            = "SDI12-Sensor",
                 const uint8_t numReturnedVars = 1, uint32_t warmUpTime_ms = 0,
                 uint32_t stabilizationTime_ms = 0,
                 uint32_t measurementTime_ms   = 0);
    SDI12Sensors(char* SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t       measurementsToAverage = 1,
                 const char*   sensorName            = "SDI12-Sensor",
                 const uint8_t numReturnedVars = 1, uint32_t warmUpTime_ms = 0,
                 uint32_t stabilizationTime_ms = 0,
                 uint32_t measurementTime_ms   = 0);
    SDI12Sensors(int SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t       measurementsToAverage = 1,
                 const char*   sensorName            = "SDI12-Sensor",
                 const uint8_t numReturnedVars = 1, uint32_t warmUpTime_ms = 0,
                 uint32_t stabilizationTime_ms = 0,
                 uint32_t measurementTime_ms   = 0);
    virtual ~SDI12Sensors();

    String getSensorVendor(void);
    String getSensorModel(void);
    String getSensorVersion(void);
    String getSensorSerialNumber(void);
    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets the pin modes for the data and power pin, sets the stream
     * timeout time and value, and sets the status bit.  This also sets the
     * mcu's timer prescaler values to clock the serial communication.  Sensor
     * power is not required.
     *
     * @return **true** The setup was successful
     * @return **false** Some part of the setup failed
     */
    bool setup(void) override;

    bool startSingleMeasurement(void) override;
    bool addSingleMeasurementResult(void) override;

 protected:
    bool  requestSensorAcknowledgement(void);
    bool  getSensorInfo(void);
    SDI12 _SDI12Internal;
    char  _SDI12address;

 private:
    String _sensorVendor;
    String _sensorModel;
    String _sensorVersion;
    String _sensorSerialNumber;
};

#endif  // SRC_SENSORS_SDI12SENSORS_H_
