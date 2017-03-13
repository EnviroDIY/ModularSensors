/*****************************************************************************
multisensor_print.ino
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
#include <Sodaq_DS3231.h>    // Controls the DS3231 Real Time Clock (RTC) built into the EnviroDIY Mayfly.

#include <SensorBase.h>

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
const char *SKETCH_NAME = "modular_sensors.ino";

// Mayfly version number
const char *MFVersion = "v0.3";

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
// Board setup info
// ---------------------------------------------------------------------------
const int SERIAL_BAUD = 9600;  // Serial port BAUD rate
const int GREEN_LED = 8;  // Pin for the green LED
const int RED_LED = 9;  // Pin for the red LED

// Variables for the timer function
long currentepochtime = 0;
char currentTime[26] = "";


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

void printSensorData(void)
{
    Serial.print(F("Updated all sensors at "));
    Serial.println(currentTime);
    for (int i = 0; i < sensorCount; i++)
    {
        Serial.print(SENSOR_LIST[i]->getSensorName());
        Serial.print(F(" attached at "));
        Serial.print(SENSOR_LIST[i]->getSensorLocation());
        Serial.print(F(" reports "));
        Serial.print(SENSOR_LIST[i]->getVarName());
        Serial.print(F(" is "));
        Serial.print(SENSOR_LIST[i]->getValue());
        Serial.print(F(" "));
        Serial.print(SENSOR_LIST[i]->getVarUnit());
        Serial.println();
    }
}


// ---------------------------------------------------------------------------
// Main setup function
// ---------------------------------------------------------------------------
void setup()
{
    // Start the primary serial connection
    Serial.begin(SERIAL_BAUD);

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
    Serial.print(F("There are "));
    Serial.print(String(sensorCount));
    Serial.println(F(" variables being recorded"));

    // Set up all the sensors
    setupSensors();

    Serial.println(F("Setup finished!"));
    Serial.println(F("------------------------------------------\n"));
}


// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    // Print a line to show new reading
    Serial.println(F("------------------------------------------"));
    // Power the sensors;
    digitalWrite(switchedPower, HIGH);
    // One second warm-up time
    delay(1000);
    // Turn on the LED to show we're taking a reading
    digitalWrite(GREEN_LED, HIGH);
    // Get the sensor value(s), store as string
    updateAllSensors();
    // Print the data to the screen
    printSensorData();
    // Turn off the LED to show we're done with the reading
    digitalWrite(GREEN_LED, LOW);
    // Cut Power to the sensors;
    digitalWrite(switchedPower, LOW);
    // Print a to close it off
    Serial.println(F("------------------------------------------\n"));

    // Wait for the next reading
    delay(10000);
}
