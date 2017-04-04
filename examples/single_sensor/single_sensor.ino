/*****************************************************************************
single_sensor.ino
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

#include <CampbellOBS3.h>

// ---------------------------------------------------------------------------
// Set up the sensor specific information
//   ie, pin locations, addresses, calibrations and related settings
// ---------------------------------------------------------------------------
// The name of this file
const char *SKETCH_NAME = "modular_sensors.ino";
const int TIME_ZONE = -5;

// Campbell OBS 3+: pin settings
const int OBSPower = 22;    // sensor power is pin 22 on Mayfly
// Campbell OBS 3+ Low Range calibration in Volts
const int OBSLowPin = 0;  // The low voltage analog pin
const float OBSLow_A = -2.4763E-01;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 1.0569E+02;  // The "B" value (X) from the low range calibration
const float OBSLow_C = -2.9928E-1;  // The "C" value from the low range calibration
// Campbell OBS 3+ High Range calibration in Volts
const int OBSHighPin = 1;  // The high voltage analog pin
const float OBSHigh_A = 3.5210E+01;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 4.0111E+02;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = 2.0709E-01;  // The "C" value from the high range calibration

// ---------------------------------------------------------------------------
// 3. The array that contains all valid sensors
// ---------------------------------------------------------------------------
SensorBase *SENSOR_LIST[] = {
    new CampbellOBS3_Turbidity(OBSPower, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C),
    new CampbellOBS3_TurbHigh(OBSPower, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C)
};
int sensorCount = sizeof(SENSOR_LIST) / sizeof(SENSOR_LIST[0]);


// ---------------------------------------------------------------------------
// Board setup info
// ---------------------------------------------------------------------------
const int SERIAL_BAUD = 9600;  // Serial port BAUD rate
const int GREEN_LED = 8;  // Pin for the green LED
const int RED_LED = 9;  // Pin for the red LED

// Variables for the clock function
long currentepochtime = 0;


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

// This function returns the datetime from the realtime clock as a string
String getDateTime(void)
{
  String dateTimeStr;
  //Create a DateTime object from the current time
  DateTime dt(rtc.makeDateTime(getNow()));
  //Convert it to a String
  dt.addToString(dateTimeStr);
  return dateTimeStr;
}

// Create a new logger instance
SensorArray sensors;


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
    Serial.println(getDateTime());

    // Count the number of sensors
    Serial.print(F("There are "));
    Serial.print(String(sensorCount));
    Serial.println(F(" variables being recorded"));

    // Initialize the sensor array;
    sensors.init(sensorCount, SENSOR_LIST);

    // Set up all the sensors
    sensors.setupSensors();

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
    digitalWrite(OBSPower, HIGH);
    // One second warm-up time
    delay(1000);
    // Turn on the LED to show we're taking a reading
    digitalWrite(GREEN_LED, HIGH);
    // Update the sensor value(s)
    sensors.updateAllSensors();
    // Print the data to the screen
    Serial.print(F("Updated all sensors at "));
    Serial.println(getDateTime());
    sensors.printSensorData(&Serial);
    // Turn off the LED to show we're done with the reading
    digitalWrite(GREEN_LED, LOW);
    // Cut Power to the sensors;
    digitalWrite(OBSPower, LOW);
    // Print a to close it off
    Serial.println(F("------------------------------------------\n"));

    // Wait for the next reading
    delay(5000);
}
