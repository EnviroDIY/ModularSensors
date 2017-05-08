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
#define MODULAR_SENSORS_OUTPUT Serial  // Without this there will be no output
#include <Arduino.h>
#include <Sodaq_DS3231.h>    // Controls the DS3231 Real Time Clock (RTC) built into the EnviroDIY Mayfly.
#include <VariableArray.h>

// ---------------------------------------------------------------------------
// Set up the sensor specific information
//   ie, pin locations, addresses, calibrations and related settings
// ---------------------------------------------------------------------------
// The name of this file
const char *SKETCH_NAME = "mayfly_turbidity_test2.ino";

// Your logger's timezone.
const int TIME_ZONE = -5;
// Create a new sensor array instance
VariableArray sensors;

// ==========================================================================
//    CAMPBELL OBS 3 / OBS 3+
// ==========================================================================
#include <CampbellOBS3.h>
// Campbell OBS 3+ Low Range calibration in Volts
const int OBSLowPin = 0;  // The low voltage analog pin
const float OBSLow_A = 2.7323;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 93.43;  // The "B" value (X) from the low range calibration
const float OBSLow_C = -0.11966;  // The "C" value from the low range calibration
const int OBS3Power = 22;  // switched sensor power is pin 22 on Mayfly
CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C);
CampbellOBS3_Turbidity turblow(&osb3low, "TurbLow");
// Campbell OBS 3+ High Range calibration in Volts
const int OBSHighPin = 1;  // The high voltage analog pin
const float OBSHigh_A = 20.042;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 389.08;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = -1.2897;  // The "C" value from the high range calibration
CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C);
CampbellOBS3_Turbidity turbhigh(&osb3high, "TurbHigh");

// ---------------------------------------------------------------------------
// Board setup info
// ---------------------------------------------------------------------------
const long SERIAL_BAUD = 57600;  // Serial port BAUD rate
const int GREEN_LED = 8;  // Pin for the green LED
const int RED_LED = 9;  // Pin for the red LED

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

    // Set up the sensor and variables
    osb3low.setup();
    osb3high.setup();
    turblow.setup();
    turbhigh.setup();
}


// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    Serial.println("---------------------------------");

    // Power the sensor
    osb3low.wake();
    osb3high.wake();

    // Turn on the LED to show we're taking a reading
    digitalWrite(GREEN_LED, HIGH);

    // Update the sensor value
    osb3low.update();
    osb3high.update();

    // Turn of sensor power
    osb3low.sleep();
    osb3high.sleep();

    // Turn off the LED to show we're done with the reading
    digitalWrite(GREEN_LED, LOW);

    if (turbhigh.getValue() < 255){
        Serial.print("Diff: ");
        Serial.println(turbhigh.getValue() - turblow.getValue());
    }

    // Wait for the next reading
    delay(1000);
}
