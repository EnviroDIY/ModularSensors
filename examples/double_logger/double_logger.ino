/** =========================================================================
 * @file double_logger.ino
 * @brief Example logging at two different timing intervals
 *
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @copyright (c) 2017-2022 Stroud Water Research Center (SWRC)
 *                          and the EnviroDIY Development Team
 *            This example is published under the BSD-3 license.
 *
 * Build Environment: Visual Studios Code with PlatformIO
 * Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
 *
 * DISCLAIMER:
 * THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
 * ======================================================================= */

// ==========================================================================
//  Defines for TinyGSM
// ==========================================================================
/** Start [defines] */
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif
#ifndef TINY_GSM_YIELD_MS
#define TINY_GSM_YIELD_MS 2
#endif
/** End [defines] */

// ==========================================================================
//  Include the libraries required for any data logger
// ==========================================================================
/** Start [includes] */
// The Arduino library is needed for every Arduino program.
#include <Arduino.h>

// EnableInterrupt is used by ModularSensors for external and pin change
// interrupts and must be explicitly included in the main program.
#include <EnableInterrupt.h>

// Include the main header for ModularSensors
#include <ModularSensors.h>
/** End [includes] */


// ==========================================================================
//  Data Logging Options
// ==========================================================================
/** Start [logging_options] */
// The name of this program file
const char* sketchName = "double_logger.ino";
// Logger ID - we're only using one logger ID for both "loggers"
const char* LoggerID = "XXXXX";
// The TWO filenames for the different logging intervals
const char* FileName5min = "Logger_5MinuteInterval.csv";
const char* FileName1min = "Logger_1MinuteInterval.csv";
// Your logger's timezone.
const int8_t timeZone = -5;  // Eastern Standard Time
// NOTE:  Daylight savings time will not be applied!  Please use standard time!

// Set the input and output pins for the logger
// NOTE:  Use -1 for pins that do not apply
const int32_t serialBaud = 115200;  // Baud rate for debugging
const int8_t  greenLED   = 8;       // Pin for the green LED
const int8_t  redLED     = 9;       // Pin for the red LED
const int8_t  buttonPin  = 21;      // Pin for debugging mode (ie, button pin)
const int8_t  wakePin    = 31;  // MCU interrupt/alarm pin to wake from sleep
// Mayfly 0.x D31 = A7
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPwrPin   = -1;  // MCU SD card power pin
const int8_t sdCardSSPin    = 12;  // SD card chip select/slave select pin
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power
/** End [logging_options] */


// ==========================================================================
//  Wifi/Cellular Modem Options
// ==========================================================================
/** Start [xbee_wifi] */
// For the Digi Wifi XBee (S6B)
#include <modems/DigiXBeeWifi.h>
// Create a reference to the serial port for the modem

HardwareSerial& modemSerial = Serial1;  // Use hardware serial if possible
const int32_t   modemBaud   = 9600;     // All XBee's use 9600 by default

// Modem Pins - Describe the physical pin connection of your modem to your board
// NOTE:  Use -1 for pins that do not apply
const int8_t modemVccPin    = -2;    // MCU pin controlling modem power
const int8_t modemStatusPin = 19;    // MCU pin used to read modem status
const bool useCTSforStatus  = true;  // Flag to use the modem CTS pin for status
const int8_t modemResetPin  = 20;    // MCU pin connected to modem reset pin
const int8_t modemSleepRqPin = 23;   // MCU pin for modem sleep/wake request
const int8_t modemLEDPin = redLED;   // MCU pin connected an LED to show modem
                                     // status (-1 if unconnected)

// Network connection information
const char* wifiId  = "xxxxx";  // WiFi access point, unnecessary for GPRS
const char* wifiPwd = "xxxxx";  // WiFi password, unnecessary for GPRS

DigiXBeeWifi modemXBWF(&modemSerial, modemVccPin, modemStatusPin,
                       useCTSforStatus, modemResetPin, modemSleepRqPin, wifiId,
                       wifiPwd);
// Create an extra reference to the modem by a generic name
DigiXBeeWifi modem = modemXBWF;
/** End [xbee_wifi] */


