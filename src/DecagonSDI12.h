/*
 *DecagonSDI12.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for all Decagon Devices that communicate with SDI-12
 *It is dependent on the EnviroDIY SDI-12 library.
 *
 *Documentation for the SDI-12 Protocol commands and responses
 *for the Decagon CTD-10 can be found at:
 * http://manuals.decagon.com/Integration%20Guides/CTD%20Integrators%20Guide.pdf
 *for the Decagon ES-2 Electrical Conductivity and Temperature:
 * http://manuals.decagon.com/Integration%20Guides/ES-2%20Integrators%20Guide.pdf
 *for the Decagon 5TM Soil Moisture and Temperature:
 * http://manuals.decagon.com/Integration%20Guides/5TM%20Integrators%20Guide.pdf
*/

#ifndef DecagonSDI12_h
#define DecagonSDI12_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include <SDI12_ExtInts.h>

// The main class for the Decagon CTD
class DecagonSDI12 : public Sensor
{
public:

    DecagonSDI12(char SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage = 1,
                 String sensorName = "SDI12-Sensor", uint8_t numReturnedVars = 1,
                 uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t remeasurementTime_ms = 0);
    DecagonSDI12(char *SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage = 1,
                 String sensorName = "SDI12-Sensor", uint8_t numReturnedVars = 1,
                 uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t remeasurementTime_ms = 0);
    DecagonSDI12(int SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage = 1,
                 String sensorName = "SDI12-Sensor", uint8_t numReturnedVars = 1,
                 uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0, uint32_t remeasurementTime_ms = 0);

    String getSensorVendor(void);
    String getSensorModel(void);
    String getSensorVersion(void);
    String getSensorSerialNumber(void);
    String getSensorLocation(void) override;

    virtual SENSOR_STATUS setup(void) override;
    virtual SENSOR_STATUS getStatus(void) override;

    virtual bool startSingleMeasurement(void);
    virtual bool addSingleMeasurementResult(void);

protected:
    bool getSensorInfo(void);
    SDI12 _SDI12Internal;

private:
    String _sensorVendor;
    String _sensorModel;
    String _sensorVersion;
    String _sensorSerialNumber;
    char _SDI12address;
};

#endif
