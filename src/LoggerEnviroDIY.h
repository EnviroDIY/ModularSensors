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
    void setupModem(modemType modType,
                  Stream *modemStream,
                  int vcc33Pin,
                  int status_CTS_pin,
                  int onoff_DTR_pin,
                  const char *APN);

    // This adds extra data to the datafile header
    virtual void setupLogFile(void);

    // Public functions to generate data formats
    String generateSensorDataJSON(void);

    // Public function to send data
    int postDataEnviroDIY(void);
    void printHTTPResult(int result);

    // Convience functions to do it all
    virtual void log(void) override;

protected:
    // Communication functions
    void dumpBuffer(Stream *stream, int timeDelay = 5, int timeout = 5000);
    void streamEnviroDIYRequest(Stream *stream);
    loggerModem modem;

private:
    // Tokens and UUID's for EnviroDIY
    const char *_registrationToken;
    const char *_samplingFeature;
    const char **_UUIDs;
};


#endif
