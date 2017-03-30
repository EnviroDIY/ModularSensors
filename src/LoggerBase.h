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

    // ===================================================================== //
    // Public functions to access the clock in proper format and time zone
    // ===================================================================== //
    // This gets the current epoch time and corrects it for the specified time zone
    static uint32_t getNow(void);
    // This converts a date-time object into a ISO8601 formatted string
    static String formatDateTime_ISO8601(DateTime dt, int timeZone);
    // This converts an epoch time into a ISO8601 formatted string
    static String formatDateTime_ISO8601(uint32_t epochTime, int timeZone);
    // This checks to see if the current time is an even interval of the logging rate
    bool checkInterval(void);
    // This sets static variables for the date/time - this is needed so that all
    // data outputs (SD, EnviroDIY, serial printing, etc) print the same time
    // for updating the sensors - even though the routines to update the sensors
    // and to output the data may take several seconds.
    // It is not currently possible to output the instantaneous time an individual
    // sensor was updated, just a single marked time.  By custom, this should be
    // called before updating the sensors, not after.
    void markTime(void);

    // Public functions for the timer and sleep modes
    void setupTimer(void);
    void setupSleep(void);
    void systemSleep(void);

    // Public functions for logging data
    void setFileName(char *fileName);
    void setFileName(void);
    String getFileName(void);
    void setupLogFile(void);
    String generateSensorDataCSV(void);
    void logToSD(String rec);

    // Convience functions to do it all
    virtual void begin(void);
    virtual void log(void);

protected:
    static long markedEpochTime;
    static DateTime markedDateTime;
    static char markedISO8601Time[26];

    RTCTimer timer;
    static bool sleep;

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
    // static char *_fileName;
private:
    // Private functions for the timer and sleep modes
    static void checkTime(uint32_t ts);
    static void wakeISR(void);

    bool _autoFileName;
};



#endif
