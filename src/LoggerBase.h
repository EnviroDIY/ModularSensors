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
class LoggerBase
{
public:
    // The class constructor
    void init(int timeZone, int SDCardPin, int sensorCount,
              SensorBase *SENSOR_LIST[],
              const char *loggerID = 0,
              const char *samplingFeature = 0,
              const char *UUIDs[] = 0);

    // Public functions to access the clock in proper format and time zone
    static uint32_t getNow(void);
    static String getDateTime_ISO8601(void);
    static void showTime(uint32_t ts);

    // Public functions for interfacing with a list of sensors
    bool setupSensors(void);  // This sets up all of the sensors in the list
    String checkSensorLocations(void);  // This checks where each sensor is attached
    bool updateAllSensors(void);

    // Public functions for logging data
    void setupLogFile(void);
    String generateSensorDataCSV(void);
    void logToSD(String rec);

    // Convience functions to do it all
    void setup(int interruptPin = -1, uint8_t periodicity = EveryMinute);
    virtual void log(int loggingIntervalMinutes, int ledPin = -1);

protected:
    static char currentTime[26];
    static long currentepochtime;
    RTCTimer timer;
    static bool sleep;

    // Private functions for the timer and sleep modes
    static void checkTime(uint32_t ts);
    static void wakeISR(void);
    void setupTimer(uint32_t period);

    void setupSleep(int interruptPin, uint8_t periodicity = EveryMinute);
    bool sensorsSleep(void);
    bool sensorsWake(void);
    void systemSleep(void);

private:
    static int _timeZone;
    int _SDCardPin;
    SensorBase **_sensorList;
    uint8_t _sensorCount;
    const char *_loggerID;
    const char *_samplingFeature;
    const char **_UUIDs;

    static String _fileName;
};



#endif
