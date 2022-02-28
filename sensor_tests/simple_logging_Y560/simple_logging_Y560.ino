/** =========================================================================
 * @file simple_logging_Y560.ino
 * @brief A data logging example for the Learn EnviroDIY tutorial.
 *
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * @copyright (c) 2017-2021 Stroud Water Research Center (SWRC)
 *                          and the EnviroDIY Development Team
 *            This example is published under the BSD-3 license.
 *
 * Build Environment: Visual Studios Code with PlatformIO
 * Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
 * Created with ModularSensors v0.32.2
 *
 * DISCLAIMER:
 * THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
 * ======================================================================= */

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
//  Creating Additional Serial Ports
// ==========================================================================
// The modem and a number of sensors communicate over UART/TTL - often called
// "serial". "Hardware" serial ports (automatically controlled by the MCU) are
// generally the most accurate and should be configured and used for as many
// peripherals as possible.  In some cases (ie, modbus communication) many
// sensors can share the same serial port.

// Unfortunately, most AVR boards have only one or two hardware serial ports,
// so we'll set up three types of extra software serial ports to use

// AltSoftSerial by Paul Stoffregen
// (https://github.com/PaulStoffregen/AltSoftSerial) is the most accurate
// software serial port for AVR boards. AltSoftSerial can only be used on one
// set of pins on each board so only one AltSoftSerial port can be used. Not all
// AVR boards are supported by AltSoftSerial.
/** Start [altsoftserial] */
#include <AltSoftSerial.h>
AltSoftSerial altSoftSerial;
/** End [altsoftserial] */


// ==========================================================================
//  Assigning Serial Port Functionality
// ==========================================================================

/** Start [assign_ports_sw] */

// Define the serial port for modbus
// Modbus (at 9600 8N1) is used by the Keller level loggers and Yosemitech
// sensors
// Since AltSoftSerial is the best software option, we use it for modbus
// If AltSoftSerial (or its pins) aren't avaiable, use NeoSWSerial
// SoftwareSerial **WILL NOT** work for modbus!
#define modbusSerial altSoftSerial  // For AltSoftSerial
// #define modbusSerial neoSSerial1  // For Neo software serial
// #define modbusSerial softSerial1  // For software serial

/** End [assign_ports_sw] */


// ==========================================================================
//  Data Logging Options
// ==========================================================================
/** Start [logging_options] */
// The name of this program file
const char* sketchName = "simple_logging_Y560.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char* LoggerID = "simple_logging_Y560";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 1;
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
#include <sensors/MaximDS3231.h>  // Includes wrapper functions for Maxim DS3231 RTC

// Create a DS3231 sensor object, using this constructor function:
MaximDS3231 ds3231(1);
/** End [ds3231] */


// ==========================================================================
//    Settings for Additional Sensors
// ==========================================================================
// Additional sensors can setup here, similar to the RTC, but only if
//   they have been supported with ModularSensors wrapper functions. See:
//   https://github.com/EnviroDIY/ModularSensors/wiki#just-getting-started
// Syntax for the include statement and constructor function for each sensor is
// at
//   https://github.com/EnviroDIY/ModularSensors/wiki#these-sensors-are-currently-supported
//   or can be copied from the `menu_a_la_carte.ino` example

// ==========================================================================
//  Yosemitech Y560 Ammonium Probe with Wiper
// ==========================================================================
/** Start [y560] */
#include <sensors/YosemitechY560.h>

// NOTE: Extra hardware and software serial ports are created in the "Settings
// for Additional Serial Ports" section

byte         y560ModbusAddress = 0x60;  // The modbus address of the Y560
            // NOTE: Hexidecimal 0x60 = 96 decimal used by Yosemitech SmartPC
const int8_t y560AdapterPower  = sensorPowerPin;  // RS485 adapter power pin
                                                  // (-1 if unconnected)
