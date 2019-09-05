/*****************************************************************************
double_logger.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.23.6

This sketch is an example of logging data from different variables at two
different logging intervals.  This example uses more of the manual functions
in the logging loop rather than the simple "log" function.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts


// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The library version this example was written for
const char *libraryVersion = "0.23.6";
// The name of this file
const char *sketchName = "double_logger.ino";
// Logger ID - we're only using one logger ID for both "loggers"
const char *LoggerID = "XXXXX";
// The TWO filenames for the different logging intervals
const char *FileName5min = "Logger_5MinuteInterval.csv";
const char *FileName1min = "Logger_1MinuteInterval.csv";
// Your logger's timezone.
const int8_t timeZone = -5;  // Eastern Standard Time
// NOTE:  Daylight savings time will not be applied!  Please use standard time!


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <sensors/ProcessorStats.h>

const long serialBaud = 115200;   // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;        // MCU pin for the green LED (-1 if not applicable)
const int8_t redLED = 9;          // MCU pin for the red LED (-1 if not applicable)
const int8_t buttonPin = 21;      // MCU pin for a button to use to enter debugging mode  (-1 if not applicable)
const int8_t wakePin = A7;        // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPwrPin = -1;     // MCU SD card power pin (-1 if not applicable)
const int8_t sdCardSSPin = 12;      // MCU SD card chip select/slave select pin (must be given!)
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power (-1 if not applicable)

// Create the main processor chip "sensor" - for general metadata
const char *mcuBoardVersion = "v0.5b";
ProcessorStats mcuBoard(mcuBoardVersion);


// ==========================================================================
//    Wifi/Cellular Modem Settings
// ==========================================================================

// Create a reference to the serial port for the modem
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
HardwareSerial &modemSerial = Serial1;  // Use hardware serial if possible
// AltSoftSerial &modemSerial = altSoftSerial;  // For software serial if needed
// NeoSWSerial &modemSerial = neoSSerial1;  // For software serial if needed

// Modem Pins - Describe the physical pin connection of your modem to your board
const int8_t modemVccPin = -2;      // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemStatusPin = 19;   // MCU pin used to read modem status (-1 if not applicable)
const int8_t modemResetPin = 20;    // MCU pin connected to modem reset pin (-1 if unconnected)
const int8_t modemSleepRqPin = 23;  // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemLEDPin = redLED;  // MCU pin connected an LED to show modem status (-1 if unconnected)

// Network connection information
const char *wifiId = "xxxxx";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = "xxxxx";  // The password for connecting to WiFi, unnecessary for gprs

// For the Digi Wifi XBee (S6B)
#include <modems/DigiXBeeWifi.h>
const long modemBaud = 9600;  // All XBee's use 9600 by default
const bool useCTSforStatus = true;   // Flag to use the modem CTS pin for status
DigiXBeeWifi modemXBWF(&modemSerial,
                       modemVccPin, modemStatusPin, useCTSforStatus,
                       modemResetPin, modemSleepRqPin,
                       wifiId, wifiPwd);
// Create an extra reference to the modem by a generic name (not necessary)
DigiXBeeWifi modem = modemXBWF;


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);


// ==========================================================================
//    AOSong AM2315 Digital Humidity and Temperature Sensor
// ==========================================================================
#include <sensors/AOSongAM2315.h>

const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)

// Create and return the AOSong AM2315 sensor object
AOSongAM2315 am2315(I2CPower);


// ==========================================================================
//    Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
#include <VariableArray.h>

// The variables to record at 1 minute intervals
Variable *variableList_at1min[] = {
    new AOSongAM2315_Humidity(&am2315),
    new AOSongAM2315_Temp(&am2315)
};
// Count up the number of pointers in the 1-minute array
int variableCount1min = sizeof(variableList_at1min) / sizeof(variableList_at1min[0]);
// Create the 1-minute VariableArray object
VariableArray array1min;

// The variables to record at 5 minute intervals
Variable *variableList_at5min[] = {
    new MaximDS3231_Temp(&ds3231),
    new ProcessorStats_Battery(&mcuBoard),
    new ProcessorStats_FreeRam(&mcuBoard)
};
// Count up the number of pointers in the 5-minute array
int variableCount5min = sizeof(variableList_at5min) / sizeof(variableList_at5min[0]);
// Create the 5-minute VariableArray object
VariableArray array5min;


// ==========================================================================
//     The Logger Object[s]
// ==========================================================================
#include <LoggerBase.h>

// Create the 1-minute  logger instance
Logger logger1min;

// Create the 5-minute  logger instance
Logger logger5min;


// ==========================================================================
//    Working Functions
// ==========================================================================

// Flashes the LED's on the primary board
void greenredflash(uint8_t numFlash = 4, uint8_t rate = 75)
{
    for (uint8_t i = 0; i < numFlash; i++) {
        digitalWrite(greenLED, HIGH);
        digitalWrite(redLED, LOW);
        delay(rate);
        digitalWrite(greenLED, LOW);
        digitalWrite(redLED, HIGH);
        delay(rate);
    }
    digitalWrite(redLED, LOW);
}


// ==========================================================================
// Main setup function
// ==========================================================================
void setup()
{
    // Start the primary serial connection
    Serial.begin(serialBaud);

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(sketchName);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);
    Serial.println();

    Serial.print(F("Using ModularSensors Library version "));
    Serial.println(MODULAR_SENSORS_VERSION);

    if (String(MODULAR_SENSORS_VERSION) !=  String(libraryVersion))
        Serial.println(F(
            "WARNING: THIS EXAMPLE WAS WRITTEN FOR A DIFFERENT VERSION OF MODULAR SENSORS!!"));

    // Start the serial connection with the modem
    modemSerial.begin(modemBaud);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Set the timezones for the logger/data and the RTC
    // Logging in the given time zone
    Logger::setLoggerTimeZone(timeZone);
    // It is STRONGLY RECOMMENDED that you set the RTC to be in UTC (UTC+0)
    Logger::setRTCTimeZone(0);

    // Begin the variable array[s], logger[s], and publisher[s]
    array1min.begin(variableCount1min, variableList_at1min);
    array5min.begin(variableCount5min, variableList_at5min);
    logger1min.begin(LoggerID, 1, &array1min);
    logger5min.begin(LoggerID, 5, &array5min);
    logger1min.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin, greenLED);
    logger5min.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin, greenLED);

    // Turn on the modem
    modem.setModemLED(modemLEDPin);
    modem.modemPowerUp();
    modem.wake();

    // Set up the sensors (do this directly on the VariableArray)
    array1min.setupSensors();
    array5min.setupSensors();

    // Print out the current time
    Serial.print(F("Current RTC time is: "));
    Serial.println(Logger::formatDateTime_ISO8601(Logger::getNowEpoch()));
    // Connect to the network
    if (modem.connectInternet())
    {
        // Synchronize the RTC
        logger1min.setRTClock(modem.getNISTTime());
        // Disconnect from the network
        modem.disconnectInternet();
    }
    // Turn off the modem
    modem.modemSleepPowerDown();

    // Give the loggers different file names
    // If we wanted to auto-generate the file name, that could also be done by
    // not calling this function, but in that case if both "loggers" have the
    // same logger id, they will end up with the same filename
    logger1min.setFileName(FileName1min);
    logger5min.setFileName(FileName5min);

    // Setup the logger files.  Specifying true will put a default header at
    // on to the file when it's created.
    // Because we've already called setFileName, we do not need to specify the
    // file name for this function.
    logger1min.turnOnSDcard(true);  // true = wait for card to settle after power up
    logger1min.createLogFile(true);  // true = write a new header
    logger5min.createLogFile(true);  // true = write a new header
    logger1min.turnOffSDcard(true);  // true = wait for internal housekeeping after write

    Serial.println(F("Logger setup finished!\n"));
    Serial.println(F("------------------------------------------"));
    Serial.println();

    // Call the processor sleep
    // Only need to do this for one of the loggers
    logger1min.systemSleep();
}


// ==========================================================================
// Main loop function
// ==========================================================================

// Because of the way alarms work on the RTC, it will wake the processor and
// start the loop every minute exactly on the minute.
// The processor may also be woken up by another interrupt or level change on a
// pin - from a button or some other input.
// The "if" statements in the loop determine what will happen - whether the
// sensors update, testing mode starts, or it goes back to sleep.
void loop()
{
    // Check if the current time is an even interval of the logging interval
    // For whichever logger we call first, use the checkInterval() function.
    if (logger1min.checkInterval())
    {
        // Print a line to show new reading
        Serial.println(F("--------------------->111<---------------------"));
        // Turn on the LED to show we're taking a reading
        digitalWrite(greenLED, HIGH);

        // Send power to all of the sensors (do this directly on the VariableArray)
        Serial.print(F("Powering sensors...\n"));
        array1min.sensorsPowerUp();
        // Wake up all of the sensors (do this directly on the VariableArray)
        Serial.print(F("Waking sensors...\n"));
        array1min.sensorsWake();
        // Update the values from all attached sensors (do this directly on the VariableArray)
        Serial.print(F("Updating sensor values...\n"));
        array1min.updateAllSensors();
        // Put sensors to sleep (do this directly on the VariableArray)
        Serial.print(F("Putting sensors back to sleep...\n"));
        array1min.sensorsSleep();
        // Cut sensor power (do this directly on the VariableArray)
        Serial.print(F("Cutting sensor power...\n"));
        array1min.sensorsPowerDown();

        // Stream the csv data to the SD card
        logger1min.turnOnSDcard(true);
        logger1min.logToSD();
        logger1min.turnOffSDcard(true);

        // Turn off the LED
        digitalWrite(greenLED, LOW);
        // Print a line to show reading ended
        Serial.println(F("---------------------<111>---------------------\n"));
    }
    // Check if the already marked time is an even interval of the logging interval
    // For logger[s] other than the first one, use the checkMarkedInterval() function.
    if (logger5min.checkMarkedInterval())
    {
        // Print a line to show new reading
        Serial.println(F("--------------------->555<---------------------"));
        // Turn on the LED to show we're taking a reading
        digitalWrite(redLED, HIGH);

        // Send power to all of the sensors (do this directly on the VariableArray)
        Serial.print(F("Powering sensors...\n"));
        array1min.sensorsPowerUp();
        // Wake up all of the sensors (do this directly on the VariableArray)
        Serial.print(F("Waking sensors...\n"));
        array1min.sensorsWake();
        // Update the values from all attached sensors (do this directly on the VariableArray)
        Serial.print(F("Updating sensor values...\n"));
        array1min.updateAllSensors();
        // Put sensors to sleep (do this directly on the VariableArray)
        Serial.print(F("Putting sensors back to sleep...\n"));
        array1min.sensorsSleep();
        // Cut sensor power (do this directly on the VariableArray)
        Serial.print(F("Cutting sensor power...\n"));
        array1min.sensorsPowerDown();

        // Stream the csv data to the SD card
        logger5min.turnOnSDcard(true);
        logger5min.logToSD();
        logger5min.turnOffSDcard(true);

        // Turn off the LED
        digitalWrite(redLED, LOW);
        // Print a line to show reading ended
        Serial.println(F("--------------------<555>---------------------\n"));
    }
    // Once a day, at midnight, sync the clock
    if (Logger::markedEpochTime % 86400 == 0)
    {
        // Turn on the modem
        modem.modemPowerUp();
        // Connect to the network
        if (modem.connectInternet())
        {
            // Synchronize the RTC
            logger1min.setRTClock(modem.getNISTTime());
            // Disconnect from the network
            modem.disconnectInternet();
        }
        // Turn off the modem
        modem.modemSleepPowerDown();
    }

    // Call the processor sleep
    // Only need to do this for one of the loggers
    logger1min.systemSleep();
}
