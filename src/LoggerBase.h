/*
 *LoggerBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the logging functions - ie, saving to an SD card.
*/

#ifndef LoggerBase_h
#define LoggerBase_h


#include <Arduino.h>
#include <Sodaq_DS3231.h>  // To communicate with the clock
#include "SensorBase.h"


// Defines the "Logger" Class
class Logger
{
public:
    // The class constructor
    Logger(int timeZone, int SDCardPin, SensorBase *SENSOR_LIST[],
           char *loggerID = 0,
           char *samplingFeature = 0,
           char *UUIDs[] = 0);

    // Public functions to access the clock in proper format and time zone
    static uint32_t getNow(void);
    String getDateTime_ISO8601(void);
    void showTime(uint32_t ts);

    // Public functions for interfacing with a list of sensors
    uint8_t countSensors(void);
    bool setupSensors(void);
    bool sensorsSleep(void);
    bool sensorsWake(void);
    bool updateAllSensors(void);

    static void checkTime(uint32_t ts);
    void setupTimer(uint32_t period);
    static void wakeISR(void);

    void setupSleep(int interruptPin, uint8_t periodicity = EveryMinute);
    void systemSleep(void);

    void setupLogFile(void);
    static String generateSensorDataCSV(void);
    void logData(String rec = generateSensorDataCSV());

    void Setup(int interruptPin = -1, uint8_t periodicity = EveryMinute);
    void Log(int loggingIntervalMinutes);

private:
    static int _timeZone;
    int _SDCardPin;
    static SensorBase **_sensorList;
    char *_loggerID;
    static uint8_t _sensorCount;
    char *_samplingFeature;
    char **_UUIDs;

    String _fileName;

    static char currentTime[26];
    static long currentepochtime;
    bool sleep;
};



#endif
