/*****************************************************************************
simple_logging.ino
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
#include <LoggerBase.h>

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
const char *SKETCH_NAME = "simple_logging.ino";

// Mayfly version number
const char *MFVersion = "v0.3";

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "Mayfly_160073";
// How frequently (in minutes) to log data
int LOGGING_INTERVAL = 0;
// Your logger's timezone.
const int TIME_ZONE = -5;
// Create a new logger instance
LoggerBase Logger;

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
const int SonarData = 10;     // recieve pin
// const int SonarTrigger = 11;   // excite (power) pin
const int SonarTrigger = -1;   // excite (power) pin
// const int switchedPower = 22;    // sensor power is pin 22 on Mayfly

// Campbell OBS 3+: pin settings
//   Campbell OBS 3+ Low Range calibration
const int OBSLowPin = 0;  // The low voltage analog pin
const float OBSLow_A = -2.4763E-07;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 1.0569E-01;  // The "B" value (X) from the low range calibration
const float OBSLow_C = -2.9928E-01;  // The "C" value from the low range calibration
//   Campbell OBS 3+ High Range calibration
const int OBSHighPin = 1;  // The high voltage analog pin
const float OBSHigh_A = 3.5310E-05;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 4.0111E-01;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = 2.0709E-01;  // The "C" value from the high range calibration
// const int switchedPower = 22;    // sensor power is pin 22 on Mayfly

// ---------------------------------------------------------------------------
// 3. The array that contains all valid sensors
// ---------------------------------------------------------------------------
SensorBase *SENSOR_LIST[] = {
    new DecagonCTD_Depth(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),
    new DecagonCTD_Temp(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),
    new DecagonCTD_Cond(*CTDSDI12address, switchedPower, SDI12Data, numberReadings),
    new Decagon5TM_Ea(*TMSDI12address, switchedPower, SDI12Data),
    new Decagon5TM_Temp(*TMSDI12address, switchedPower, SDI12Data),
    new Decagon5TM_VWC(*TMSDI12address, switchedPower, SDI12Data),
    new DecagonES2_Cond(*ES2DI12address, switchedPower, SDI12Data),
    new DecagonES2_Temp(*ES2DI12address, switchedPower, SDI12Data),
    new MaxBotixSonar_Range(switchedPower, SonarData, SonarTrigger),
    new CampbellOBS3_Turbidity(switchedPower, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C),
    new CampbellOBS3_TurbHigh(switchedPower, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C),
    new MayflyOnboardTemp(MFVersion),
    new MayflyOnboardBatt(MFVersion),
    new MayflyFreeRam()
    // new YOUR_sensorName_HERE()
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
// Board setup info
// ---------------------------------------------------------------------------
const int SERIAL_BAUD = 9600;  // Serial port BAUD rate
const int GREEN_LED = 8;  // Pin for the green LED
const int RED_LED = 9;  // Pin for the red LED
const int RTC_PIN = A7;  // RTC Interrupt/Alarm pin
const int SD_SS_PIN = 12;  // SD Card Card Select/Slave Select Pin

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
    Logger.init(TIME_ZONE, SD_SS_PIN, RTC_PIN, sensorCount, SENSOR_LIST,
                LOGGING_INTERVAL, LoggerID, SAMPLING_FEATURE, UUIDs);
    Logger.setAlertPin(GREEN_LED);
    // Run the logger setup;
    Logger.begin();
}


// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    // Log the data
    Logger.log();
}
