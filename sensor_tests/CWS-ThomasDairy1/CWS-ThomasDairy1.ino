/*****************************************************************************
Developed from ModularSensors examples DRWI_LTE.ino + menu_a_la_carte.ino
Adapted by:  Anthony Aufdenkampe <aaufdenkampe@limno.com>
based on examples written by Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.23.3

This sketch is an example of logging data to an SD card and sending the data to
both the EnviroDIY data portal as should be used by groups involved with
The William Penn Foundation's Delaware River Watershed Initiative

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
const char *libraryVersion = "0.23.5";
// The name of this file
const char *sketchName = "CWS-ThomasDairy1.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "CWS-ThomasDairy1";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 10;
// Your logger's timezone.
const int8_t timeZone = -8;  // Pacific Standard Time
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
const int8_t sdCardPwrPin = -1;    // MCU SD card power pin (-1 if not applicable)
const int8_t sdCardSSPin = 12;     // MCU SD card chip select/slave select pin (must be given!)
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power (-1 if not applicable)

// Create the main processor chip "sensor" - for general metadata
const char *mcuBoardVersion = "v0.5b";
ProcessorStats mcuBoard(mcuBoardVersion);

// ==========================================================================
//    Settings for Additional Serial Ports
// ==========================================================================

// AltSoftSerial by Paul Stoffregen (https://github.com/PaulStoffregen/AltSoftSerial)
// is the most accurate software serial port for AVR boards.
// AltSoftSerial can only be used on one set of pins on each board so only one
// AltSoftSerial port can be used.
// Not all AVR boards are supported by AltSoftSerial.
// #include <AltSoftSerial.h>
// AltSoftSerial altSoftSerial;

// NeoSWSerial (https://github.com/SRGDamia1/NeoSWSerial) is the best software
// serial that can be used on any pin supporting interrupts.
// You can use as many instances of NeoSWSerial as you want.
// Not all AVR boards are supported by NeoSWSerial.
// #include <NeoSWSerial.h>  // for the stream communication
// const int8_t neoSSerial1Rx = 11;     // data in pin
// const int8_t neoSSerial1Tx = -1;     // data out pin
// NeoSWSerial neoSSerial1(neoSSerial1Rx, neoSSerial1Tx);
// // To use NeoSWSerial in this library, we define a function to receive data
// // This is just a short-cut for later
// void neoSSerial1ISR()
// {
//     NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(neoSSerial1Rx)));
// }


// ==========================================================================
//    Wifi/Cellular Modem Settings
// ==========================================================================

// Create a reference to the serial port for the modem
HardwareSerial &modemSerial = Serial1;  // Use hardware serial if possible

// Modem Pins - Describe the physical pin connection of your modem to your board
const int8_t modemVccPin = -2;      // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemStatusPin = 19;   // MCU pin used to read modem status (-1 if not applicable)
const int8_t modemResetPin = 20;    // MCU pin connected to modem reset pin (-1 if unconnected)
const int8_t modemSleepRqPin = 23;  // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemLEDPin = redLED;  // MCU pin connected an LED to show modem status (-1 if unconnected)

// Network connection information
const char *apn = "hologram";  // The APN for the gprs connection
// ==========================================================================
//    The modem object
// ==========================================================================

// For any Digi Cellular XBee's
// NOTE:  The u-blox based Digi XBee's (3G global and LTE-M global)
// are more stable used in bypass mode (below) if not using an LTEBee Adapter board, https://github.com/EnviroDIY/LTEbee-Adapter
// The Telit based Digi XBees (LTE Cat1) can only use this mode.
#include <modems/DigiXBeeCellularTransparent.h>
const long modemBaud = 9600;  // All XBee's use 9600 by default
const bool useCTSforStatus = false;   // Flag to use the modem CTS pin for status. `false` if using a Mayfly with the LTEBee Adapter board, `true` if using a Mayfly without the LTEBee adapter board (because pin 19 of the mayfly (that you set as the status pin) is connected to Bee pin 12, which is CTS_not)
DigiXBeeCellularTransparent modemXBCT(&modemSerial,
                                      modemVccPin, modemStatusPin, useCTSforStatus,
                                      modemResetPin, modemSleepRqPin,
                                      apn);
// Create an extra reference to the modem by a generic name (not necessary)
DigiXBeeCellularTransparent modem = modemXBCT;


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);


// ==========================================================================
//    Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
// ==========================================================================
#include <sensors/BoschBME280.h>

const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
uint8_t BMEi2c_addr = 0x76;
// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove default)
// Either can be physically mofidied for the other address
const uint8_t BME280NumberReadings = 3;  // The number of readings to average


// Create a Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, BMEi2c_addr, BME280NumberReadings);


// ==========================================================================
//    METER TEROS 11 Soil Moisture Sensor #1, top / shallow
// ==========================================================================
#include <sensors/MeterTeros11.h>

const char *teros11SDI12address = "1";  // The SDI-12 Address of the Teros 11
const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t SDI12Data = 7;  // The SDI12 data pin
const uint8_t teros11NumberReadings = 3;  // The number of readings to average


// Create a METER TEROS 11 sensor object
MeterTeros11 teros11(*teros11SDI12address, SDI12Power, SDI12Data, teros11NumberReadings);


// ==========================================================================
//    METER TEROS 11 Soil Moisture Sensor #2, bottom / deep
// ==========================================================================

const char *teros11_2SDI12address = "6";  // The SDI-12 Address of the Teros 11

// Create a METER TEROS 11 sensor object, #2
MeterTeros11 teros11_2(*teros11_2SDI12address, SDI12Power, SDI12Data, teros11NumberReadings);


// ==========================================================================
//    Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
#include <VariableArray.h>

Variable *variableList[] = {
    new MeterTeros11_Ea(&teros11, "db7d4a18-5b98-4188-bf7d-8762cb1da2d3"),
    new MeterTeros11_Temp(&teros11, "9da8d39f-710d-4678-b1f3-009f667a7073"),
    new MeterTeros11_VWC(&teros11, "8c26d5d9-da81-4218-a198-a78c815cedce"),
    new BoschBME280_Temp(&bme280, "fe553ef8-75cd-4677-9f01-82d9bbce3762"),
    new BoschBME280_Humidity(&bme280, "63401eba-984b-4b34-856d-9c0cbc9d9a48"),
    new BoschBME280_Pressure(&bme280, "99b87310-03aa-40ca-beab-2696e3db22bb"),
    new MeterTeros11_Ea(&teros11_2, "858b111c-014b-40ad-bf24-4aa15c35f1ef"),
    new MeterTeros11_Temp(&teros11_2, "929ca303-01b3-49a8-a96b-17121c8cb8c7"),
    new MeterTeros11_VWC(&teros11_2, "f68e2659-f2bf-492e-a4d6-2313e825dc61"),
    new ProcessorStats_Battery(&mcuBoard, "6b3a0e43-3392-4e83-b0aa-ed236b5d9ae9"),
    // new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-ef00-1234567890ab"),
    // new Modem_RSSI(&modem, "12345678-abcd-1234-ef00-1234567890ab"),
    new Modem_SignalPercent(&modem, "12345678-abcd-1234-ef00-1234567890ab"),
};
// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList);


// ==========================================================================
//     The Logger Object[s]
// ==========================================================================
#include <LoggerBase.h>

// Create a new logger instance
Logger dataLogger(LoggerID, loggingInterval, &varArray);


// ==========================================================================
//    A Publisher to Monitor My Watershed / EnviroDIY Data Sharing Portal
// ==========================================================================
// Device registration and sampling feature information can be obtained after
// registration at https://monitormywatershed.org or https://data.envirodiy.org
const char *registrationToken = "75af6f65-b93d-4517-94e3-abdafaf9e253";   // Device registration token
const char *samplingFeature = "c24919f5-e95a-41ca-9bf8-ce241eeb0669";     // Sampling feature UUID

// Create a data publisher for the EnviroDIY/WikiWatershed POST endpoint
#include <publishers/EnviroDIYPublisher.h>
EnviroDIYPublisher EnviroDIYPOST(dataLogger, &modem.gsmClient, registrationToken, samplingFeature);


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


// Read's the battery voltage
// NOTE: This will actually return the battery level from the previous update!
float getBatteryVoltage()
{
    if (mcuBoard.sensorValues[0] == -9999) mcuBoard.update();
    return mcuBoard.sensorValues[0];
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

    // Allow interrupts for software serial
    #if defined SoftwareSerial_ExtInts_h
        enableInterrupt(softSerialRx, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
    #endif
    #if defined NeoSWSerial_h
        enableInterrupt(neoSSerial1Rx, neoSSerial1ISR, CHANGE);
    #endif

    // Start the serial connection with the modem
    modemSerial.begin(modemBaud);

    // Start the stream for the modbus sensors; all currently supported modbus sensors use 9600 baud
    // modbusSerial.begin(9600);

    // Start the SoftwareSerial stream for the sonar; it will always be at 9600 baud
    // sonarSerial.begin(9600);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Set up some of the power pins so the board boots up with them off
    // NOTE:  This isn't necessary at all.  The logger begin() function
    // should leave all power pins off when it finishes.
    if (modemVccPin >= 0)
    {
        pinMode(modemVccPin, OUTPUT);
        digitalWrite(modemVccPin, LOW);
    }
    if (sensorPowerPin >= 0)
    {
        pinMode(sensorPowerPin, OUTPUT);
        digitalWrite(sensorPowerPin, LOW);
    }
    if (modemSleepRqPin >= 0)
    {
        pinMode(modemSleepRqPin, OUTPUT);
        digitalWrite(modemSleepRqPin, HIGH);
    }

    // Set the timezones for the logger/data and the RTC
    // Logging in the given time zone
    Logger::setLoggerTimeZone(timeZone);
    // It is STRONGLY RECOMMENDED that you set the RTC to be in UTC (UTC+0)
    Logger::setRTCTimeZone(0);

    // Attach the modem and information pins to the logger
    dataLogger.attachModem(modem);
    modem.setModemLED(modemLEDPin);
    dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin, greenLED);

    // Begin the logger
    dataLogger.begin();

    // Note:  Please change these battery voltages to match your battery
    // Check that the battery is OK before powering the modem
    if (getBatteryVoltage() > 3.65)
    {
        modem.modemPowerUp();
        modem.wake();
        modem.setup();

        // At very good battery voltage, or with suspicious time stamp, sync the clock
        // Note:  Please change these battery voltages to match your battery
        if (getBatteryVoltage() > 3.8 ||
            dataLogger.getNowEpoch() < 1546300800 ||  /*Before 01/01/2019*/
            dataLogger.getNowEpoch() > 1735689600)  /*After 1/1/2025*/
        {
            // Synchronize the RTC with NIST
            Serial.println(F("Attempting to connect to the internet and synchronize RTC with NIST"));
            if (modem.connectInternet(120000L))
            {
                dataLogger.setRTClock(modem.getNISTTime());
            }
            else
            {
                Serial.println(F("Could not connect to internet for clock sync."));
            }
        }
    }

    // Set up the sensors, except at lowest battery level
    if (getBatteryVoltage() > 3.4)
    {
        Serial.println(F("Setting up sensors..."));
        varArray.setupSensors();
    }

    // Create the log file, adding the default header to it
    // Do this last so we have the best chance of getting the time correct and
    // all sensor names correct
    // Writing to the SD card can be power intensive, so if we're skipping
    // the sensor setup we'll skip this too.
    if (getBatteryVoltage() > 3.4)
    {
        Serial.println(F("Setting up file on SD card"));
        dataLogger.turnOnSDcard(true);  // true = wait for card to settle after power up
        dataLogger.createLogFile(true);  // true = write a new header
        dataLogger.turnOffSDcard(true);  // true = wait for internal housekeeping after write
    }

    // Power down the modem - but only if there will be more than 15 seconds before
    // the first logging interval - it can take the LTE modem that long to shut down
    if (Logger::getNowEpoch() % (loggingInterval*60) > 15 ||
        Logger::getNowEpoch() % (loggingInterval*60) < 6)
    {
        Serial.println(F("Putting modem to sleep"));
        modem.modemSleepPowerDown();
    }
    else
    {
        Serial.println(F("Leaving modem on until after first measurement"));
    }

    // Call the processor sleep
    Serial.println(F("Putting processor to sleep"));
    dataLogger.systemSleep();
}


