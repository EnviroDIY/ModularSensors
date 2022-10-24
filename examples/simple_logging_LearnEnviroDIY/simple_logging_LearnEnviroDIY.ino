/** =========================================================================
 * @file simple_logging_LearnEnviroDIY.ino
 * @brief A data logging example for the Learn EnviroDIY tutorial.
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
const char* sketchName = "simple_logging.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char* LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 15;
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
//  Bosch BME280 Environmental Sensor
// ==========================================================================
/** Start [bme280] */
#include <sensors/BoschBME280.h>

const int8_t I2CPower    = sensorPowerPin;  // Power pin (-1 if unconnected)
uint8_t      BMEi2c_addr = 0x76;
// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove
// default) Either can be physically mofidied for the other address

// Create a Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, BMEi2c_addr);
/** End [bme280] */


// ==========================================================================
//  Maxim DS18 One Wire Temperature Sensor
// ==========================================================================
/** Start [ds18] */
#include <sensors/MaximDS18.h>

// OneWire Address [array of 8 hex characters]
// If only using a single sensor on the OneWire bus, you may omit the address
// DeviceAddress OneWireAddress1 = {0x28, 0xFF, 0xBD, 0xBA, 0x81, 0x16, 0x03,
// 0x0C};
const int8_t OneWirePower = sensorPowerPin;  // Power pin (-1 if unconnected)
const int8_t OneWireBus   = 6;  // OneWire Bus Pin (-1 if unconnected)

// Create a Maxim DS18 sensor objects (use this form for a known address)
// MaximDS18 ds18(OneWireAddress1, OneWirePower, OneWireBus);

// Create a Maxim DS18 sensor object (use this form for a single sensor on bus
// with an unknown address)
MaximDS18 ds18(OneWirePower, OneWireBus);
/** End [ds18] */


// ==========================================================================
//  Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
/** Start [variable_arrays] */
Variable* variableList[] = {
    new ProcessorStats_SampleNumber(&mcuBoard),
    new ProcessorStats_FreeRam(&mcuBoard),
    new ProcessorStats_Battery(&mcuBoard),
    new MaximDS3231_Temp(&ds3231),
    new BoschBME280_Temp(&bme280),
    new BoschBME280_Humidity(&bme280),
    new BoschBME280_Pressure(&bme280),
    new BoschBME280_Altitude(&bme280),
    new MaximDS18_Temp(&ds18)
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
/** Start [loop] */
void loop() {
    dataLogger.logData();
}
/** End [loop] */
