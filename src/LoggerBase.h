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
#include <SdFat.h>  // To communicate with the SD card
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
              const char *loggerID = 0);
    void setAlertPin(int ledPin);

    // The timer
    RTCTimer loggerTimer;

    // ===================================================================== //
    // Public functions to access the clock in proper format and time zone
    // ===================================================================== //
    // This gets the current epoch time and corrects it for the specified time zone
    static uint32_t getNow(void);
    // This converts a date-time object into a ISO8601 formatted string
    static String formatDateTime_ISO8601(DateTime dt);
    // This converts an epoch time into a ISO8601 formatted string
    static String formatDateTime_ISO8601(uint32_t epochTime);
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

    // ===================================================================== //
    // Public functions for the timer and sleep modes
    // ===================================================================== //
    void setupTimer(void);
    void setupSleep(void);
    void systemSleep(void);

    // ===================================================================== //
    // Public functions for logging data
    // ===================================================================== //
    // This sets a file name, if you want to decide on it in advance
    void setFileName(char *fileName);
    void setFileName(String fileName);
    // This generates a file name from the logger id and the current date
    void setFileName(void);
    // This returns the current filename.  Must be run after setFileName.
    String getFileName(void);
    // This initializes a file on the SD card and writes a header to it
    virtual void setupLogFile(void);
    // This generates a comma separated list of volues of sensor data - including the time
    String generateSensorDataCSV(void);
    // This writes a record to the SD card
    void logToSD(String rec);

    // ===================================================================== //
    // Convience functions to do it all
    // ===================================================================== //
    virtual void begin(void);
    virtual void log(void);


// ===================================================================== //
// Things that are private and protected below here
// ===================================================================== //
protected:

    // The SD card and file
    SdFat sd;
    SdFile logFile;

    static long markedEpochTime;
    static DateTime markedDateTime;
    static char markedISO8601Time[26];

    bool sleep;

    static int _timeZone;
    int _SDCardPin;
    int _interruptPin;
    const char *_loggerID;

    float _loggingIntervalMinutes;
    int _interruptRate;
    int _ledPin;

    static String _fileName;
    static uint8_t _numReadings;
private:
    // Private functions for the timer and sleep modes
    static void checkTime(uint32_t ts);
    static void wakeISR(void);

    bool _autoFileName;
};



#endif
