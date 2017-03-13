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
    GPRS = 0,  // Sodaq GPRSBee - 2G (GPRS) communication
    WIFI,  // Digi XBee S6B - WiFi communication
    // RADIO,
    // THREEG
} xbee;

// Defines the "Logger" Class
class LoggerEnviroDIY : public virtual LoggerBase
{
public:

    // For the server response
    enum HTTP_RESPONSE
    {
        HTTP_FAILURE = 0,
        HTTP_SUCCESS,
        HTTP_TIMEOUT,
        HTTP_FORBIDDEN,
        HTTP_SERVER_ERROR,
        HTTP_REDIRECT,
        HTTP_OTHER
    };

    // The class constructor
    void init(int timeZone, int SDCardPin, int sensorCount,
              SensorBase *SENSOR_LIST[],
              const char *loggerID = 0,
              const char *samplingFeature = 0,
              const char *UUIDs[] = 0);
    void setCommunication(xbee beeType = GPRS,
                          const char *registrationToken = "UNKNOWN",
                          const char *hostAddress = "data.envirodiy.org",
                          const char *APIEndpoint = "/api/data-stream/",
                          int serverTimeout = 15000,
                          const char *APN = "apn.konekt.io");

    // Public functions to generate data formats
    String generateSensorDataJSON(void);

    // Public function to send data
    void printRemainingChars(int timeDelay = 1, int timeout = 5000);
    void streamPostRequest(Stream & stream);
    int postDataWiFi(void);
    int postDataGPRS(void);
    void printPostResult(int result);


// Only allow these functions if you have a portal URL
#ifdef DreamHostURL
    String generateSensorDataDreamHost(void);
    int postDataDreamHost(void);
#endif

    // Convience functions to do it all
    void log(int loggingIntervalMinutes, int ledPin = -1) override;

protected:
    static char currentTime[26];
    static long currentepochtime;

private:
    int _timeZone;
    int _SDCardPin;
    SensorBase **_sensorList;
    uint8_t _sensorCount;
    const char *_loggerID;
    const char *_samplingFeature;
    const char **_UUIDs;

    const char *_registrationToken;
    const char *_hostAddress;
    const char *_APIEndpoint;
    int _serverTimeout;
    xbee _beeType;
    const char *_APN;
};


#endif