// ==========================================================================
// Main loop function
// ==========================================================================

// Use this long loop when you want to do something special
// Because of the way alarms work on the RTC, it will wake the processor and
// start the loop every minute exactly on the minute.
// The processor may also be woken up by another interrupt or level change on a
// pin - from a button or some other input.
// The "if" statements in the loop determine what will happen - whether the
// sensors update, testing mode starts, or it goes back to sleep.

// We use this long to reset AltSoftSerial pins to LOW,
// to reduce RS485 adapter power bleed on sleep

void loop()
{
    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    // We're only doing anything at all if the battery is above 3.4V
    if (dataLogger.checkInterval() && getBatteryVoltage() > 3.4)
    {
        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;

        // Print a line to show new reading
        Serial.println(F("------------------------------------------"));
        // Turn on the LED to show we're taking a reading
        dataLogger.alertOn();
        // Power up the SD Card, but skip any waits after power up
        dataLogger.turnOnSDcard(false);

        // Turn on the modem to let it start searching for the network
        // Only turn the modem on if the battery at the last interval was high enough
        // NOTE:  if the modemPowerUp function is not run before the completeUpdate
        // function is run, the modem will not be powered and will not return
        // a signal strength reading.
        if (getBatteryVoltage() > 3.6)
            modem.modemPowerUp();

        // Start the stream for the modbus sensors
        // Because RS485 adapters tend to "steal" current from the data pins when they are powered off,
        // we will explicitly start and end the serial connection in the loop.
        // See https://github.com/EnviroDIY/ModularSensors/issues/140#issuecomment-389380833
        // modbusSerial.begin(9600);

        // Do a complete update on the variable array.
        // This this includes powering all of the sensors, getting updated
        // values, and turing them back off.
        // NOTE:  The wake function for each sensor should force sensor setup
        // to run if the sensor was not previously set up.
        varArray.completeUpdate();

        // Reset AltSoftSerial pins to LOW, to reduce RS485 adapter power bleed on sleep,
        // because Modbus Stop bit leaves these pins HIGH.  See https://github.com/EnviroDIY/ModularSensors/issues/140#issuecomment-389380833
        // digitalWrite(5, LOW);   // Reset AltSoftSerial Tx pin to LOW
        // digitalWrite(6, LOW);   // Reset AltSoftSerial Rx pin to LOW

        // Create a csv data record and save it to the log file
        dataLogger.logToSD();

        // Connect to the network
        // Again, we're only doing this if the battery is doing well
        if (getBatteryVoltage() > 3.65)
        {
            if (modem.connectInternet())
            {
                // Publish data to remotes
                dataLogger.publishDataToRemotes();

                // Sync the clock at midnight
                if (Logger::markedEpochTime != 0 && Logger::markedEpochTime % 86400 == 0)
                {
                    Serial.println(F("Running a daily clock sync..."));
                    dataLogger.setRTClock(modem.getNISTTime());
                }

                // Disconnect from the network
                modem.disconnectInternet();
            }
            // Turn the modem off
            modem.modemSleepPowerDown();
        }

        // Cut power from the SD card - without additional housekeeping wait
        dataLogger.turnOffSDcard(false);
        // Turn off the LED
        dataLogger.alertOff();
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));

        // Unset flag
        Logger::isLoggingNow = false;
    }

    // Start the stream for the modbus sensors, for sensor testingMode
    // Because RS485 adapters tend to "steal" current from the data pins when they are powered off,
    // we will explicitly start and end the serial connection in the loop.
    // See https://github.com/EnviroDIY/ModularSensors/issues/140#issuecomment-389380833
    // modbusSerial.begin(9600);

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::startTesting) dataLogger.testingMode();

    // Reset AltSoftSerial pins to LOW, to reduce RS485 adapter power bleed on sleep,
    // because Modbus Stop bit leaves these pins HIGH.  See https://github.com/EnviroDIY/ModularSensors/issues/140#issuecomment-389380833
    // digitalWrite(5, LOW);   // Reset AltSoftSerial Tx pin to LOW
    // digitalWrite(6, LOW);   // Reset AltSoftSerial Rx pin to LOW


    // Call the processor sleep
    dataLogger.systemSleep();
}
