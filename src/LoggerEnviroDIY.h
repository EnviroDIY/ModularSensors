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
#include "LoggerBase.h"
#include "SensorBase.h"


// For the "Bee" devices"
typedef enum xbee
{
    GPRSv4 = 0,  // Sodaq GPRSBee v4 - 2G (GPRS) communication
    GPRSv6,  // Sodaq GPRSBee v6 - 2G (GPRS) communication
    WIFI,  // Digi XBee S6B - WiFi communication
    // RADIO,
    // THREEG
} xbee;

// Defines the "Logger" Class
class LoggerEnviroDIY : public virtual LoggerBase
{
public:
    // Set up communications
    void setToken(const char *registrationToken);
    void setupBee(xbee beeType,
                  Stream *beeStream,
                  int beeCTSPin,
                  int beeDTRPin,
                  const char *APN);

    // Public functions to generate data formats
    String generateSensorDataJSON(void);

    // Public function to send data
    int postDataWiFi(void);
    int postDataGPRS(void);
    void printPostResult(int result);

    // Convience functions to do it all
    virtual void log(void) override;

protected:
    // Communication functions
    void dumpBuffer(Stream *stream, int timeDelay = 5, int timeout = 5000);
    void streamPostRequest(Stream *stream);

    const char *_registrationToken;
    xbee _beeType;
    Stream *_beeStream;
    const char *_APN;
};


#endif
