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
#include "SensorBase.h"

// The main class for the Decagon CTD
class DecagonSDI12 : public virtual SensorBase
{
public:
    DecagonSDI12(char SDI12address, int powerPin, int dataPin, int numReadings = 1);
    // String getSensorName(void) override;
    String getSensorVendor(void);
    String getSensorModel(void);
    String getSensorVersion(void);
    String getSensorSerialNumber(void);
    String getSensorLocation(void) override;

    virtual bool update(void);

    virtual float getValue(void) = 0;
protected:
    void getSensorInfo(void);
    SENSOR_STATUS sensorStatus;
    String sensorLocation;
    static int numMeasurements;
    static float sensorValues[];
private:
    String _sensorName;
    String _sensorVendor;
    String _sensorModel;
    String _sensorVersion;
    String _sensorSerialNumber;
    char _SDI12address;
    int _numReadings;
    String myCommand;
    String sdiResponse;
};

#endif
