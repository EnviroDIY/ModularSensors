/*****************************************************************************
data_saving.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.21.3

This sketch is an example of logging data to an SD card and sending only a
portion of that data to the EnviroDIY data portal.

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
const char *libraryVersion = "0.21.3";
// The name of this file
const char *sketchName = "data_saving.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 5;
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

// Create sample number, battery voltage, and free RAM variable pointers for the processor
Variable *mcuBoardBatt = new ProcessorStats_Batt(&mcuBoard, "12345678-abcd-1234-efgh-1234567890ab");
Variable *mcuBoardAvailableRAM = new ProcessorStats_FreeRam(&mcuBoard, "12345678-abcd-1234-efgh-1234567890ab");
Variable *mcuBoardSampNo = new ProcessorStats_SampleNumber(&mcuBoard, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Settings for Additional Serial Ports
// ==========================================================================

// The modem and a number of sensors communicate over UART/TTL - often called "serial".
// "Hardware" serial ports (automatically controlled by the MCU) are generally
// the most accurate and should be configured and used for as many peripherals
// as possible.  In some cases (ie, modbus communication) many sensors can share
// the same serial port.

#if not defined ARDUINO_ARCH_SAMD && not defined ATMEGA2560  // For AVR boards
// Unfortunately, most AVR boards have only one or two hardware serial ports,
// so we'll set up three types of extra software serial ports to use

// AltSoftSerial by Paul Stoffregen (https://github.com/PaulStoffregen/AltSoftSerial)
// is the most accurate software serial port for AVR boards.
// AltSoftSerial can only be used on one set of pins on each board so only one
// AltSoftSerial port can be used.
// Not all AVR boards are supported by AltSoftSerial.
#include <AltSoftSerial.h>
AltSoftSerial altSoftSerial;

// NeoSWSerial (https://github.com/SRGDamia1/NeoSWSerial) is the best software
// serial that can be used on any pin supporting interrupts.
// You can use as many instances of NeoSWSerial as you want.
// Not all AVR boards are supported by NeoSWSerial.
#include <NeoSWSerial.h>  // for the stream communication
const int8_t neoSSerial1Rx = 11;     // data in pin
const int8_t neoSSerial1Tx = -1;     // data out pin
NeoSWSerial neoSSerial1(neoSSerial1Rx, neoSSerial1Tx);
// To use NeoSWSerial in this library, we define a function to receive data
// This is just a short-cut for later
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(neoSSerial1Rx)));
}

// The "standard" software serial library uses interrupts that conflict
// with several other libraries used within this program, we must use a
// version of software serial that has been stripped of interrupts.
// NOTE:  Only use if necessary.  This is not a very accurate serial port!
const int8_t softSerialRx = A3;     // data in pin
const int8_t softSerialTx = A4;     // data out pin

#include <SoftwareSerial_ExtInts.h>  // for the stream communication
SoftwareSerial_ExtInts softSerial1(softSerialRx, softSerialTx);
#endif  // End software serial for avr boards


// The SAMD21 has 6 "SERCOM" ports, any of which can be used for UART communication.
// The "core" code for most boards defines one or more UART (Serial) ports with
// the SERCOMs and uses others for I2C and SPI.  We can create new UART ports on
// any available SERCOM.  The table below shows definitions for select boards.

// Board =>   Arduino Zero       Adafruit Feather    Sodaq Boards
// -------    ---------------    ----------------    ----------------
// SERCOM0    Serial1 (D0/D1)    Serial1 (D0/D1)     Serial (D0/D1)
// SERCOM1    Available          Available           Serial3 (D12/D13)
// SERCOM2    Available          Available           I2C (A4/A5)
// SERCOM3    I2C (D20/D21)      I2C (D20/D21)       SPI (D11/12/13)
// SERCOM4    SPI (D21/22/23)    SPI (D21/22/23)     SPI1/Serial2
// SERCOM5    EDBG/Serial        Available           Serial1

// If using a Sodaq board, do not define the new sercoms, instead:
// #define ENABLE_SERIAL2
// #define ENABLE_SERIAL3


#if defined ARDUINO_ARCH_SAMD
  #include <wiring_private.h> // Needed for SAMD pinPeripheral() function

#ifndef ENABLE_SERIAL2
// Set up a 'new' UART using SERCOM1
// The Rx will be on digital pin 11, which is SERCOM1's Pad #0
// The Tx will be on digital pin 10, which is SERCOM1's Pad #2
// NOTE:  SERCOM1 is undefinied on a "standard" Arduino Zero and many clones,
//        but not all!  Please check the variant.cpp file for you individual board!
//        Sodaq Autonomo's and Sodaq One's do NOT follow the 'standard' SERCOM definitions!
Uart Serial2(&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
// Hand over the interrupts to the sercom port
void SERCOM1_Handler()
{
    Serial2.IrqHandler();
}
#endif

#ifndef ENABLE_SERIAL3
// Set up a 'new' UART using SERCOM2
// The Rx will be on digital pin 5, which is SERCOM2's Pad #3
// The Tx will be on digital pin 2, which is SERCOM2's Pad #2
// NOTE:  SERCOM2 is undefinied on a "standard" Arduino Zero and many clones,
//        but not all!  Please check the variant.cpp file for you individual board!
//        Sodaq Autonomo's and Sodaq One's do NOT follow the 'standard' SERCOM definitions!
Uart Serial3(&sercom2, 5, 2, SERCOM_RX_PAD_3, UART_TX_PAD_2);
// Hand over the interrupts to the sercom port
void SERCOM2_Handler()
{
    Serial3.IrqHandler();
}
#endif

#endif  // End hardware serial on SAMD21 boards


// ==========================================================================
//    Wifi/Cellular Modem Main Chip Selection
// ==========================================================================

// Select your modem chip - this determines the exact commands sent to it
#define TINY_GSM_MODEM_SIM800  // Select for a SIMCOM SIM800, SIM900, or variant thereof


// ==========================================================================
//    Modem Pins
// ==========================================================================

const int8_t modemVccPin = -2;      // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemSleepRqPin = 23;  // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemStatusPin = 19;   // MCU pin used to read modem status (-1 if not applicable)


// ==========================================================================
//    TinyGSM Client
// ==========================================================================

// Include TinyGSM for the modem
// This include must be included below the define of the modem name!
#include <TinyGsmClient.h>

// Create a reference to the serial port for the modem
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
HardwareSerial &modemSerial = Serial1;  // Use hardware serial if possible

// Create a new TinyGSM modem to run on that serial port and return a pointer to it
TinyGsm *tinyModem = new TinyGsm(modemSerial);

// Use this to create a modem if you want to spy on modem communication through
// a secondary Arduino stream.  Make sure you install the StreamDebugger library!
// https://github.com/vshymanskyy/StreamDebugger
// Also make sure you comment out the modem creation above to use this.
// #include <StreamDebugger.h>
// StreamDebugger modemDebugger(modemSerial, Serial);
// TinyGsm *tinyModem = new TinyGsm(modemDebugger);

// Create a new TCP client on that modem and return a pointer to it
TinyGsmClient *tinyClient = new TinyGsmClient(*tinyModem);


// ==========================================================================
//    Specific Modem On-Off Methods
// ==========================================================================

// THIS ONLY APPLIES TO A SODAQ GPRSBEE R6!!!
// Describe the physical pin connection of your modem to your board
const long modemBaud = 9600;         // Communication speed of the modem
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool modemWakeFxn(void)
{
    digitalWrite(modemSleepRqPin, HIGH);
    digitalWrite(redLED, HIGH);  // A light just for show
    return true;
}
bool modemSleepFxn(void)
{
    digitalWrite(modemSleepRqPin, LOW);
    digitalWrite(redLED, LOW);
    return true;
}
void extraModemSetup(void){}


// ==========================================================================
//    Network Information and LoggerModem Object
// ==========================================================================
#include <LoggerModem.h>

// Network connection information
const char *apn = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId = "xxxxx";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = "xxxxx";  // The password for connecting to WiFi, unnecessary for gprs

// Create the loggerModem instance
// A "loggerModem" is a combination of a TinyGSM Modem, a Client, and functions for wake and sleep
loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, modemWakeFxn, modemSleepFxn, tinyModem, tinyClient, apn);

// Create RSSI and signal strength variable pointers for the modem
Variable *modemRSSI = new Modem_RSSI(&modem, "12345678-abcd-1234-efgh-1234567890ab");
Variable *modemSignalPct = new Modem_SignalPercent(&modem, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);

// Create a temperature variable pointer for the DS3231
Variable *ds3231Temp = new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y504 Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/YosemitechY504.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
#if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
#else
AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
#endif

byte y504ModbusAddress = 0x04;  // The modbus address of the Y504
const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y504NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption

// Create a Yosemitech Y504 dissolved oxygen sensor object
YosemitechY504 y504(y504ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y504NumberReadings);

// Create the dissolved oxygen percent, dissolved oxygen concentration, and
// temperature variable pointers for the Y504
Variable *y504DOpct = new YosemitechY504_DOpct(&y504, "12345678-abcd-1234-efgh-1234567890ab");
Variable *y504DOmgL = new YosemitechY504_DOmgL(&y504, "12345678-abcd-1234-efgh-1234567890ab");
Variable *y504Temp = new YosemitechY504_Temp(&y504, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y511 Turbidity Sensor with Wiper
// ==========================================================================
#include <sensors/YosemitechY511.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte y511ModbusAddress = 0x1A;  // The modbus address of the Y511
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y511NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption

// Create a Y511-A Turbidity sensor object
YosemitechY511 y511(y511ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y511NumberReadings);

// Create turbidity and temperature variable pointers for the Y511
Variable *y511Turb = new YosemitechY511_Turbidity(&y511, "12345678-abcd-1234-efgh-1234567890ab");
Variable *y511Temp = new YosemitechY511_Temp(&y511, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y514 Chlorophyll Sensor
// ==========================================================================
#include <sensors/YosemitechY514.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte y514ModbusAddress = 0x14;  // The modbus address of the Y514
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y514NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption

// Create a Y514 chlorophyll sensor object
YosemitechY514 y514(y514ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y514NumberReadings);

// Create chlorophyll concentration and temperature variable pointers for the Y514
Variable *y514Chloro = new YosemitechY514_Chlorophyll(&y514, "12345678-abcd-1234-efgh-1234567890ab");
Variable *y514Temp = new YosemitechY514_Temp(&y514, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y520 Conductivity Sensor
// ==========================================================================
#include <sensors/YosemitechY520.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte y520ModbusAddress = 0x20;  // The modbus address of the Y520
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y520NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption

// Create a Y520 conductivity sensor object
YosemitechY520 y520(y520ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y520NumberReadings);

// Create specific conductance and temperature variable pointers for the Y520
Variable *y520Cond = new YosemitechY520_Cond(&y520, "12345678-abcd-1234-efgh-1234567890ab");
Variable *y520Temp = new YosemitechY520_Temp(&y520, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
#include <VariableArray.h>

// FORM2: Fill array with already created and named variable pointers
// We put ALL of the variable pointers into the first array
Variable *variableList_complete[] = {
    mcuBoardSampNo,
    mcuBoardBatt,
    mcuBoardAvailableRAM,
    ds3231Temp,
    y504DOpct,
    y504DOmgL,
    y504Temp,
    y511Turb,
    y511Temp,
    y514Chloro,
    y514Temp,
    y520Cond,
    y520Temp,
    modemRSSI,
    modemSignalPct
};
// Count up the number of pointers in the array
int variableCount_complete = sizeof(variableList_complete) / sizeof(variableList_complete[0]);
// Create the VariableArray object
VariableArray arrayComplete(variableCount_complete, variableList_complete);


// Put only the particularly interesting variables into a second array
// NOTE:  We can the same variables into multiple arrays
Variable *variableList_toGo[] = {
    y504DOmgL,
    y504Temp,
    y511Turb,
    y514Chloro,
    y520Cond,
    modemRSSI
};
// Count up the number of pointers in the array
int variableCount_toGo = sizeof(variableList_toGo) / sizeof(variableList_toGo[0]);
// Create the VariableArray object
VariableArray arrayToGo(variableCount_toGo, variableList_toGo);


// ==========================================================================
//     The Logger Object[s]
// ==========================================================================
#include <LoggerBase.h>

// Create one new logger instance for the complete array
Logger loggerAllVars(LoggerID, loggingInterval, &arrayComplete);

// Create "another" logger for the variables to go out over the internet
Logger loggerToGo(LoggerID, loggingInterval, &arrayToGo);

// ==========================================================================
//    A Publisher to WikiWatershed
// ==========================================================================
// Device registration and sampling feature information can be obtained after
// registration at http://data.WikiWatershed.org
const char *registrationToken = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *samplingFeature = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID

// Create a data publisher for the EnviroDIY/WikiWatershed POST endpoint
// This is only attached to the logger with the shorter variable array
#include <publishers/EnviroDIYPublisher.h>
EnviroDIYPublisher EnviroDIYPOST(loggerToGo, registrationToken, samplingFeature);


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
    // Wait for USB connection to be established by PC
    // NOTE:  Only use this when debugging - if not connected to a PC, this
    // could prevent the script from starting
    #if defined SERIAL_PORT_USBVIRTUAL
      while (!SERIAL_PORT_USBVIRTUAL && (millis() < 10000)){}
    #endif

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

    // Start the stream for the modbus sensors; all currently supported modbus sensors use 9600 baud
    modbusSerial.begin(9600);

    // Assign pins SERCOM functionality for SAMD boards
    // NOTE:  This must happen *after* the begin
    #if defined ARDUINO_ARCH_SAMD
    #ifndef ENABLE_SERIAL2
    pinPeripheral(10, PIO_SERCOM);  // Serial2 Tx/Dout = SERCOM1 Pad #2
    pinPeripheral(11, PIO_SERCOM);  // Serial2 Rx/Din = SERCOM1 Pad #0
    #endif
    #endif
    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Set up the sleep/wake pin for the modem and put its inital value as "off"
    pinMode(modemSleepRqPin, OUTPUT);
    digitalWrite(modemSleepRqPin, LOW);

    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);

    // Attach the same modem to both loggers
    // It is only needed for the logger that will be sending out data, but
    // attaching it to both allows either logger to control NIST synchronization
    loggerAllVars.attachModem(modem);
    loggerToGo.attachModem(modem);
    loggerAllVars.setLoggerPins(wakePin, sdCardSSPin, sensorPowerPin, buttonPin, greenLED);

    // Set up the connection information with EnviroDIY for both loggers
    // Doing this for both loggers ensures that the header of the csv will have the tokens in it
    loggerAllVars.setSamplingFeatureUUID(samplingFeature);
    loggerToGo.setSamplingFeatureUUID(samplingFeature);

    // Note:  Please change these battery voltages to match your battery
    // Check that the battery is OK before powering the modem
    if (getBatteryVoltage() > 3.7)
    {
        modem.modemPowerUp();
        modem.wake();

        // Run any extra pre-set-up for the modem
        Serial.println(F("Running extra modem pre-setup"));
        extraModemSetup();
        modem.setup();

        // At very good battery voltage, or with suspicious time stamp, sync the clock
        // Note:  Please change these battery voltages to match your battery
        if (getBatteryVoltage() > 3.8 ||
            loggerAllVars.getNowEpoch() < 1546300800 ||  /*Before 01/01/2019*/
            loggerAllVars.getNowEpoch() > 1735689600)  /*Before 1/1/2025*/
        {
            // Synchronize the RTC with NIST
            Serial.println(F("Attempting to synchronize RTC with NIST"));
            if (modem.connectInternet(120000L))
            {
                loggerAllVars.setRTClock(modem.getNISTTime());
            }
        }
    }

    // Set up the sensors, except at lowest battery level
    // Like with the logger, because the variables are duplicated in the arrays,
    // we only need to do this for the complete array.
    if (getBatteryVoltage() > 3.4)
    {
        Serial.println(F("Setting up sensors..."));
        arrayComplete.setupSensors();
    }

    // Power down the modem
    modem.modemSleepPowerDown();

    // Create the log file, adding the default header to it
    // Do this last so we have the best chance of getting the time correct and
    // all sensor names correct
    // Writing to the SD card can be power intensive, so if we're skipping
    // the sensor setup we'll skip this too.
    if (getBatteryVoltage() > 3.4)
    {
        loggerAllVars.turnOnSDcard(true);  // true = wait for card to settle after power up
        loggerAllVars.createLogFile(true);  // true = write a new header
        loggerAllVars.turnOffSDcard(true);  // true = wait for internal housekeeping after write
    }

    // Call the processor sleep
    loggerAllVars.systemSleep();
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
void loop()
{
    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    // We're only doing anything at all if the battery is above 3.4V
    if (loggerAllVars.checkInterval() && getBatteryVoltage() > 3.4)
    {
        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;

        // Print a line to show new reading
        Serial.println(F("------------------------------------------"));
        // Turn on the LED to show we're taking a reading
        loggerAllVars.alertOn();
        // Power up the SD Card, but skip any waits after power up
        loggerAllVars.turnOnSDcard(false);

        // Turn on the modem to let it start searching for the network
        // Only turn the modem on if the battery at the last interval was high enough
        // NOTE:  if the modemPowerUp function is not run before the completeUpdate
        // function is run, the modem will not be powered and will not return
        // a signal strength readign.
        if (getBatteryVoltage() > 3.7)
            modem.modemPowerUp();

        // Start the stream for the modbus sensors
        // Because RS485 adapters tend to "steal" current from the data pins
        // we will explicitly start and end the serial connection in the loop.
        modbusSerial.begin(9600);

        // Do a complete update on the "full" array.
        // This this includes powering all of the sensors, getting updated
        // values, and turing them back off.
        // NOTE:  The wake function for each sensor should force sensor setup
        // to run if the sensor was not previously set up.
        arrayComplete.completeUpdate();

        // End the stream for the modbus sensors
        // Because RS485 adapters tend to "steal" current from the data pins
        // we will explicitly start and end the serial connection in the loop.
        modbusSerial.end();

        #if defined AltSoftSerial_h
        // Explicitly set the pin modes for the AltSoftSerial pins to make sure they're low
        pinMode(5, OUTPUT);  // On a Mayfly, pin D5 is the AltSoftSerial Tx pin
        pinMode(6, OUTPUT);  // On a Mayfly, pin D6 is the AltSoftSerial Rx pin
        digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        #endif

        #if defined ARDUINO_SAMD_ZERO
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
        #endif

        // Create a csv data record and save it to the log file
        loggerAllVars.logToSD();

        // Connect to the network
        // Again, we're only doing this if the battery is doing well
        if (getBatteryVoltage() > 3.7)
        {
            if (modem.connectInternet())
            {
                // Publish data to remotes
                loggerToGo.sendDataToRemotes();

                // Sync the clock at midnight
                // NOTE:  All loggers have the same clock, pick one
                if (Logger::markedEpochTime != 0 && Logger::markedEpochTime % 86400 == 0)
                {
                    Serial.println(F("Running a daily clock sync..."));
                    loggerAllVars.setRTClock(modem.getNISTTime());
                }

                // Disconnect from the network
                modem.disconnectInternet();
            }
            // Turn the modem off
            modem.modemSleepPowerDown();
        }

        // Cut power from the SD card - without additional housekeeping wait
        loggerAllVars.turnOffSDcard(false);
        // Turn off the LED
        loggerAllVars.alertOff();
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));

        // Unset flag
        Logger::isLoggingNow = false;
    }

    // Check if it was instead the testing interrupt that woke us up
    // Want to enter the testing mode for the "complete" logger so we can see
    // the data from _ALL_ sensors
    // NOTE:  The testingISR attached to the button at the end of the "setup()"
    // function turns on the startTesting flag.  So we know if that flag is set
    // then we want to run the testing mode function.
    if (Logger::startTesting) loggerAllVars.testingMode();

    // Call the processor sleep
    // Only need to do this for one of the loggers
    loggerAllVars.systemSleep();
}
