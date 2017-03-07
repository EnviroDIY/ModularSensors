/**************************************************************************
multisensor_print.ino
Written By:  JSara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of printing data from multiple sensors using
the modular sensor library.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
**************************************************************************/
// -----------------------------------------------
// 1. Include all required libraries
// -----------------------------------------------
#include <Sodaq_DS3231.h>    // Controls the DS3231 Real Time Clock (RTC) built into the EnviroDIY Mayfly.
#include <SdFat.h>  // Controls communication with the SD card
#include "Config.h"


// Variables for the timer function
long currentepochtime = 0;
char currentTime[26] = "";

// For the number of sensors
int sensorCount = 0;

// Helper function to get the current date/time from the RTC
// as a unix timestamp - and apply the correct time zone.
uint32_t getNow()
{
  currentepochtime = rtc.now().getEpoch();
  currentepochtime += TIME_ZONE*3600;
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
  String tzString = String(TIME_ZONE);
  if (-24 <= TIME_ZONE && TIME_ZONE <= -10)
  {
      tzString += F(":00");
  }
  else if (-10 < TIME_ZONE && TIME_ZONE < 0)
  {
      tzString = tzString.substring(0,1) + F("0") + tzString.substring(1,2) + F(":00");
  }
  else if (TIME_ZONE == 0)
  {
      tzString = F("Z");
  }
  else if (0 < TIME_ZONE && TIME_ZONE < 10)
  {
      tzString = "+0" + tzString + F(":00");
  }
  else if (10 <= TIME_ZONE && TIME_ZONE <= 24)
  {
      tzString = "+" + tzString + F(":00");
  }
  dateTimeStr += tzString;
  return dateTimeStr;
}


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


// This sets up the sensors, generally setting pin modes and the like
bool setupSensors(void)
{
    bool success = true;
    bool sensorSuccess = false;
    int setupTries = 0;
    for (int i = 0; i < sensorCount; i++)
    {
        // Make 5 attempts before giving up
        while(setupTries < 5)
        {
            sensorSuccess = SENSOR_LIST[i]->setup();
            // Prints for debugging
            if(sensorSuccess)
            {
                Serial.print(F("--- Successfully set up "));
                Serial.print(SENSOR_LIST[i]->getSensorName());
                Serial.println(F(" ---"));
                break;
            }
            else
            {
                Serial.print(F("--- Setup for  "));
                Serial.print(SENSOR_LIST[i]->getSensorName());
                Serial.println(F(" failed! ---"));
                setupTries++;
            }
        }
        success &= sensorSuccess;

        // Check for and skip the setup of any identical sensors
        for (int j = i+1; j < sensorCount; j++)
        {
            if (SENSOR_LIST[i]->getSensorName() == SENSOR_LIST[j]->getSensorName() &&
                SENSOR_LIST[i]->getSensorLocation() == SENSOR_LIST[j]->getSensorLocation())
            {i++;}
            else {break;}
        }
    }
    return success;
}

// This function updates the values for any connected sensors.
bool updateAllSensors()
{
    // Get the clock time when we begin updating sensors
    getDateTime_ISO8601().toCharArray(currentTime, 26) ;

    bool success = true;
    for (int i = 0; i < sensorCount; i++)
    {
        success &= SENSOR_LIST[i]->update();
        // Prints for debugging
        Serial.print(F("--- Updated "));
        Serial.print(SENSOR_LIST[i]->getSensorName());
        Serial.print(F(" for "));
        Serial.print(SENSOR_LIST[i]->getVarName());

        // Check for and skip the updates of any identical sensors
        for (int j = i+1; j < sensorCount; j++)
        {
            if (SENSOR_LIST[i]->getSensorName() == SENSOR_LIST[j]->getSensorName() &&
                SENSOR_LIST[i]->getSensorLocation() == SENSOR_LIST[j]->getSensorLocation())
            {
                // Prints for debugging
                Serial.print(F(" and "));
                Serial.print(SENSOR_LIST[i+1]->getVarName());
                i++;
            }
            else {break;}
        }
        Serial.println(F(" ---"));  // For Debugging
        // delay(250);  // A short delay before next sensor - is this needed??;
    }

    return success;
}

String generateSensorDataCSV(void)
{
    String csvString = String(currentTime) + F(", ");

    for (int i = 0; i < sensorCount; i++)
    {
        csvString += String(SENSOR_LIST[i]->getValue());
        if (i + 1 != sensorCount)
        {
            csvString += F(", ");
        }
    }

    return csvString;
}

String checkSensorLocations(void)
{
    String locationString = String(currentTime) + F(", ");

    for (int i = 0; i < sensorCount; i++)
    {
        locationString += String(SENSOR_LIST[i]->getSensorLocation());
        if (i + 1 != sensorCount)
        {
            locationString += F(", ");
        }
    }

    return locationString;
}


// -----------------------------------------------
// Main setup function
// -----------------------------------------------
void setup()
{
    // Start the primary serial connection
    Serial.begin(SERIAL_BAUD);
    // Start the serial connection with the *bee
    Serial1.begin(BEE_BAUD);

    // Start the Real Time Clock
    rtc.begin();
    delay(100);

    // Set up pins for the LED's
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenred4flash();

    // Print a start-up note to the first serial port
    Serial.println(F("WebSDL Device: EnviroDIY Mayfly"));
    Serial.print(F("Now running "));
    Serial.println(SKETCH_NAME);
    Serial.print(F("Current Mayfly RTC time is: "));
    Serial.println(getDateTime_ISO8601());

    // Count the number of sensors
    sensorCount = sizeof(SENSOR_LIST) / sizeof(SENSOR_LIST[0]);
    Serial.print(F("There are "));
    Serial.print(String(sensorCount));
    Serial.println(F(" variables being recorded"));

    // Set up all the sensors
    setupSensors();

    Serial.println(F("Setup finished!"));
    Serial.println(F("------------------------------------------\n"));
}


// -----------------------------------------------
// Main loop function
// -----------------------------------------------
void loop()
{
    // Print a line to show new reading
    Serial.println(F("------------------------------------------"));
    // Power the sensors;
    digitalWrite(switchedPower, HIGH);
    // One second warm-up time
    // delay(1000);
    // Turn on the LED to show we're taking a reading
    digitalWrite(GREEN_LED, HIGH);
    // Get the sensor value(s), store as string
    updateAllSensors();
    // Print the data to the screen
    Serial.println(generateSensorDataCSV());
    // Turn off the LED to show we're done with the reading
    digitalWrite(GREEN_LED, LOW);
    // Cut Power to the sensors;
    digitalWrite(switchedPower, LOW);
    // Print a to close it off
    Serial.println(F("------------------------------------------\n"));

    // Wait for the next reading
    delay(10000);
}
