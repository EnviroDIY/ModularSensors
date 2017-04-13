/*
 *LoggerEnviroDIY.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#ifndef LoggerEnviroDIY_h
#define LoggerEnviroDIY_h

#include <Arduino.h>
#include "SensorBase.h"
#include "LoggerBase.h"
#include "ModemSupport.h"

// Defines the "Logger" Class
class LoggerEnviroDIY : public virtual LoggerBase
{
public:
    // Set up communications
    void setToken(const char *registrationToken);
    void setSamplingFeature(const char *samplingFeature);
    void setUUIDs(const char *UUIDs[]);

    static loggerModem modem;

    // This adds extra data to the datafile header
    String generateFileHeader(void) override;

    // Public functions to generate data formats
    String generateSensorDataJSON(void);

    // Public function to send data
    int postDataEnviroDIY(void);

    // Convience functions to do it all
    virtual void log(void) override;

private:
    // Communication functions
    void streamEnviroDIYRequest(Stream *stream);

    // Tokens and UUID's for EnviroDIY
    const char *_registrationToken;
    const char *_samplingFeature;
    const char **_UUIDs;
};

#endif
