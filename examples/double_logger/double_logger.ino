/*****************************************************************************
double_logger.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of logging data from different variables at two
different logging intervals.  This example uses more of the manual functions
in the logging loop rather than the simple "log" function.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// Some define statements
#define MODULAR_SENSORS_OUTPUT Serial  // Without this there will be no output

// Select your modem chip, comment out all of the others
// #define TINY_GSM_MODEM_SIM800  // Select for a SIM800, SIM900, or varient thereof
// #define TINY_GSM_MODEM_A6  // Select for a AI-Thinker A6 or A7 chip
// #define TINY_GSM_MODEM_M590  // Select for a Neoway M590
// #define TINY_GSM_MODEM_U201  // Select for a U-blox U201
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <LoggerBase.h>
#include <ModemSupport.h>


// ==========================================================================
//    Basic Logger Settings
// ==========================================================================
// The name of this file
const char *sketchName = "logger_test.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "SL099";
const char *FileName = "doubleLoggerFile.csv";
// Your logger's timezone.
const int timeZone = -5;
// Create TWO new logger instances
Logger logger1min;
Logger logger5min;


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <ProcessorMetadata.h>

const long serialBaud = 57600;  // Baud rate for the primary serial port for debugging
const int greenLED = 8;  // Pin for the green LED (else -1)
const int redLED = 9;  // Pin for the red LED (else -1)
const int buttonPin = 21;  // Pin for a button to use to enter debugging mode (else -1)
const int wakePin = A7;  // Interrupt/Alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int sdCardPin = 12;  // SD Card Chip Select/Slave Select Pin (must be defined!)

const char *MFVersion = "v0.5";
ProcessorMetadata mayfly(MFVersion) ;


// ==========================================================================
//    Modem/Internet connection options
// ==========================================================================
HardwareSerial &ModemSerial = Serial1; // The serial port for the modem - software serial can also be used.
const int modemSleepRqPin = 23;  // Modem SleepRq Pin (for sleep requests) (-1 if unconnected)
const int modemStatusPin = 19;   // Modem Status Pin (indicates power status) (-1 if unconnected)
const int modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (else -1)

ModemSleepType ModemSleepMode = reverse;  // How the modem is put to sleep
// Use "held" if the DTR pin is held HIGH to keep the modem awake, as with a Sodaq GPRSBee rev6.
// Use "pulsed" if the DTR pin is pulsed high and then low to wake the modem up, as with an Adafruit Fona or Sodaq GPRSBee rev4.
// Use "reverse" if the DTR pin is held LOW to keep the modem awake, as with all XBees.
// Use "always_on" if you do not want the library to control the modem power and sleep or if none of the above apply.

const long ModemBaud = 9600;  // Modem baud rate
const char *wifiId = "XXXXXXX";  // The WiFi access point
const char *wifiPwd = "XXXXXXX";  // The password for connecting to WiFi
// Create the loggerModem instance
// A "loggerModem" is a combination of a TinyGSM Modem, a TinyGSM Client, and an on/off method
loggerModem modem;


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <MaximDS3231.h>
MaximDS3231 ds3231(1);


// ==========================================================================
//    AOSong AM2315 Digital Humidity and Temperature Sensor
// ==========================================================================
#include <AOSongAM2315.h>
const int I2CPower = 22;  // switched sensor power is pin 22 on Mayfly
AOSongAM2315 am2315(I2CPower);


// ==========================================================================
// The two arrays that contains the variables for the different intervals
// ==========================================================================
Variable *variableList_at1min[] = {
    new AOSongAM2315_Humidity(&am2315),
    new AOSongAM2315_Temp(&am2315)
    // new YOUR_variableName_HERE(&)
};
int variableCount1min = sizeof(variableList_at1min) / sizeof(variableList_at1min[0]);
Variable *variableList_at5min[] = {
    new MaximDS3231_Temp(&ds3231),
    new ProcessorMetadata_Batt(&mayfly),
    new ProcessorMetadata_FreeRam(&mayfly)
    // new YOUR_variableName_HERE(&)
};
int variableCount5min = sizeof(variableList_at5min) / sizeof(variableList_at5min[0]);



// ==========================================================================
//    Working Functions
// ==========================================================================

// Flashes the LED's on the primary board
void greenredflash(int numFlash = 4, int rate = 75)
{
  for (int i = 0; i < numFlash; i++) {
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

    // Start the serial connection with the modem
    ModemSerial.begin(ModemBaud);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Start the Real Time Clock
    rtc.begin();
    delay(100);

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(sketchName);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);

    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);

    // Initialize the two logger instances
    logger1min.init(sdCardPin, wakePin, variableCount1min, variableList_at1min,
                1, LoggerID);
    logger5min.init(sdCardPin, wakePin, variableCount5min, variableList_at5min,
                5, LoggerID);
    // There is no reason to call the setAlertPin() function, because we have to
    // write the loop on our own.

    // Initialize the logger modem
    modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, wifiId, wifiPwd);

    // Attach the modem to one of the loggers
    logger1min.attachModem(modem);

    // Set up the sensors on both loggers
    logger1min.setupSensors();
    logger5min.setupSensors();

    // Tell both loggers to save data to the same file
    // If we wanted to auto-generate the file name, that could also be done by
    // not calling this function.  If both "loggers" have the same logger id,
    // they will end up with the same filename
    logger1min.setFileName(FileName);
    logger5min.setFileName(FileName);

    // Setup the logger file
    // Because both loggers are saving to the same file, only
    // need to do this once
    logger1min.setupLogFile();
    // Create a header for the second logger and write it to the SD card
    logger5min.logToSD(logger5min.generateFileHeader());

    // Print out the current time
    Serial.print(F("Current RTC time is: "));
    Serial.println(Logger::formatDateTime_ISO8601(Logger::getNowEpoch()));

    // Turn on the modem
    modem.wake();
    // Connect to the network
    if (modem.connectNetwork())
    {
        // Synchronize the RTC
        logger1min.syncRTClock(modem.getNISTTime());
        // Disconnect from the network
        modem.disconnectNetwork();
    }
    // Turn off the modem
    modem.off();

    // Set up the processor sleep mode
    // Because there's only one processor, we only need to do this once
    logger1min.setupSleep();

    Serial.println(F("Logger setup finished!\n"));
    Serial.println(F("------------------------------------------"));
    Serial.println();
}


// ==========================================================================
// Main loop function
// ==========================================================================

// Because of the way the sleep mode is set up, the processor will wake up
// and start the loop every minute exactly on the minute.
// Log the data
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

        // Wake up all of the sensors
        logger1min.sensorsWake();
        // Update the values from all attached sensors
        logger1min.updateAllSensors();
        // Immediately put sensors to sleep to save power
        logger1min.sensorsSleep();

        // Create a csv data record and save it to the log file
        logger1min.logToSD(logger1min.generateSensorDataCSV());

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

        // Wake up all of the sensors
        logger5min.sensorsWake();
        // Update the values from all attached sensors
        logger5min.updateAllSensors();
        // Immediately put sensors to sleep to save power
        logger5min.sensorsSleep();

        // Create a csv data record and save it to the log file
        logger5min.logToSD(logger5min.generateSensorDataCSV());

        // Turn off the LED
        digitalWrite(redLED, LOW);
        // Print a line to show reading ended
        Serial.println(F("--------------------<555>---------------------\n"));
    }
    // Once a day, at midnight, sync the clock
    if (Logger::markedEpochTime % 86400 == 0)
    {
        // Turn on the modem
        modem.wake();
        // Connect to the network
        if (modem.connectNetwork())
        {
            // Synchronize the RTC
            logger1min.syncRTClock(modem.getNISTTime());
            // Disconnect from the network
            modem.disconnectNetwork();
        }
        // Turn off the modem
        modem.off();
    }

    // Call the processor sleep
    // Only need to do this for one of the loggers
    logger1min.systemSleep();
}