// ==========================================================================
//  Using the Processor as a Sensor
// ==========================================================================
/** Start [processor_sensor] */
#include <sensors/ProcessorStats.h>

// Create the main processor chip "sensor" - for general metadata
const char*    mcuBoardVersion = "v1.1";
ProcessorStats mcuBoard(mcuBoardVersion);
/** End [processor_sensor] */


// ==========================================================================
//  Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
/** Start [ds3231] */
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);
/** End [ds3231] */


// ==========================================================================
//  AOSong AM2315 Digital Humidity and Temperature Sensor
// ==========================================================================
/** Start [ao_song_am2315] */
#include <sensors/AOSongAM2315.h>

const int8_t I2CPower = sensorPowerPin;  // Power pin (-1 if unconnected)

// Create and return the AOSong AM2315 sensor object
AOSongAM2315 am2315(I2CPower);
/** End [ao_song_am2315] */


// ==========================================================================
//  Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
/** Start [variable_arrays] */
// The variables to record at 1 minute intervals
Variable* variableList_at1min[] = {new AOSongAM2315_Humidity(&am2315),
                                   new AOSongAM2315_Temp(&am2315)};
// Count up the number of pointers in the 1-minute array
int variableCount1min = sizeof(variableList_at1min) /
    sizeof(variableList_at1min[0]);
// Create the 1-minute VariableArray object
VariableArray array1min;

// The variables to record at 5 minute intervals
Variable* variableList_at5min[] = {new MaximDS3231_Temp(&ds3231),
                                   new ProcessorStats_Battery(&mcuBoard),
                                   new ProcessorStats_FreeRam(&mcuBoard)};
// Count up the number of pointers in the 5-minute array
int variableCount5min = sizeof(variableList_at5min) /
    sizeof(variableList_at5min[0]);
// Create the 5-minute VariableArray object
VariableArray array5min;
/** End [variable_arrays] */


// ==========================================================================
//  The Logger Object[s]
// ==========================================================================
/** Start [loggers] */
// Create the 1-minute  logger instance
Logger logger1min;

// Create the 5-minute  logger instance
Logger logger5min;
/** End [loggers] */


// ==========================================================================
//  Working Functions
// ==========================================================================
/** Start [working_functions] */
// Flashes the LED's on the primary board
void greenredflash(uint8_t numFlash = 4, uint8_t rate = 75) {
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
/** End [working_functions] */


// ==========================================================================
//  Arduino Setup Function
// ==========================================================================
/** Start [setup] */
void setup() {
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
    Serial.print(F("TinyGSM Library version "));
    Serial.println(TINYGSM_VERSION);
    Serial.println();

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
    logger1min.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin,
                             greenLED);
    logger5min.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin,
                             greenLED);

    // Turn on the modem
    modem.setModemLED(modemLEDPin);

    // Set up the sensors (do this directly on the VariableArray)
    array1min.setupSensors();
    array5min.setupSensors();

    // Print out the current time
    Serial.print(F("Current RTC time is: "));
    Serial.println(Logger::formatDateTime_ISO8601(Logger::getNowUTCEpoch()));
    Serial.print(F("Current localized logger time is: "));
    Serial.println(Logger::formatDateTime_ISO8601(Logger::getNowLocalEpoch()));
    // Connect to the network
    if (modem.connectInternet()) {
        // Synchronize the RTC
        logger1min.setRTClock(modem.getNISTTime());
        modem.updateModemMetadata();
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
    logger1min.turnOnSDcard(
        true);  // true = wait for card to settle after power up
    logger1min.createLogFile(true);  // true = write a new header
    logger5min.createLogFile(true);  // true = write a new header
    logger1min.turnOffSDcard(
        true);  // true = wait for internal housekeeping after write

    Serial.println(F("Logger setup finished!\n"));
    Serial.println(F("------------------------------------------"));
    Serial.println();

    // Call the processor sleep
    // Only need to do this for one of the loggers
    logger1min.systemSleep();
}
/** End [setup] */


