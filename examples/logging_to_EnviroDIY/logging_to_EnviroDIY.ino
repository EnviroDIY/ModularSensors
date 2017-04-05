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

// ---------------------------------------------------------------------------
// Include the base required libraries
// ---------------------------------------------------------------------------
#include <Arduino.h>
#include <SensorBase.h>

#ifdef DreamHostURL
#include <LoggerDreamHost.h>
#else
#include <LoggerEnviroDIY.h>
#endif

#include <DecagonCTD.h>
#include <Decagon5TM.h>
#include <DecagonES2.h>
#include <CampbellOBS3.h>
#include <MaxBotixSonar.h>
#include <MayflyOnboardSensors.h>

// ---------------------------------------------------------------------------
// Set up the sensor specific information
//   ie, pin locations, addresses, calibrations and related settings
// ---------------------------------------------------------------------------
// The name of this file
const char *SKETCH_NAME = "logging_to_EnviroDIY.ino";

// Mayfly version number
const char *MFVersion = "v0.3";

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "Mayfly_160073";
// How frequently (in minutes) to log data
int LOGGING_INTERVAL = 5;
// Your logger's timezone.
const int TIME_ZONE = -5;

// Decagon CTD: pin settings
// sdi-12 data pin is usually, pin 7 on shield 3.0
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const int numberReadings = 10;  // The number of readings to average
const int SDI12Data = 7;  // The pin the CTD is attached to
const int switchedPower = 22;  // sensor power is pin 22 on Mayfly

// Decagon 5TM: pin settings
// sdi-12 data pin is usually, pin 7 on shield 3.0
const char *TMSDI12address = "2";  // The SDI-12 Address of the 5-TM
// const int SDI12Data = 7;  // The pin the 5TM is attached to
// const int switchedPower = 22;  // sensor power is pin 22 on Mayfly

// Decagon ES2: pin settings
// sdi-12 data pin is usually, pin 7 on shield 3.0
const char *ES2DI12address = "3";  // The SDI-12 Address of the 5-TM
// const int SDI12Data = 7;  // The pin the 5TM is attached to
// const int switchedPower = 22;  // sensor power is pin 22 on Mayfly

// MaxBotix Sonar: pin settings
const int SonarData = 10;     // data  pin
// const int SonarPower = 11;   // excite (power) pin
const int SonarTrigger = -1;   // Trigger pin
// const int switchedPower = 22;    // sensor power is pin 22 on Mayfly

// Campbell OBS 3+: pin settings
// Campbell OBS 3+ Low Range calibration in Volts
const int OBSLowPin = 0;  // The low voltage analog pin
const float OBSLow_A = 4.0749E+00;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 9.1011E+01;  // The "B" value (X) from the low range calibration
const float OBSLow_C = -3.9570E-01;  // The "C" value from the low range calibration
// Campbell OBS 3+ High Range calibration in Volts
const int OBSHighPin = 1;  // The high voltage analog pin
const float OBSHigh_A = 5.2996E+01;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 3.7828E+02;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = -1.3927E+00;  // The "C" value from the high range calibration
// const int switchedPower = 22;    // sensor power is pin 22 on Mayfly

// ---------------------------------------------------------------------------
// 3. The array that contains all valid sensors
// ---------------------------------------------------------------------------
SensorBase *SENSOR_LIST[] = {
    new DecagonCTD_Cond(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),
    new DecagonCTD_Temp(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),
    new DecagonCTD_Depth(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),
    new Decagon5TM_Ea(*TMSDI12address, switchedPower, SDI12Data),
    new Decagon5TM_VWC(*TMSDI12address, switchedPower, SDI12Data),
    new Decagon5TM_Temp(*TMSDI12address, switchedPower, SDI12Data),
    new DecagonES2_Cond(*ES2DI12address, switchedPower, SDI12Data),
    new DecagonES2_Temp(*ES2DI12address, switchedPower, SDI12Data),
    new CampbellOBS3_Turbidity(switchedPower, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C),
    new CampbellOBS3_TurbHigh(switchedPower, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C),
    new MaxBotixSonar_Range(switchedPower, SonarData, SonarTrigger),
        new MayflyOnboardTemp(MFVersion),
        new MayflyOnboardBatt(MFVersion),
        new MayflyFreeRam()
};
int sensorCount = sizeof(SENSOR_LIST) / sizeof(SENSOR_LIST[0]);


// ---------------------------------------------------------------------------
// Device registration and sampling feature information
//   This should be obtained after registration at http://data.envirodiy.org
// ---------------------------------------------------------------------------

const char *REGISTRATION_TOKEN = "12345678-abcd-1234-efgh-1234567890ab";
const char *SAMPLING_FEATURE = "12345678-abcd-1234-efgh-1234567890ab";

const char *UUIDs[] =
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
"12345678-abcd-1234-efgh-1234567890ab"
};

// ---------------------------------------------------------------------------
// Device Connection Options and WebSDL Endpoints for POST requests
// ---------------------------------------------------------------------------
xbee BEE_TYPE = WIFI;  // The type of XBee, either GPRSv4, GPRSv6, or WIFI
HardwareSerial &BeeSerial = Serial1; // The serial port for the xbee - software serial can also be used.
const int BEE_BAUD = 9600;  // Bee BAUD rate (9600 is default)
const char *APN = "apn.konekt.io";  // The APN for the GPRSBee, unnecessary for WiFi


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

// Create a new logger instance
#ifdef DreamHostURL
LoggerDreamHost EnviroDIYLogger;
#else
LoggerEnviroDIY EnviroDIYLogger;
#endif



// ---------------------------------------------------------------------------
// Main setup function
// ---------------------------------------------------------------------------
void setup()
{
    // Start the primary serial connection
    Serial.begin(SERIAL_BAUD);
    // Start the serial connection with the *bee
    BeeSerial.begin(BEE_BAUD);

    // Set up pins for the LED's
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenred4flash();

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(SKETCH_NAME);
    Serial.print(F(" on EnviroDIY Mayfly "));
    Serial.println(LoggerID);

    // Initialize the logger;
    EnviroDIYLogger.init(TIME_ZONE, SD_SS_PIN, RTC_PIN, sensorCount, SENSOR_LIST,
                LOGGING_INTERVAL, LoggerID);
    EnviroDIYLogger.setAlertPin(GREEN_LED);
    // Set up the communication with EnviroDIY
    EnviroDIYLogger.setToken(REGISTRATION_TOKEN);
    EnviroDIYLogger.setSamplingFeature(SAMPLING_FEATURE);
    EnviroDIYLogger.setUUIDs(UUIDs);
    EnviroDIYLogger.setupBee(BEE_TYPE, &BeeSerial, BEE_CTS_PIN, BEE_DTR_PIN, APN);
    #ifdef DreamHostURL
    EnviroDIYLogger.setDreamHostURL(DreamHostURL);
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
