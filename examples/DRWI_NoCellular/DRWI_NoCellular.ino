/*****************************************************************************
DRWI_NoCellular.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of logging data from a Decagon CTD-10 and a Campbell
OBS 3+ to an SD card.

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
// The name of this file
const char *sketchName = "DRWI_NoCellular.ino";
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
const int8_t sdCardPin = 12;      // MCU SD card chip select/slave select pin (must be given!)
const int8_t sensorPowerPin = 22; // MCU pin controlling main sensor power (-1 if not applicable)

// Create and return the processor "sensor"
const char *MFVersion = "v0.5b";
ProcessorStats mayfly(MFVersion);

// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>
// Create and return the DS3231 sensor object
MaximDS3231 ds3231(1);


// ==========================================================================
//    CAMPBELL OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
#include <sensors/CampbellOBS3.h>
const int8_t OBS3Power = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t OBS3numberReadings = 10;
const uint8_t OBS3_ADS1115Address = 0x48;  // The I2C address of the ADS1115 ADC
// Campbell OBS 3+ Low Range calibration in Volts
const int8_t OBSLowPin = 0;  // The low voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSLow_A = xxxxE+xx;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = xxxxE+xx;  // The "B" value (X) from the low range calibration
const float OBSLow_C = xxxxE+xx;  // The "C" value from the low range calibration
// Create and return the Campbell OBS3+ LOW RANGE sensor object
CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C, OBS3_ADS1115Address, OBS3numberReadings);
// Campbell OBS 3+ High Range calibration in Volts
const int8_t OBSHighPin = 1;  // The high voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSHigh_A = xxxxE+xx;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = xxxxE+xx;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = xxxxE+xx;  // The "C" value from the high range calibration
// Create and return the Campbell OBS3+ HIGH RANGE sensor object
CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C, OBS3_ADS1115Address, OBS3numberReadings);


// ==========================================================================
//    Decagon CTD Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <sensors/DecagonCTD.h>
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const uint8_t CTDnumberReadings = 6;  // The number of readings to average
const int8_t SDI12Data = 7;  // The pin the CTD is attached to
const int8_t SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Decagon CTD sensor object
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, CTDnumberReadings);


// ==========================================================================
//    The array that contains all variables to be logged
// ==========================================================================
#include <VariableArray.h>
// Create pointers for all of the variables from the sensors
// at the same time putting them into an array
Variable *variableList[] = {
    new DecagonCTD_Cond(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonCTD_Temp(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonCTD_Depth(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new CampbellOBS3_Turbidity(&osb3low, "12345678-abcd-1234-efgh-1234567890ab", "TurbLow"),
    new CampbellOBS3_Turbidity(&osb3high, "12345678-abcd-1234-efgh-1234567890ab", "TurbHigh"),
    new ProcessorStats_Batt(&mayfly, "12345678-abcd-1234-efgh-1234567890ab"),
    new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-efgh-1234567890ab"),
};
// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);
// Create the VariableArray object
VariableArray varArray(variableCount, variableList);

// Create a new logger instance
#include <LoggerBase.h>
Logger logger(LoggerID, loggingInterval, sdCardPin, wakePin, &varArray);

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


// Read's the battery voltage
float getBatteryVoltage(const char *version = MFVersion)
{
    float batteryVoltage;
    if (strcmp(version, "v0.3") == 0 or strcmp(version, "v0.4") == 0)
    {
        // Get the battery voltage
        float rawBattery = analogRead(A6);
        batteryVoltage = (3.3 / 1023.) * 1.47 * rawBattery;
    }
    if (strcmp(version, "v0.5") == 0 or strcmp(version, "v0.5b") == 0)
    {
        // Get the battery voltage
        float rawBattery = analogRead(A6);
        batteryVoltage = (3.3 / 1023.) * 4.7 * rawBattery;
    }
    return batteryVoltage;
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

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);

    // Set pins for alerts and entering testing mode
    logger.setAlertPin(greenLED);
    logger.setTestingModePin(buttonPin);

    // Begin the logger
    Serial.print("Battery: ");
    Serial.println(getBatteryVoltage());
    if (getBatteryVoltage() < 3.4) logger.begin(true);  // skip sensor set-up
    else logger.begin();  // set up sensors
}


// ==========================================================================
// Main loop function
// ==========================================================================
void loop()
{
    // Log the data
    Serial.print("Battery: ");
    Serial.println(getBatteryVoltage());
    if (getBatteryVoltage() < 3.4) logger.systemSleep();  // just go back to sleep
    else logger.logData();  // log data
}
