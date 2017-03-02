/*
 *DecagonSDI12.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
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
    String getSensorName(void) override;
    String getSensorLocation(void) override;

    SENSOR_STATUS setup(void) override;

    bool sleep(void) override;
    bool wake(void) override;

    virtual bool update(void);

    virtual float getValue(void) = 0;
protected:
    SENSOR_STATUS sensorStatus;
    String sensorName;
    String sensorLocation;
    static int numMeasurements;
    static float sensorValues[];
private:
    char _SDI12address;
    int _powerPin;
    int _dataPin;
    int _numReadings;
    String myCommand;
    String sdiResponse;
};

#endif
