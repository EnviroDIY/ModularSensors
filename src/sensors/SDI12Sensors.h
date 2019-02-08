/*
 *SDI12Sensors.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for all Devices that communicate over SDI-12
 *It is dependent on the EnviroDIY SDI-12 library.
 *
 *Documentation for the SDI-12 Protocol commands and responses can be found at:
 *http://www.sdi-12.org/
*/

// Header Guards
#ifndef SDI12Sensors_h
#define SDI12Sensors_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"
#ifdef SDI12_EXTERNAL_PCINT
#include <SDI12.h>
#else
#include <SDI12_ExtInts.h>
#endif
// NOTE:  Can use the "regular" sdi-12 library with build flag -D SDI12_EXTERNAL_PCINT
// Unfortunately, that is not compatible with the Arduino IDE

// The main class for SDI-12 Sensors
class SDI12Sensors : public Sensor
{
public:

    SDI12Sensors(char SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage = 1,
                 const char *sensorName = "SDI12-Sensor", uint8_t numReturnedVars = 1,
                 uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0);
    SDI12Sensors(char *SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage = 1,
                 const char *sensorName = "SDI12-Sensor", uint8_t numReturnedVars = 1,
                 uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0);
    SDI12Sensors(int SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage = 1,
                 const char *sensorName = "SDI12-Sensor", uint8_t numReturnedVars = 1,
                 uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0);
    virtual ~SDI12Sensors();

    String getSensorVendor(void);
    String getSensorModel(void);
    String getSensorVersion(void);
    String getSensorSerialNumber(void);
    String getSensorLocation(void) override;

    virtual bool setup(void) override;

    virtual bool startSingleMeasurement(void);
    virtual bool addSingleMeasurementResult(void);

protected:
    bool requestSensorAcknowledgement(void);
    bool getSensorInfo(void);
    SDI12 _SDI12Internal;
    char _SDI12address;

private:
    String _sensorVendor;
    String _sensorModel;
    String _sensorVersion;
    String _sensorSerialNumber;
};

#endif  // Header Guard
