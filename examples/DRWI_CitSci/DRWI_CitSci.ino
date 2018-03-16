/*****************************************************************************
DWRI_CitSci.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of logging data to an SD card and sending the data to
both the EnviroDIY data portal and Stroud's custom data portal as should be
used by groups involved with The William Penn Foundation's Delaware River
Watershed Initiative

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// Some define statements
#define STANDARD_SERIAL_OUTPUT Serial  // Without this there will be no output

#define DreamHostPortalRX "TALK TO STROUD FOR THIS VALUE"

// Select your modem chip
#define TINY_GSM_MODEM_SIM800

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <LoggerDreamHost.h>

// ==========================================================================
//    Basic Logger Settings
// ==========================================================================
// The name of this file
const char *sketchName = "DWRI_CitSci.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXX";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 5;
// Your logger's timezone.
const int8_t timeZone = -5;
// Create a new logger instance
LoggerDreamHost EnviroDIYLogger;


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <ProcessorStats.h>

const long serialBaud = 57600;  // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;  // Pin for the green LED (-1 if unconnected)
const int8_t redLED = 9;  // Pin for the red LED (-1 if unconnected)
const int8_t buttonPin = 21;  // Pin for a button to use to enter debugging mode (-1 if unconnected)
const int8_t wakePin = A7;  // Interrupt/Alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPin = 12;  // SD Card Chip Select/Slave Select Pin (must be defined!)

// Create the processor "sensor"
const char *MFVersion = "v0.5";
ProcessorStats mayfly(MFVersion) ;


// ==========================================================================
//    Modem/Internet connection options
// ==========================================================================
HardwareSerial &ModemSerial = Serial1; // The serial port for the modem - software serial can also be used.
const int8_t modemSleepRqPin = 23;  // Modem SleepRq Pin (for sleep requests) (-1 if unconnected)
const int8_t modemStatusPin = 19;   // Modem Status Pin (indicates power status) (-1 if unconnected)
const int8_t modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (-1 if unconnected)

ModemSleepType ModemSleepMode = modem_sleep_held;  // How the modem is put to sleep

const long ModemBaud = 9600;  // Modem baud rate

const char *apn = "apn.konekt.io";  // The APN for the gprs connection, unnecessary for WiFi

// Create the loggerModem instance
// A "loggerModem" is a combination of a TinyGSM Modem, a TinyGSM Client, and an on/off method
loggerModem modem;


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <MaximDS3231.h>
MaximDS3231 ds3231(1);


// ==========================================================================
//    CAMPBELL OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
#include <CampbellOBS3.h>
const int8_t OBS3Power = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t OBS3numberReadings = 10;
const uint8_t OBS3_ADS1115Address = 0x48;  // The I2C address of the ADS1115 ADC
// Campbell OBS 3+ Low Range calibration in Volts
const int8_t OBSLowPin = 0;  // The low voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSLow_A = 4.0749E+00;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 9.1011E+01;  // The "B" value (X) from the low range calibration
const float OBSLow_C = -3.9570E-01;  // The "C" value from the low range calibration
CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C, OBS3_ADS1115Address, OBS3numberReadings);
// Campbell OBS 3+ High Range calibration in Volts
const int8_t OBSHighPin = 1;  // The high voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSHigh_A = 5.2996E+01;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 3.7828E+02;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = -1.3927E+00;  // The "C" value from the high range calibration
CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C, OBS3_ADS1115Address, OBS3numberReadings);


// ==========================================================================
//    Decagon CTD Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <DecagonCTD.h>
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const uint8_t CTDnumberReadings = 6;  // The number of readings to average
const int8_t SDI12Data = 7;  // The pin the CTD is attached to
SDI12 SDI12Bus = SDI12(SDI12Data);  // Create an SDI-12 bus
const int8_t SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
DecagonCTD ctd(*CTDSDI12address, SDI12Bus, SDI12Power, CTDnumberReadings);


// ==========================================================================
//    The array that contains all variables to be logged
// ==========================================================================
Variable *variableList[] = {
    new ProcessorStats_Batt(&mayfly, "12345678-abcd-1234-efgh-1234567890ab"),
    new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonCTD_Cond(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonCTD_Temp(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonCTD_Depth(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new CampbellOBS3_Turbidity(&osb3low, "12345678-abcd-1234-efgh-1234567890ab", "TurbLow"),
    new CampbellOBS3_Turbidity(&osb3high, "12345678-abcd-1234-efgh-1234567890ab", "TurbHigh"),
    new Modem_RSSI(&modem, "12345678-abcd-1234-efgh-1234567890ab"),
    new Modem_SignalPercent(&modem, "12345678-abcd-1234-efgh-1234567890ab"),
};
int variableCount = sizeof(variableList) / sizeof(variableList[0]);


// ==========================================================================
// Device registration and sampling feature information
//   This should be obtained after registration at http://data.envirodiy.org
// ==========================================================================
const char *registrationToken = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *samplingFeature = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID


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

    // Initialize the logger
    EnviroDIYLogger.init(sdCardPin, wakePin, variableCount, variableList,
                loggingInterval, LoggerID);
    EnviroDIYLogger.setAlertPin(greenLED);

    // Setup the logger modem
    modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, apn);

    // Attach the modem to the logger
    EnviroDIYLogger.attachModem(&modem);

    // Set up the connection with EnviroDIY
    EnviroDIYLogger.setToken(registrationToken);
    EnviroDIYLogger.setSamplingFeatureUUID(samplingFeature);

    // Set up the connection with DreamHost
    EnviroDIYLogger.setDreamHostPortalRX(DreamHostPortalRX);

    // Begin the logger
    EnviroDIYLogger.begin();

    // Check for debugging mode
    pinMode(buttonPin, INPUT_PULLUP);
    enableInterrupt(buttonPin, Logger::testingISR, CHANGE);
    Serial.print(F("Push button on pin "));
    Serial.print(buttonPin);
    Serial.println(F(" at any time to enter sensor testing mode."));

    // Blink the LEDs really fast to show start-up is done
    greenredflash(6, 25);
}


// ==========================================================================
// Main loop function
// ==========================================================================
void loop()
{
    // Log the data
    EnviroDIYLogger.log();
}
