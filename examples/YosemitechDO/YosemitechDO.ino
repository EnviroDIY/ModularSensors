/*****************************************************************************
simple_logging.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of logging data to an SD card

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ---------------------------------------------------------------------------
// Include the base required libraries
// ---------------------------------------------------------------------------
#define MODULAR_SENSORS_OUTPUT Serial  // Without this there will be no output
#define VAR_ARRAY_DBG Serial  // For in-depth debugging of variable array and logger fxns

#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <LoggerBase.h>

// ---------------------------------------------------------------------------
// Set up the sensor specific information
//   ie, pin locations, addresses, calibrations and related settings
// ---------------------------------------------------------------------------
// The name of this file
const char *SKETCH_NAME = "simple_logging.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "Mayfly_Yosemitech";
// How frequently (in minutes) to log data
int LOGGING_INTERVAL = 15;
// Your logger's timezone.
const int TIME_ZONE = -5;
// Create a new logger instance
Logger logger;

// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <MaximDS3231.h>
MaximDS3231 ds3231(1);


// ==========================================================================
//    EnviroDIY Mayfly Arduino-Based Board and Processor
// ==========================================================================
#include <ProcessorMetadata.h>
const char *MFVersion = "v0.5";
ProcessorMetadata mayfly(MFVersion) ;


// ==========================================================================
//    Yosemitech Y504 Dissolved Oxygen Sensor
// ==========================================================================
#include <YosemitechY504.h>
byte y504modbusAddress = 0x04;  // The modbus address of the Y504
const int modbusPower = 22;  // switched sensor power is pin 22 on Mayfly
const int max485EnablePin = 7;  // the pin connected to the RE/DE on the 485 chip
const int y504NumberReadings = 10;  // The manufacturer strongly recommends taking and averaging 10 readings

#if defined __AVR__
// #include <SoftwareSerial_ExtInts.h>  // for the stream communication
// const int modbusRx = 10;
// const int modbusTx = 11;
// SoftwareSerial_ExtInts modbusSerial(modbusRx, modbusTx);
#include <AltSoftSerial.h>
AltSoftSerial modbusSerial;
YosemitechY504 y504(y504modbusAddress, modbusPower, modbusSerial, max485EnablePin, y504NumberReadings);
#else
HardwareSerial &modbusSerial = Serial1;
YosemitechY504 y504(y504modbusAddress, modbusPower, modbusSerial, max485EnablePin, y504NumberReadings);
#endif


// // ==========================================================================
// //    Yosemitech Y510 or Y511 Turbidity Sensor
// // ==========================================================================
// #include <YosemitechY510.h>
// byte y510modbusAddress = 0x0B;  // The modbus address of the Y510 or Y511
// // const int modbusPower = 22;  // switched sensor power is pin 22 on Mayfly
// // const int max485EnablePin = -1;  // the pin connected to the RE/DE on the 485 chip (-1 if N/A)
// const int y510NumberReadings = 10;  // The manufacturer strongly recommends taking and averaging 10 readings
//
// #if defined __AVR__
// // #include <SoftwareSerial_ExtInts.h>  // for the stream communication
// // const int modbusRx = 10;
// // const int modbusTx = 11;
// // SoftwareSerial_ExtInts modbusSerial(modbusRx, modbusTx);
// // #include <AltSoftSerial.h>
// // AltSoftSerial modbusSerial;
// YosemitechY510 y510(y510modbusAddress, modbusPower, modbusSerial, max485EnablePin, y510NumberReadings);
// #else
// // HardwareSerial &modbusSerial = Serial1;
// YosemitechY510 y510(y510modbusAddress, modbusPower, modbusSerial, max485EnablePin, y510NumberReadings);
// #endif
//
//
// // ==========================================================================
// //    Yosemitech Y514 Chlorophyll Sensor
// // ==========================================================================
// #include <YosemitechY514.h>
// byte y514modbusAddress = 0x14;  // The modbus address of the Y514
// // const int modbusPower = 22;  // switched sensor power is pin 22 on Mayfly
// // const int max485EnablePin = -1;  // the pin connected to the RE/DE on the 485 chip (-1 if N/A)
// const int y514NumberReadings = 10;  // The manufacturer strongly recommends taking and averaging 10 readings
//
// #if defined __AVR__
// // #include <SoftwareSerial_ExtInts.h>  // for the stream communication
// // const int modbusRx = 10;
// // const int modbusTx = 11;
// // SoftwareSerial_ExtInts modbusSerial(modbusRx, modbusTx);
// // #include <AltSoftSerial.h>
// // AltSoftSerial modbusSerial;
// YosemitechY514 y514(y514modbusAddress, modbusPower, modbusSerial, max485EnablePin, y514NumberReadings);
// #else
// // HardwareSerial &modbusSerial = Serial1;
// YosemitechY514 y514(y514modbusAddress, modbusPower, modbusSerial, max485EnablePin, y514NumberReadings);
// #endif
//
//
// // ==========================================================================
// //    Yosemitech Y520 Conductivity Sensor
// // ==========================================================================
// #include <YosemitechY520.h>
// byte y520modbusAddress = 0x20;  // The modbus address of the Y520
// // const int modbusPower = 22;  // switched sensor power is pin 22 on Mayfly
// // const int max485EnablePin = -1;  // the pin connected to the RE/DE on the 485 chip (-1 if N/A)
// const int y520NumberReadings = 10;  // The manufacturer strongly recommends taking and averaging 10 readings
//
// #if defined __AVR__
// // #include <SoftwareSerial_ExtInts.h>  // for the stream communication
// // const int modbusRx = 10;
// // const int modbusTx = 11;
// // SoftwareSerial_ExtInts modbusSerial(modbusRx, modbusTx);
// // #include <AltSoftSerial.h>
// // AltSoftSerial modbusSerial;
// YosemitechY520 y520(y520modbusAddress, modbusPower, modbusSerial, max485EnablePin, y520NumberReadings);
// #else
// // HardwareSerial &modbusSerial = Serial1;
// YosemitechY520 y520(y520modbusAddress, modbusPower, modbusSerial, max485EnablePin, y520NumberReadings);
// #endif
//
//
// // ==========================================================================
// //    Yosemitech Y532 pH
// // ==========================================================================
// #include <YosemitechY532.h>
// byte y532modbusAddress = 0x32;  // The modbus address of the Y532
// // const int modbusPower = 22;  // switched sensor power is pin 22 on Mayfly
// // const int max485EnablePin = -1;  // the pin connected to the RE/DE on the 485 chip (-1 if N/A)
// const int y532NumberReadings = 1;  // The manufacturer actually doesn't mention averaging for this one
//
// #if defined __AVR__
// // #include <SoftwareSerial_ExtInts.h>  // for the stream communication
// // const int modbusRx = 10;
// // const int modbusTx = 11;
// // SoftwareSerial_ExtInts modbusSerial(modbusRx, modbusTx);
// // #include <AltSoftSerial.h>
// // AltSoftSerial modbusSerial;
// YosemitechY532 y532(y532modbusAddress, modbusPower, modbusSerial, max485EnablePin, y532NumberReadings);
// #else
// // HardwareSerial &modbusSerial = Serial1;
// YosemitechY532 y532(y532modbusAddress, modbusPower, modbusSerial, max485EnablePin, y532NumberReadings);
// #endif

// ---------------------------------------------------------------------------
// The array that contains all valid variables
// ---------------------------------------------------------------------------
Variable *variableList[] = {
    new ProcessorMetadata_Batt(&mayfly),
    new ProcessorMetadata_FreeRam(&mayfly),
    new MaximDS3231_Temp(&ds3231),
    new YosemitechY504_DOpct(&y504),
    new YosemitechY504_Temp(&y504),
    new YosemitechY504_DOmgL(&y504),
    // new YosemitechY510_Turbidity(&y510),
    // new YosemitechY510_Temp(&y510),
    // new YosemitechY514_Chlorophyll(&y514),
    // new YosemitechY514_Temp(&y514),
    // new YosemitechY520_Cond(&y520),
    // new YosemitechY520_Temp(&y520),
    // new YosemitechY532_pH(&y532),
    // new YosemitechY532_Temp(&y532),
    // new YosemitechY532_Voltage(&y532),
    // new YOUR_variableName_HERE(&)
};
int variableCount = sizeof(variableList) / sizeof(variableList[0]);


// ---------------------------------------------------------------------------
// Board setup info
// ---------------------------------------------------------------------------
const long SERIAL_BAUD = 57600;  // Serial port baud rate
const int GREEN_LED = 8;  // Pin for the green LED
const int RED_LED = 9;  // Pin for the red LED
const int BUTTON_PIN = 21;  // Pin for the button
const int RTC_PIN = A7;  // RTC Interrupt/Alarm pin
const int SD_SS_PIN = 12;  // SD Card Chip Select/Slave Select Pin


// ---------------------------------------------------------------------------
// Working Functions
// ---------------------------------------------------------------------------

// Flashes to Mayfly's LED's
void greenredflash(int numFlash = 4, int rate = 75)
{
  for (int i = 0; i < numFlash; i++) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    delay(rate);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(rate);
  }
  digitalWrite(RED_LED, LOW);
}


// ---------------------------------------------------------------------------
// Main setup function
// ---------------------------------------------------------------------------
void setup()
{
    // Start the primary serial connection
    Serial.begin(SERIAL_BAUD);

    // Start the stream for the modbus sensors
    modbusSerial.begin(9600);

    // Set up pins for the LED's
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(SKETCH_NAME);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);

    // Set the timezone and offsets
    Logger::setTimeZone(TIME_ZONE);
    Logger::setTZOffset(TIME_ZONE);  // Because RTC is in UTC

    // Initialize the logger;
    logger.init(SD_SS_PIN, RTC_PIN, variableCount, variableList,
                LOGGING_INTERVAL, LoggerID);
    logger.setAlertPin(GREEN_LED);

    // Begin the logger;
    logger.begin();

    // Check for debugging mode
    logger.checkForDebugMode(BUTTON_PIN, &Serial);
}


// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    // Log the data
    logger.log();
}
