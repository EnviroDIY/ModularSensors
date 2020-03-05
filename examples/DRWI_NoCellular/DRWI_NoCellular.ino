/*****************************************************************************
DRWI_NoCellular.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of logging data to an SD card as should be used by
groups involved with The William Penn Foundation's Delaware River Watershed
Initiative at sites without cellular service.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ==========================================================================
//    Defines for the Arduino IDE
//    In PlatformIO, set these build flags in your platformio.ini
// ==========================================================================

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>          // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <LoggerBase.h>       // The modular sensors library


// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The name of this file
const char* sketchName = "DRWI_NoCellular.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char* LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 5;
// Your logger's timezone.
const int8_t timeZone = -5;  // Eastern Standard Time
// NOTE:  Daylight savings time will not be applied!  Please use standard time!


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <sensors/ProcessorStats.h>
// NOTE:  Use -1 for pins that do not apply
const long   serialBaud = 115200;  // Baud rate for debugging
const int8_t greenLED   = 8;       // Pin for the green LED
const int8_t redLED     = 9;       // Pin for the red LED
const int8_t buttonPin  = 21;      // Pin for debugging mode (ie, button pin)
const int8_t wakePin    = A7;      // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPwrPin   = -1;  // MCU SD card power pin
const int8_t sdCardSSPin    = 12;  // SD card chip select/slave select pin
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power

// Create the main processor chip "sensor" - for general metadata
const char*    mcuBoardVersion = "v0.5b";
ProcessorStats mcuBoard(mcuBoardVersion);


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);


// ==========================================================================
//    Campbell OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
#include <sensors/CampbellOBS3.h>

const int8_t  OBS3Power = sensorPowerPin;  // Power pin (-1 if unconnected)
const uint8_t OBS3NumberReadings = 10;
const uint8_t ADSi2c_addr        = 0x48;  // The I2C address of the ADS1115 ADC
// Campbell OBS 3+ Low Range calibration in Volts
const int8_t OBSLowADSChannel = 0;          // ADS channel for LOW range output
const float  OBSLow_A         = 0.000E+00;  // "A" value (X^2) [LOW range]
const float  OBSLow_B         = 1.000E+00;  // "B" value (X) [LOW range]
const float  OBSLow_C         = 0.000E+00;  // "C" value [LOW range]

// Create a Campbell OBS3+ LOW RANGE sensor object
CampbellOBS3 osb3low(OBS3Power, OBSLowADSChannel, OBSLow_A, OBSLow_B, OBSLow_C,
                     ADSi2c_addr, OBS3NumberReadings);


// Campbell OBS 3+ High Range calibration in Volts
const int8_t OBSHighADSChannel = 1;  // ADS channel for HIGH range output
const float  OBSHigh_A         = 0.000E+00;  // "A" value (X^2) [HIGH range]
const float  OBSHigh_B         = 1.000E+00;  // "B" value (X) [HIGH range]
const float  OBSHigh_C         = 0.000E+00;  // "C" value [HIGH range]

// Create a Campbell OBS3+ HIGH RANGE sensor object
CampbellOBS3 osb3high(OBS3Power, OBSHighADSChannel, OBSHigh_A, OBSHigh_B,
                      OBSHigh_C, ADSi2c_addr, OBS3NumberReadings);


// ==========================================================================
//    Decagon CTD Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <sensors/DecagonCTD.h>

const char*   CTDSDI12address   = "1";      // The SDI-12 Address of the CTD
const uint8_t CTDNumberReadings = 6;        // The number of readings to average
const int8_t  SDI12Power = sensorPowerPin;  // Power pin (-1 if unconnected)
const int8_t  SDI12Data  = 7;               // The SDI12 data pin

// Create a Decagon CTD sensor object
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, CTDNumberReadings);


// ==========================================================================
//    Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================

Variable* variableList[] = {
    new DecagonCTD_Cond(&ctd),
    new DecagonCTD_Temp(&ctd),
    new DecagonCTD_Depth(&ctd),
    new CampbellOBS3_Turbidity(&osb3low, "", "TurbLow"),
    new CampbellOBS3_Turbidity(&osb3high, "", "TurbHigh"),
    new ProcessorStats_Battery(&mcuBoard),
    new MaximDS3231_Temp(&ds3231),
};
// *** CAUTION --- CAUTION --- CAUTION --- CAUTION --- CAUTION ***
// Check the order of your variables in the variable list!!!
// Be VERY certain that they match the order of your UUID's!
// Rearrange the variables in the variable list if necessary to match!
// *** CAUTION --- CAUTION --- CAUTION --- CAUTION --- CAUTION ***
const char* UUIDs[] = {
    "12345678-abcd-1234-ef00-1234567890ab",   // Electrical conductivity (Decagon_CTD-10_Cond)
    "12345678-abcd-1234-ef00-1234567890ab",   // Temperature (Decagon_CTD-10_Temp)
    "12345678-abcd-1234-ef00-1234567890ab",   // Water depth (Decagon_CTD-10_Depth)
    "12345678-abcd-1234-ef00-1234567890ab",   // Turbidity (Campbell_OBS3_Turb)
    "12345678-abcd-1234-ef00-1234567890ab",   // Turbidity (Campbell_OBS3_Turb)
    "12345678-abcd-1234-ef00-1234567890ab",   // Battery voltage (EnviroDIY_Mayfly_Batt)
    "12345678-abcd-1234-ef00-1234567890ab"    // Temperature (EnviroDIY_Mayfly_Temp)
};
const char* registrationToken = "12345678-abcd-1234-ef00-1234567890ab";  // Device registration token
const char* samplingFeature = "12345678-abcd-1234-ef00-1234567890ab";  // Sampling feature UUID

// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList, UUIDs);


// ==========================================================================
//     The Logger Object[s]
// ==========================================================================

// Create a new logger instance
Logger dataLogger(LoggerID, loggingInterval, &varArray);

// Device registration and sampling feature information
// This should be obtained after registration at http://data.envirodiy.org
// This is needed so the logger file will be "drag-and-drop" ready for manual
// upload to the portal.


// ==========================================================================
//    Working Functions
// ==========================================================================

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


// Read's the battery voltage
// NOTE: This will actually return the battery level from the previous update!
float getBatteryVoltage() {
    if (mcuBoard.sensorValues[0] == -9999) mcuBoard.update();
    return mcuBoard.sensorValues[0];
}


// ==========================================================================
// Main setup function
// ==========================================================================
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

    // Attach information pins to the logger
    dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin,
                             greenLED);
    dataLogger.setSamplingFeatureUUID(samplingFeature);

    // Begin the logger
    dataLogger.begin();

    // Note:  Please change these battery voltages to match your battery
    // Set up the sensors, except at lowest battery level
    if (getBatteryVoltage() > 3.4) {
        Serial.println(F("Setting up sensors..."));
        varArray.setupSensors();
    }

    // Create the log file, adding the default header to it
    // Do this last so we have the best chance of getting the time correct and
    // all sensor names correct
    // Writing to the SD card can be power intensive, so if we're skipping
    // the sensor setup we'll skip this too.
    if (getBatteryVoltage() > 3.4) {
        Serial.println(F("Setting up file on SD card"));
        dataLogger.turnOnSDcard(
            true);  // true = wait for card to settle after power up
        dataLogger.createLogFile(true);  // true = write a new header
        dataLogger.turnOffSDcard(
            true);  // true = wait for internal housekeeping after write
    }

    // Call the processor sleep
    Serial.println(F("Putting processor to sleep\n"));
    dataLogger.systemSleep();
}


// ==========================================================================
// Main loop function
// ==========================================================================

// Use this short loop for simple data logging and sending
void loop() {
    // Note:  Please change these battery voltages to match your battery
    // At very low battery, just go back to sleep
    if (getBatteryVoltage() < 3.4) {
        dataLogger.systemSleep();
    }
    // If the battery is OK, log data
    else {
        dataLogger.logData();
    }
}