// ==========================================================================
//  Arduino Loop Function
// ==========================================================================
/** Start [loop] */
// Because of the way alarms work on the RTC, it will wake the processor and
// start the loop every minute exactly on the minute.
// The processor may also be woken up by another interrupt or level change on a
// pin - from a button or some other input.
// The "if" statements in the loop determine what will happen - whether the
// sensors update, testing mode starts, or it goes back to sleep.
void loop() {
    // Check if the current time is an even interval of the logging interval
    // For whichever logger we call first, use the checkInterval() function.
    if (logger1min.checkInterval()) {
        // Print a line to show new reading
        Serial.println(F("--------------------->111<---------------------"));
        // Turn on the LED to show we're taking a reading
        digitalWrite(greenLED, HIGH);

        // Send power to all of the sensors (do this directly on the
        // VariableArray)
        Serial.print(F("Powering sensors...\n"));
        array1min.sensorsPowerUp();
        logger1min.watchDogTimer.resetWatchDog();
        // Wake up all of the sensors (do this directly on the VariableArray)
        Serial.print(F("Waking sensors...\n"));
        array1min.sensorsWake();
        logger1min.watchDogTimer.resetWatchDog();
        // Update the values from all attached sensors (do this directly on the
        // VariableArray)
        Serial.print(F("Updating sensor values...\n"));
        array1min.updateAllSensors();
        logger1min.watchDogTimer.resetWatchDog();
        // Put sensors to sleep (do this directly on the VariableArray)
        Serial.print(F("Putting sensors back to sleep...\n"));
        array1min.sensorsSleep();
        logger1min.watchDogTimer.resetWatchDog();
        // Cut sensor power (do this directly on the VariableArray)
        Serial.print(F("Cutting sensor power...\n"));
        array1min.sensorsPowerDown();
        logger1min.watchDogTimer.resetWatchDog();

        // Stream the csv data to the SD card
        logger1min.turnOnSDcard(true);
        logger1min.logToSD();
        logger1min.turnOffSDcard(true);
        logger1min.watchDogTimer.resetWatchDog();

        // Turn off the LED
        digitalWrite(greenLED, LOW);
        // Print a line to show reading ended
        Serial.println(F("---------------------<111>---------------------\n"));
    }
    // Check if the already marked time is an even interval of the logging
    // interval For logger[s] other than the first one, use the
    // checkMarkedInterval() function.
    if (logger5min.checkMarkedInterval()) {
        // Print a line to show new reading
        Serial.println(F("--------------------->555<---------------------"));
        // Turn on the LED to show we're taking a reading
        digitalWrite(redLED, HIGH);

        // Send power to all of the sensors (do this directly on the
        // VariableArray)
        Serial.print(F("Powering sensors...\n"));
        array5min.sensorsPowerUp();
        logger1min.watchDogTimer.resetWatchDog();
        // Wake up all of the sensors (do this directly on the VariableArray)
        Serial.print(F("Waking sensors...\n"));
        array5min.sensorsWake();
        logger1min.watchDogTimer.resetWatchDog();
        // Update the values from all attached sensors (do this directly on the
        // VariableArray)
        Serial.print(F("Updating sensor values...\n"));
        array5min.updateAllSensors();
        logger1min.watchDogTimer.resetWatchDog();
        // Put sensors to sleep (do this directly on the VariableArray)
        Serial.print(F("Putting sensors back to sleep...\n"));
        array5min.sensorsSleep();
        logger1min.watchDogTimer.resetWatchDog();
        // Cut sensor power (do this directly on the VariableArray)
        Serial.print(F("Cutting sensor power...\n"));
        array5min.sensorsPowerDown();
        logger1min.watchDogTimer.resetWatchDog();

        // Stream the csv data to the SD card
        logger5min.turnOnSDcard(true);
        logger5min.logToSD();
        logger5min.turnOffSDcard(true);
        logger1min.watchDogTimer.resetWatchDog();

        // Turn off the LED
        digitalWrite(redLED, LOW);
        // Print a line to show reading ended
        Serial.println(F("--------------------<555>---------------------\n"));
    }
    // Once a day, at noon, sync the clock
    if (Logger::markedLocalEpochTime % 86400 == 43200) {
        // Turn on the modem
        modem.modemWake();
        // Connect to the network
        if (modem.connectInternet()) {
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
/** End [loop] */
