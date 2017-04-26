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
#include "VariableArray.h"


// Defines the "Logger" Class
class LoggerBase : public VariableArray
{
public:
    // Setup and initialization function
    void init(int SDCardPin, int interruptPin,
              int variableCount,
              Variable *variableList[],
              float loggingIntervalMinutes,
              const char *loggerID = 0);
    // Sets the static timezone - this must be set
    static void setTimeZone(int timeZone);
    // This set the offset between the built-in clock and the time zone where
    // the data is being recorded.  If your RTC is set in UTC and your logging
    // timezone is EST, this should be -5.  If your RTC is set in EST and your
    // timezone is EST this does not need to be called.
    static void setTZOffset(int offset);
    // Sets up a pin for an LED or other way of alerting that data is being logged
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
    // This creates a header for the logger file
    virtual String generateFileHeader(void);
    // This generates a comma separated list of volues of sensor data - including the time
    String generateSensorDataCSV(void) override;
    // This initializes a file on the SD card and writes a header to it
    void setupLogFile(void);
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
    String _fileName;

    // Static variables - identical for EVERY logger
    static uint8_t _numReadings;
    static int _timeZone;
    static int _offset;

    // Initialization variables
    int _SDCardPin;
    int _interruptPin;
    const char *_loggerID;
    float _loggingIntervalMinutes;
    int _interruptRate;
    bool _sleep;
    int _ledPin;

    // Time stamps - want to set them at a single time and carry them forward
    long markedEpochTime;
    DateTime markedDateTime;
    char markedISO8601Time[26];

private:
    // Private functions for the timer and sleep modes
    static void checkTime(uint32_t ts);
    static void wakeISR(void);

    bool _autoFileName;
    bool _isFileNameSet;
};



#endif
