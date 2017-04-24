/*****************************************************************************
logging_to_EnviroDIY.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of printing data from multiple sensors using
the modular sensor library.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/


// Select your modem chip, comment out all of the others
// #define TINY_GSM_MODEM_SIM800  // Select for anything using a SIM800, SIM900, or varient thereof: Sodaq GPRSBees, Microduino GPRS chips, Adafruit Fona, etc
// #define TINY_GSM_MODEM_A6  // Select for A6 or A7 chips
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_ESP8266
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand XBee's, including WiFi or LTE-M1

// ---------------------------------------------------------------------------
// Include the base required libraries
// ---------------------------------------------------------------------------
#include <Arduino.h>
#include <SensorBase.h>
#ifdef DreamHostPortalRX
#include <LoggerDreamHost.h>
#else
#include <LoggerEnviroDIY.h>
#endif

// ---------------------------------------------------------------------------
// Set up the sensor specific information
//   ie, pin locations, addresses, calibrations and related settings
// ---------------------------------------------------------------------------
// The name of this file
const char *SKETCH_NAME = "logging_to_EnviroDIY.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "Mayfly_160073";
// How frequently (in minutes) to log data
int LOGGING_INTERVAL = 5;
// Your logger's timezone.
const int TIME_ZONE = -5;
// Create a new logger instance
#ifdef DreamHostPortalRX
LoggerDreamHost EnviroDIYLogger;
#else
LoggerEnviroDIY EnviroDIYLogger;
#endif

// ==========================================================================
//    AOSong AM2315
// ==========================================================================
#include <AOSongAM2315.h>
// Campbell OBS 3+ Low Range calibration in Volts
const int I2CPower = 22;  // switched sensor power is pin 22 on Mayfly
AOSongAM2315 am2315(I2CPower);


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
//    Decagon 5TM
// ==========================================================================
#include <Decagon5TM.h>
const char *TMSDI12address = "2";  // The SDI-12 Address of the 5-TM
const int SDI12Data = 7;  // The pin the 5TM is attached to
const int SDI12Power = 22;  // switched sensor power is pin 22 on Mayfly
Decagon5TM fivetm(*TMSDI12address, SDI12Power, SDI12Data);


// ==========================================================================
//    Decagon CTD
// ==========================================================================
#include <DecagonCTD.h>
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const int numberReadings = 10;  // The number of readings to average
// const int SDI12Data = 7;  // The pin the CTD is attached to
// const int SDI12Power = 22;  // switched sensor power is pin 22 on Mayfly
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, numberReadings);


// ==========================================================================
//    Decagon ES2
// ==========================================================================
#include <DecagonES2.h>
const char *ES2SDI12address = "3";  // The SDI-12 Address of the ES2
// const int SDI12Data = 7;  // The pin the 5TM is attached to
// const int SDI12Power = 22;  // switched sensor power is pin 22 on Mayfly
DecagonES2 es2(*ES2SDI12address, SDI12Power, SDI12Data);


// ==========================================================================
//    Maxbotix HRXL
// ==========================================================================
#include <MaxBotixSonar.h>
const int SonarData = 10;     // data  pin
const int SonarTrigger = -1;   // Trigger pin
const int SonarPower = 22;   // excite (power) pin
MaxBotixSonar sonar(SonarPower, SonarData, SonarTrigger) ;


// ==========================================================================
//    EnviroDIY Mayfly
// ==========================================================================
#include <MayflyOnboardSensors.h>
const char *MFVersion = "v0.3";
EnviroDIYMayfly mayfly(MFVersion) ;

// ---------------------------------------------------------------------------
// The array that contains all valid variables
// ---------------------------------------------------------------------------
Variable *variableList[] = {
    new AOSongAM2315_Humidity(&am2315),
    new AOSongAM2315_Temp(&am2315),
    new CampbellOBS3_Turbidity(&osb3low),
    new CampbellOBS3_TurbHigh(&osb3high),
    new Decagon5TM_Ea(&fivetm),
    new Decagon5TM_Temp(&fivetm),
    new Decagon5TM_VWC(&fivetm),
    new DecagonCTD_Depth(&ctd),
    new DecagonCTD_Temp(&ctd),
    new DecagonCTD_Cond(&ctd),
    new DecagonES2_Cond(&es2),
    new DecagonES2_Temp(&es2),
    new MaxBotixSonar_Range(&sonar),
    new EnviroDIYMayfly_Temp(&mayfly),
    new EnviroDIYMayfly_Batt(&mayfly),
    new EnviroDIYMayfly_FreeRam(&mayfly)
    // new YOUR_variableName_HERE(&)
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
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab",
"12345678-abcd-1234-efgh-1234567890ab"
};


// ---------------------------------------------------------------------------
// Device Connection Options and WebSDL Endpoints for POST requests
// ---------------------------------------------------------------------------
DTRSleepType ModemSleepMode = held;  // How the modem is put to sleep
// Use "held" if the DTR pin is held HIGH to keep the modem awake, as with a Sodaq GPRSBee rev6.
// Use "pulsed" if the DTR pin is pulsed high and then low to wake the modem up, as with an Adafruit Fona or Sodaq GPRSBee rev4.
// Use "reverse" if the DTR pin is held LOW to keep the modem awake, as with all XBees.
// Use "always_on" if you do not want the library to control the modem power and sleep.
HardwareSerial &ModemSerial = Serial1; // The serial port for the modem - software serial can also be used.
const int ModemBaud = 9600;  // Modem BAUD rate (9600 is default), can use higher for SIM800 (19200 works)
const char *APN = "apn.konekt.io";  // The APN for the gprs connection, unnecessary for WiFi
const char *SSID = "XXXXXXX";  // The WiFi access point, unnecessary for gprs
const char *PWD = "XXXXXXX";  // The password for connecting to WiFi, unnecessary for gprs


// ---------------------------------------------------------------------------
// Board setup info
// ---------------------------------------------------------------------------
const int SERIAL_BAUD = 9600;  // Serial port BAUD rate
const int GREEN_LED = 8;  // Pin for the green LED
const int RED_LED = 9;  // Pin for the red LED
const int RTC_PIN = A7;  // RTC Interrupt/Alarm pin
const int SD_SS_PIN = 12;  // SD Card Card Select/Slave Select Pin

const int BEE_DTR_PIN = 23;  // Bee DTR Pin (Data Terminal Ready - used for sleep)
const int BEE_CTS_PIN = 19;   // Bee CTS Pin (Clear to Send)
const int BEE_VCC_PIN = -1;

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
    Serial.print(F("There are "));
    Serial.print(String(variableCount));
    Serial.println(F(" variables being recorded"));

    // Set the timezone and offsets
    EnviroDIYLogger.setTimeZone(TIME_ZONE);
    EnviroDIYLogger.setTZOffset(0);

    // Initialize the logger;
    EnviroDIYLogger.init(SD_SS_PIN, RTC_PIN, variableCount, variableList,
                LOGGING_INTERVAL, LoggerID);
    EnviroDIYLogger.setAlertPin(GREEN_LED);

    // Set up the connection with EnviroDIY
    EnviroDIYLogger.setToken(REGISTRATION_TOKEN);
    EnviroDIYLogger.setSamplingFeature(SAMPLING_FEATURE);
    EnviroDIYLogger.setUUIDs(UUIDs);

    #if defined(TINY_GSM_MODEM_XBEE) || defined(TINY_GSM_MODEM_ESP8266)
        EnviroDIYLogger.modem.setupModem(&ModemSerial, BEE_VCC_PIN, BEE_CTS_PIN, BEE_DTR_PIN, ModemSleepMode, SSID, PWD);
    #else
        EnviroDIYLogger.modem.setupModem(&ModemSerial, BEE_VCC_PIN, BEE_CTS_PIN, BEE_DTR_PIN, ModemSleepMode, APN);
    #endif

    #ifdef DreamHostPortalRX
        EnviroDIYLogger.setDreamHostPortalRX(DreamHostPortalRX);
    #endif
    // Begin the logger;
    EnviroDIYLogger.begin();
}


// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    // Log the data
    EnviroDIYLogger.log();
}