const int8_t  y560SensorPower = sensorPowerPin;   // Sensor power pin
const int8_t  y560EnablePin   = -1;  // Adapter RE/DE pin (-1 if not applicable)
const uint8_t y560NumberReadings = 3;
// The manufacturer recommends averaging 10 readings, but we take 5 to minimize
// power consumption

// Create a Y560 Ammonium Probe object
YosemitechY560 y560(y560ModbusAddress, modbusSerial, y560AdapterPower,
                    y560SensorPower, y560EnablePin, y560NumberReadings);

// Create COD, turbidity, and temperature variable pointers for the Y560
Variable* y560NH4_N =
    new YosemitechY560_NH4_N(&y560, "12345678-abcd-1234-ef00-1234567890ab");
Variable* y560pH =
    new YosemitechY560_pH(&y560, "12345678-abcd-1234-ef00-1234567890ab");
Variable* y560Temp =
    new YosemitechY560_Temp(&y560, "12345678-abcd-1234-ef00-1234567890ab");
/** End [y560] */


// ==========================================================================
//  Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
/** Start [variable_arrays] */
Variable* variableList[] = {
    new ProcessorStats_SampleNumber(&mcuBoard),
    new ProcessorStats_FreeRam(&mcuBoard),
    new ProcessorStats_Battery(&mcuBoard),
    new MaximDS3231_Temp(&ds3231),
    y560NH4_N,
    y560pH,
    y560Temp,
    // Additional sensor variables can be added here, by copying the syntax
    //   for creating the variable pointer (FORM1) from the
    //   `menu_a_la_carte.ino` example
    // The example code snippets in the wiki are primarily FORM2.
};
// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray;
/** End [variable_arrays] */


// ==========================================================================
//  The Logger Object[s]
// ==========================================================================
/** Start [loggers] */
// Create a logger instance
Logger dataLogger;
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

// Uses the processor sensor object to read the battery voltage
// NOTE: This will actually return the battery level from the previous update!
float getBatteryVoltage() {
    if (mcuBoard.sensorValues[0] == -9999) mcuBoard.update();
    return mcuBoard.sensorValues[0];
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

    // Set information pins
    dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin,
                             greenLED);

    // Begin the variable array[s], logger[s], and publisher[s]
    varArray.begin(variableCount, variableList);
    dataLogger.begin(LoggerID, loggingInterval, &varArray);

    // Set up the sensors
    Serial.println(F("Setting up sensors..."));
    varArray.setupSensors();

    // Create the log file, adding the default header to it
    // Do this last so we have the best chance of getting the time correct and
    // all sensor names correct
    dataLogger.createLogFile(true);  // true = write a new header

    // Call the processor sleep
    dataLogger.systemSleep();
}
/** End [setup] */


