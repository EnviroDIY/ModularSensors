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
// 1. Include all sensors and necessary files here
// -----------------------------------------------
#include <avr/sleep.h>
#include <SPI.h>
#include <SdFat.h>
#include <RTCTimer.h>
#include <Sodaq_DS3231.h>
#include <Sodaq_PcInt_PCINT0.h>
#include <GPRSbee.h>
#include "Config.h"

// The timer functions for the RTC
RTCTimer timer;

// Variables for the timer function
long currentepochtime = 0;
char currentTime[26] = "";

// The SD initialization
SdFat SD;
String fileName = String(FILE_NAME);  // For the file name

// For the number of sensors
int sensorCount = 0;
// -----------------------------------------------
// 8. Working functions
// -----------------------------------------------

// Used only for debugging - can be removed
int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

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

// This sets up the sensors.. most have no method defined.
bool setupSensors()
{
    bool success = true;
    for (int i = 0; i < sensorCount; i++)
    {
        success &= SENSOR_LIST[i]->setup();
    }

    return success;
}

// Initializes the SDcard and prints a header to it
void setupLogFile()
{
  // Initialise the SD card
  if (!SD.begin(SD_SS_PIN))
  {
    Serial.println(F("Error: SD card failed to initialise or is missing."));
    //Hang
    //  while (true);
  }

  fileName += String(LoggerID) + F("_") + getDateTime_ISO8601().substring(0,10) + F(".txt");
  // Check if the file already exists
  bool oldFile = SD.exists(fileName.c_str());

  // Open the file in write mode
  File logFile = SD.open(fileName, FILE_WRITE);

  // Add header information if the file did not already exist
  if (!oldFile)
  {
    logFile.println(LoggerID);
    logFile.print(F("Sampling Feature UUID: "));
    logFile.println(SAMPLING_FEATURE);

    String dataHeader = F("\"Timestamp\", ");
    for (int i = 0; i < sensorCount; i++)
    {
        dataHeader += "\"" + String(SENSOR_LIST[i]->getSensorName());
        dataHeader += " " + String(SENSOR_LIST[i]->getVarName());
        dataHeader += " " + String(SENSOR_LIST[i]->getVarUnit());
        dataHeader += " (" + String(UUIDs[i]) + ")\"";
        if (i + 1 != sensorCount)
        {
            dataHeader += F(", ");
        }
    }

    // Serial.println(dataHeader);
    logFile.println(dataHeader);
  }

  //Close the file to save it
  logFile.close();
}

// Flashess to Mayfly's LED's
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
        delay(250);  // A short delay before next sensor;
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

// Writes a string to a text file on the SD Card
void logData(String rec)
{
  // Re-open the file
  File logFile = SD.open(fileName, FILE_WRITE);

  // Write the CSV data
  logFile.println(rec);

  // Close the file to save it
  logFile.close();
}


// Main setup function
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

    // Count the number of sensors
    sensorCount = sizeof(SENSOR_LIST) / sizeof(SENSOR_LIST[0]);

    // Set up all the sensors
    setupSensors();

    // Set up the log file
    setupLogFile();

    // Print a start-up note to the first serial port
    Serial.println(F("WebSDL Device: EnviroDIY Mayfly"));
    Serial.print(F("Now running "));
    Serial.println(SKETCH_NAME);
    Serial.print(F("Free RAM: "));
    Serial.println(freeRam());
    Serial.print(F("Current Mayfly RTC time is: "));
    Serial.println(getDateTime_ISO8601());
    Serial.print(F("There are "));
    Serial.print(String(sensorCount));
    Serial.println(F(" variables being recorded"));
}

void loop()
{
    // Turn on the LED
    // digitalWrite(GREEN_LED, HIGH);
    // Print a line to show new reading
    Serial.println(F("------------------------------------------\n"));
    Serial.print(F("Free RAM: "));
    Serial.println(freeRam());

    // Turn on the LED
    digitalWrite(GREEN_LED, HIGH);
    // Power the sensors;
    digitalWrite(switchedPower, HIGH);
    // Get the sensor value(s), store as string
    updateAllSensors();
    Serial.print(F("Free RAM: "));
    Serial.println(freeRam());
    // Print the data to the screen
    Serial.println(generateSensorDataCSV());
    //Save the data record to the log file
    logData(generateSensorDataCSV());
    // Cut Power to the sensors;
    digitalWrite(switchedPower, LOW);
    // Turn off the LED
    digitalWrite(GREEN_LED, LOW);
    Serial.print(F("Free RAM: "));
    Serial.println(freeRam());

    // Wait for the next reading
    delay(5000);
}
