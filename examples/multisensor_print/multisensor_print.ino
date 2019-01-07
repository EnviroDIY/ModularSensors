/*****************************************************************************
multisensor_print.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.19.3

This sketch is an example of printing data from multiple sensors using
the modular sensor library.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <Sodaq_DS3231.h>    // Controls the DS3231 Real Time Clock (RTC) built into the EnviroDIY Mayfly.
#include <VariableArray.h>


// The library version this example was written for
const char *libraryVersion = "0.19.3";
// The name of this file
const char *sketchName = "modular_sensors.ino";


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <sensors/ProcessorStats.h>

const long serialBaud = 115200;  // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;  // Pin for the green LED (-1 if unconnected)
const int8_t redLED = 9;  // Pin for the red LED (-1 if unconnected)

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
//    The array that contains all variables to be logged
// ==========================================================================
// Create pointers for all of the variables from the sensors
// at the same time putting them into an array
Variable *variableList[] = {
    new ProcessorStats_SampleNumber(&mayfly),
    new ProcessorStats_FreeRam(&mayfly),
    new ProcessorStats_Batt(&mayfly),
    new MaximDS3231_Temp(&ds3231)
};
// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray sensors(variableCount, variableList);


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

// The clock's timezone.
const int8_t timeZone = -5;

// Helper function to get the current date/time from the RTC
// as a unix timestamp - and apply the correct time zone.
long currentepochtime = 0;
uint32_t getNow()
{
  currentepochtime = rtc.now().getEpoch();
  currentepochtime += timeZone*3600;
  return currentepochtime;
}

// This function returns the datetime from the realtime clock as an ISO 8601 formated string
String getDateTime_ISO8601(void)
{
  String dateTimeStr;
  //Create a DateTime object from the current time
  DateTime dt(rtc.makeDateTime(getNow()));
  //Convert it to a String
  dt.addToString(dateTimeStr);
  dateTimeStr.replace(F(" "), F("T"));
  String tzString = String(timeZone);
  if (-24 <= timeZone && timeZone <= -10)
  {
      tzString += F(":00");
  }
  else if (-10 < timeZone && timeZone < 0)
  {
      tzString = tzString.substring(0,1) + F("0") + tzString.substring(1,2) + F(":00");
  }
  else if (timeZone == 0)
  {
      tzString = F("Z");
  }
  else if (0 < timeZone && timeZone < 10)
  {
      tzString = "+0" + tzString + F(":00");
  }
  else if (10 <= timeZone && timeZone <= 24)
  {
      tzString = "+" + tzString + F(":00");
  }
  dateTimeStr += tzString;
  return dateTimeStr;
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
    Serial.println(sketchName);

    Serial.print(F("Using ModularSensors Library version "));
    Serial.println(MODULAR_SENSORS_VERSION);

    if (String(MODULAR_SENSORS_VERSION) !=  String(libraryVersion))
        Serial.println(F(
            "WARNING: THIS EXAMPLE WAS WRITTEN FOR A DIFFERENT VERSION OF MODULAR SENSORS!!"));

    // Start the Real Time Clock
    rtc.begin();
    delay(100);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Print out the current time
    Serial.print(F("Current Mayfly RTC time is: "));
    Serial.println(getDateTime_ISO8601());
    Serial.print(F("There are "));
    Serial.print(String(variableCount));
    Serial.println(F(" variables to be recorded."));

    // Set up all the sensors
    sensors.setupSensors();

    Serial.println(F("Setup finished!"));
    Serial.println(F("------------------------------------------\n"));
}


// ==========================================================================
// Main loop function
// ==========================================================================
void loop()
{
    // Print a line to show new reading
    Serial.println(F("------------------------------------------"));
    // Power the sensors;
    digitalWrite(22, HIGH);
    // One second warm-up time
    delay(1000);
    // Turn on the LED to show we're taking a reading
    digitalWrite(greenLED, HIGH);

    // Send power to all of the sensors
    Serial.print(F("Powering sensors...\n"));
    sensors.sensorsPowerUp();
    // Wake up all of the sensors
    Serial.print(F("Waking sensors...\n"));
    sensors.sensorsWake();
    // Update the values from all attached sensors
    Serial.print(F("Updating sensor values...\n"));
    sensors.updateAllSensors();
    // Put sensors to sleep
    Serial.print(F("Putting sensors back to sleep...\n"));
    sensors.sensorsSleep();
    // Cut sensor power
    Serial.print(F("Cutting sensor power...\n"));
    sensors.sensorsPowerDown();

    // Print the data to the screen
    Serial.print(F("Updated all sensors at "));
    Serial.println(getDateTime_ISO8601());
    sensors.printSensorData(&Serial);
    // Turn off the LED to show we're done with the reading
    digitalWrite(greenLED, LOW);
    // Print a to close it off
    Serial.println(F("------------------------------------------\n"));

    // Wait for the next reading
    delay(10000);
}
