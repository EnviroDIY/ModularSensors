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
#define DreamHostPortalRX "TALK TO STROUD FOR THIS VALUE"

// Select your modem chip, comment out all of the others
#define TINY_GSM_MODEM_SIM800

// ---------------------------------------------------------------------------
// Include the base required libraries
// ---------------------------------------------------------------------------
#define MODULAR_SENSORS_OUTPUT Serial  // Without this there will be no output
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <LoggerDreamHost.h>

// ---------------------------------------------------------------------------
// Set up the sensor specific information
//   ie, pin locations, addresses, calibrations and related settings
// ---------------------------------------------------------------------------
// The name of this file
const char *SKETCH_NAME = "DWRI_CitSci.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "SL099";
// How frequently (in minutes) to log data
int LOGGING_INTERVAL = 5;
// Your logger's timezone.
const int TIME_ZONE = -5;
// Create a new logger instance
LoggerDreamHost EnviroDIYLogger;


// ==========================================================================
//    CAMPBELL OBS 3 / OBS 3+
// ==========================================================================
#include <CampbellOBS3.h>
// Campbell OBS 3+ Low Range calibration in Volts
const int OBSLowPin = 0;  // The low voltage analog pin
const float OBSLow_A = 4.0749E+00;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 9.1011E+01;  // The "B" value (X) from the low range calibration
const float OBSLow_C = -3.9570E-01;  // The "C" value from the low range calibration
const int OBS3Power = 22;  // switched sensor power is pin 22 on Mayfly
CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C);
// Campbell OBS 3+ High Range calibration in Volts
const int OBSHighPin = 1;  // The high voltage analog pin
const float OBSHigh_A = 5.2996E+01;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 3.7828E+02;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = -1.3927E+00;  // The "C" value from the high range calibration
CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C);


// ==========================================================================
//    Decagon CTD
// ==========================================================================
#include <DecagonCTD.h>
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const int numberReadings = 6;  // The number of readings to average
const int SDI12Data = 7;  // The pin the CTD is attached to
const int SDI12Power = 22;  // switched sensor power is pin 22 on Mayfly
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, numberReadings);


// ==========================================================================
//    EnviroDIY Mayfly
// ==========================================================================
#include <MayflyOnboardSensors.h>
const char *MFVersion = "v0.5";
EnviroDIYMayfly mayfly(MFVersion) ;

// ---------------------------------------------------------------------------
// The array that contains all valid variables
// ---------------------------------------------------------------------------
Variable *variableList[] = {
    new EnviroDIYMayfly_Batt(&mayfly),
    new EnviroDIYMayfly_Temp(&mayfly),
    new DecagonCTD_Cond(&ctd),
    new DecagonCTD_Temp(&ctd),
    new DecagonCTD_Depth(&ctd),
    new CampbellOBS3_Turbidity(&osb3low, "TurbLow"),
    new CampbellOBS3_Turbidity(&osb3high, "TurbHigh")
};
int variableCount = sizeof(variableList) / sizeof(variableList[0]);


// ---------------------------------------------------------------------------
// Device registration and sampling feature information
//   This should be obtained after registration at http://data.envirodiy.org
//   You can copy the entire code snippet directly into this block below.
// ---------------------------------------------------------------------------
const char *REGISTRATION_TOKEN = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *SAMPLING_FEATURE = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID
const char *UUIDs[] =                                                      // UUID array for device sensors
{
"12345678-abcd-1234-efgh-1234567890ab",   // Battery voltage (EnviroDIY_Mayfly_Volt)
"12345678-abcd-1234-efgh-1234567890ab",   // Temperature (EnviroDIY_Mayfly_Temp)
"12345678-abcd-1234-efgh-1234567890ab",   // Electrical conductivity (Decagon_CTD-10_EC)
"12345678-abcd-1234-efgh-1234567890ab",   // Temperature (Decagon_CTD-10_Temp)
"12345678-abcd-1234-efgh-1234567890ab",   // Water depth (Decagon_CTD-10_Depth)
"12345678-abcd-1234-efgh-1234567890ab",   // Turbidity (Campbell_OBS-3+_Turb)
"12345678-abcd-1234-efgh-1234567890ab"    // Turbidity (Campbell_OBS-3+_Turb)
};


// ---------------------------------------------------------------------------
// Device Connection Options and WebSDL Endpoints for POST requests
// ---------------------------------------------------------------------------
const int modemDTRPin = 23;  // Modem DTR Pin (Data Terminal Ready - used for sleep) (-1 if unconnected)
const int modemCTSPin = 19;   // Modem CTS Pin (Clear to Send) (-1 if unconnected)
const int modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (else -1)

DTRSleepType ModemSleepMode = held;  // How the modem is put to sleep
HardwareSerial &ModemSerial = Serial1; // The serial port for the modem - software serial can also be used.
const long ModemBaud = 9600;  // SIM800 auto-detects, but I've had trouble making it fast (19200 works)
const char *APN = "apn.konekt.io";  // The APN for the gprs connection, unnecessary for WiFi


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
void greenred4flash()
{
  for (int i = 1; i <= 4; i++) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    delay(50);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(50);
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
    // Start the serial connection with the *bee
    ModemSerial.begin(ModemBaud);

    // Set up pins for the LED's
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenred4flash();

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(SKETCH_NAME);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);

    // Set the timezone and offsets
    Logger::setTimeZone(TIME_ZONE);  // Logging in the given time zone
    // EnviroDIYLogger.setTZOffset(0);  // Also set the clock in that time zone
    Logger::setTZOffset(TIME_ZONE);  // Set the clock in UTC

    // Initialize the logger;
    EnviroDIYLogger.init(SD_SS_PIN, RTC_PIN, variableCount, variableList,
                LOGGING_INTERVAL, LoggerID);
    EnviroDIYLogger.setAlertPin(GREEN_LED);

    // Set up the connection with EnviroDIY
    EnviroDIYLogger.setToken(REGISTRATION_TOKEN);
    EnviroDIYLogger.setSamplingFeature(SAMPLING_FEATURE);
    EnviroDIYLogger.setUUIDs(UUIDs);

    EnviroDIYLogger.modem.setupModem(&ModemSerial, modemVCCPin, modemCTSPin, modemDTRPin, ModemSleepMode, APN);

    EnviroDIYLogger.setDreamHostPortalRX(DreamHostPortalRX);

    // Begin the logger
    EnviroDIYLogger.begin();

    // Check for debugging mode
    EnviroDIYLogger.checkForDebugMode(BUTTON_PIN, &Serial);
}


// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    // Log the data
    EnviroDIYLogger.log();
}
