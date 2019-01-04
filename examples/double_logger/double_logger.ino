/*****************************************************************************
double_logger.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.19.3

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
#include <LoggerBase.h>
#include <LoggerModem.h>


// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The library version this example was written for
const char *libraryVersion = "0.19.3";
// The name of this file
const char *sketchName = "logger_test.ino";
// Logger ID - since it's the same logger device, we only need one
const char *LoggerID = "XXXXX";
// The TWO filenames for the different logging intervals
const char *FileName5min = "Logger_5MinuteInterval.csv";
const char *FileName1min = "Logger_1MinuteInterval.csv";
// Your logger's timezone.
const int8_t timeZone = -5;


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
const int8_t sdCardPin = 12;      // MCU SD card chip select/slave select pin (must be given!)
const int8_t sensorPowerPin = 22; // MCU pin controlling main sensor power (-1 if not applicable)

// Create and return the processor "sensor"
const char *MFVersion = "v0.5b";
ProcessorStats mayfly(MFVersion);


// ==========================================================================
//    Modem MCU Type and TinyGSM Client
// ==========================================================================

// Select your modem chip - this determines the exact commands sent to it
// #define TINY_GSM_MODEM_SIM800  // Select for a SIM800, SIM900, or variant thereof
// #define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's

// Include TinyGSM for the modem
// This include must be included below the define of the modem name!
#include <TinyGsmClient.h>

// Create a reference to the serial port for the modem
HardwareSerial &modemSerial = Serial1;  // Use hardware serial if possible

// Create a variable for the modem baud rate - this will be used in the begin function for the port
const long ModemBaud = 9600;

// Create a new TinyGSM modem to run on that serial port and return a pointer to it
TinyGsm *tinyModem = new TinyGsm(modemSerial);

// Create a new TCP client on that modem and return a pointer to it
TinyGsmClient *tinyClient = new TinyGsmClient(*tinyModem);

// Describe the physical pin connection of your modem to your board
const int8_t modemVccPin = -2;  // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemSleepRqPin = 23;  // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemStatusPin = 19;   // MCU pin used to read modem status (-1 if not applicable)
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool wakeFxn(void)
{
    digitalWrite(modemSleepRqPin, LOW);
    digitalWrite(redLED, HIGH);  // Because the XBee doesn't have any lights
    return true;
}
bool sleepFxn(void)
{
    digitalWrite(modemSleepRqPin, HIGH);
    digitalWrite(redLED, LOW);
    return true;
}

// Network connection information
const char *apn = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId = "xxxxx";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = "xxxxx";  // The password for connecting to WiFi, unnecessary for gprs

// Create the loggerModem instance
// A "loggerModem" is a combination of a TinyGSM Modem, a Client, and functions for wake and sleep
// loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, wifiId, wifiPwd);
loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, apn);



// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>
// Create and return the DS3231 sensor object
MaximDS3231 ds3231(1);


// ==========================================================================
//    AOSong AM2315 Digital Humidity and Temperature Sensor
// ==========================================================================
#include <sensors/AOSongAM2315.h>
const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the AOSong AM2315 sensor object
AOSongAM2315 am2315(I2CPower);


// ==========================================================================
// The two arrays that contains the variables for the different intervals
// ==========================================================================
// Create pointers for all of the variables from the sensors recording at 1
// minute intervals and at the same time putting them into an array
Variable *variableList_at1min[] = {
    new AOSongAM2315_Humidity(&am2315),
    new AOSongAM2315_Temp(&am2315)
};
// Count up the number of pointers in the 1-minute array
int variableCount1min = sizeof(variableList_at1min) / sizeof(variableList_at1min[0]);
// Create the 1-minute VariableArray object
VariableArray array1min(variableCount1min, variableList_at1min);
// Create the 1-minute  logger instance
Logger  logger1min(LoggerID, 1, sdCardPin, wakePin, &array1min);

// Create pointers for all of the variables from the sensors recording at 5
// minute intervals and at the same time putting them into an array
Variable *variableList_at5min[] = {
    new MaximDS3231_Temp(&ds3231),
    new ProcessorStats_Batt(&mayfly),
    new ProcessorStats_FreeRam(&mayfly)
};
// Count up the number of pointers in the 5-minute array
int variableCount5min = sizeof(variableList_at5min) / sizeof(variableList_at5min[0]);
// Create the 5-minute VariableArray object
VariableArray array5min(variableCount5min, variableList_at5min);
// Create the 1-minute  logger instance
Logger  logger5min(LoggerID, 5, sdCardPin, wakePin, &array5min);


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
    modemSerial.begin(ModemBaud);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Set up pin for the modem
    pinMode(modemSleepRqPin, OUTPUT);

    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);


    // Turn on the modem
    modem.modemPowerUp();

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
    logger1min.createLogFile(true);
    logger5min.createLogFile(true);

    // Set up the processor sleep mode
    // Because there's only one processor, we only need to do this once
    logger1min.setupSleep();

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

// Because of the way the sleep mode is set up, the processor will wake up
// and start the loop every minute exactly on the minute.
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
        logger1min.logToSD();

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
        logger5min.logToSD();

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
