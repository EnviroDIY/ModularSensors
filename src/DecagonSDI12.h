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
#include <SDI12_PCINT3.h>

// #define MODULES_DBG Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"

// The main class for the Decagon CTD
class DecagonSDI12 : public Sensor
{
public:
    DecagonSDI12(char SDI12address, int powerPin, int dataPin,
                 int numReadings = 1,
                 String sensName = "SDI12-Sensor", int numMeasurements = 1);
    DecagonSDI12(char *SDI12address, int powerPin, int dataPin,
                 int numReadings = 1,
                 String sensName = "SDI12-Sensor", int numMeasurements = 1);
    DecagonSDI12(int SDI12address, int powerPin, int dataPin,
                 int numReadings = 1,
                 String sensName = "SDI12-Sensor", int numMeasurements = 1);

    String getSensorVendor(void);
    String getSensorModel(void);
    String getSensorVersion(void);
    String getSensorSerialNumber(void);
    String getSensorLocation(void) override;

    virtual bool update(void);
protected:
    void getSensorInfo(void);
private:
    String _sensorVendor;
    String _sensorModel;
    String _sensorVersion;
    String _sensorSerialNumber;
    char _SDI12address;
    int _numReadings;
};

#endif