// ==========================================================================
//  Arduino Loop Function
// ==========================================================================
/** Start [complex_loop] */
// Use this long loop when you want to do something special
// Because of the way alarms work on the RTC, it will wake the processor and
// start the loop every minute exactly on the minute.
// The processor may also be woken up by another interrupt or level change on a
// pin - from a button or some other input.
// The "if" statements in the loop determine what will happen - whether the
// sensors update, testing mode starts, or it goes back to sleep.
void loop() {
    // Reset the watchdog
    dataLogger.watchDogTimer.resetWatchDog();

    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    // We're only doing anything at all if the battery is above 3.4V
    if (dataLogger.checkInterval() && getBatteryVoltage() > 3.4) {
        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;
        dataLogger.watchDogTimer.resetWatchDog();

        // Print a line to show new reading
        Serial.println(F("------------------------------------------"));
        // Turn on the LED to show we're taking a reading
        dataLogger.alertOn();
        // Power up the SD Card, but skip any waits after power up
        dataLogger.turnOnSDcard(false);
        dataLogger.watchDogTimer.resetWatchDog();

        // // Turn on the modem to let it start searching for the network
        // // Only turn the modem on if the battery at the last interval was high
        // // enough
        // // NOTE:  if the modemPowerUp function is not run before the
        // // completeUpdate
        // // function is run, the modem will not be powered and will not
        // // return a signal strength reading.
        // if (getBatteryVoltage() > 3.6) modem.modemPowerUp();

        // Start the stream for the modbus sensors, if your RS485 adapter bleeds
        // current from data pins when powered off & you stop modbus serial
        // connection with digitalWrite(5, LOW), below.
        // https://github.com/EnviroDIY/ModularSensors/issues/140#issuecomment-389380833
        altSoftSerial.begin(9600);

        // Do a complete update on the variable array.
        // This this includes powering all of the sensors, getting updated
        // values, and turing them back off.
        // NOTE:  The wake function for each sensor should force sensor setup
        // to run if the sensor was not previously set up.
        varArray.completeUpdate();

        dataLogger.watchDogTimer.resetWatchDog();

        // Reset modbus serial pins to LOW, if your RS485 adapter bleeds power
        // on sleep, because Modbus Stop bit leaves these pins HIGH.
        // https://github.com/EnviroDIY/ModularSensors/issues/140#issuecomment-389380833
        digitalWrite(5, LOW);  // Reset AltSoftSerial Tx pin to LOW
        digitalWrite(6, LOW);  // Reset AltSoftSerial Rx pin to LOW

        // Create a csv data record and save it to the log file
        dataLogger.logToSD();
        dataLogger.watchDogTimer.resetWatchDog();

        // // Connect to the network
        // // Again, we're only doing this if the battery is doing well
        // if (getBatteryVoltage() > 3.55) {
        //     dataLogger.watchDogTimer.resetWatchDog();
        //     if (modem.connectInternet()) {
        //         dataLogger.watchDogTimer.resetWatchDog();
        //         // Publish data to remotes
        //         Serial.println(F("Modem connected to internet."));
        //         dataLogger.publishDataToRemotes();
        //
        //         // Sync the clock at midnight
        //         dataLogger.watchDogTimer.resetWatchDog();
        //         if (Logger::markedEpochTime != 0 &&
        //             Logger::markedEpochTime % 86400 == 0) {
        //             Serial.println(F("Running a daily clock sync..."));
        //             dataLogger.setRTClock(modem.getNISTTime());
        //             dataLogger.watchDogTimer.resetWatchDog();
        //             modem.updateModemMetadata();
        //             dataLogger.watchDogTimer.resetWatchDog();
        //         }
        //
        //         // Disconnect from the network
        //         modem.disconnectInternet();
        //         dataLogger.watchDogTimer.resetWatchDog();
        //     }
        //     // Turn the modem off
        //     modem.modemSleepPowerDown();
        //     dataLogger.watchDogTimer.resetWatchDog();
        // }

        // Cut power from the SD card - without additional housekeeping wait
        dataLogger.turnOffSDcard(false);
        dataLogger.watchDogTimer.resetWatchDog();
        // Turn off the LED
        dataLogger.alertOff();
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));

        // Unset flag
        Logger::isLoggingNow = false;
    }

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::startTesting) {
        // Start the stream for the modbus sensors, if your RS485 adapter bleeds
        // current from data pins when powered off & you stop modbus serial
        // connection with digitalWrite(5, LOW), below.
        // https://github.com/EnviroDIY/ModularSensors/issues/140#issuecomment-389380833
        altSoftSerial.begin(9600);

        dataLogger.testingMode();
    }

    // Reset modbus serial pins to LOW, if your RS485 adapter bleeds power
    // on sleep, because Modbus Stop bit leaves these pins HIGH.
    // https://github.com/EnviroDIY/ModularSensors/issues/140#issuecomment-389380833
    digitalWrite(5, LOW);  // Reset AltSoftSerial Tx pin to LOW
    digitalWrite(6, LOW);  // Reset AltSoftSerial Rx pin to LOW

    // Call the processor sleep
    dataLogger.systemSleep();
}
/** End [complex_loop] */
