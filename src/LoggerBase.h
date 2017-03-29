/*
 *LoggerBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the basic logging functions - ie, saving to an SD card.
*/

#ifndef LoggerBase_h
#define LoggerBase_h


#include <Arduino.h>
#include <Sodaq_DS3231.h>  // To communicate with the clock
#include <RTCTimer.h>  // To handle timing on a schedule
#include "SensorBase.h"


// Defines the "Logger" Class
class LoggerBase : public virtual SensorArray
{
public:
    // Setup and initialization function
    void init(int timeZone, int SDCardPin, int interruptPin,
              int sensorCount,
              SensorBase *SENSOR_LIST[],
              float loggingIntervalMinutes,
              const char *loggerID = 0,
              const char *samplingFeature = 0,
              const char *UUIDs[] = 0);
    void setAlertPin(int ledPin);

    // Public functions to access the clock in proper format and time zone
    static uint32_t getNow(void);
    static String getDateTime_ISO8601(void);
    static void showTime(uint32_t ts);

    // Public functions for the timer and sleep modes
    void setupTimer(void);
    bool checkInterval(void);
    void setupSleep(void);
    void systemSleep(void);

    // Public functions for logging data
    void setupLogFile(void);
    String generateSensorDataCSV(void);
    void logToSD(String rec);

    // Convience functions to do it all
    virtual void begin(void);
    virtual void log(void);

protected:
    static char logTime[26];
    static long currentepochtime;
    RTCTimer timer;
    static bool sleep;

    // Private functions for the timer and sleep modes
    static void checkTime(uint32_t ts);
    static void wakeISR(void);

    static int _timeZone;
    int _SDCardPin;
    int _interruptPin;
    const char *_loggerID;
    const char *_samplingFeature;
    const char **_UUIDs;

    float _loggingIntervalMinutes;
    int _interruptRate;
    int _ledPin;

    static String _fileName;
    // static char _fileName[];
};



#endif
