/*****************************************************************************
single_sensor_SQ212.ino
Written By:  Anthony Aufdenkampe <aaufdenkampe@limno.com>
Adapted from single_sensor_SQ212.ino by Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of getting data from a single sensor, in this case, a
Apogee SQ-212 Quantum Light sensor

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ---------------------------------------------------------------------------
// Include the base required libraries
// ---------------------------------------------------------------------------
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <SensorBase.h>
#include <VariableBase.h>

// ---------------------------------------------------------------------------
// Set up the sensor object
// ---------------------------------------------------------------------------

// ==========================================================================
//    Apogee SQ-212 Quantum Light sensor
// ==========================================================================
#include <ApogeeSQ212.h>
// MaxBotix Sonar: pin settings
const int SQ212Power = 22;   // excite (power) pin
const int SQ212Data = 0;     // data  pin
// Create a new instance of the sonar sensor;
ApogeeSQ212 SQ212(SQ212Power, SQ212Data);
// Create a new instance of the range variable;
ApogeeSQ212_PAR SQ212_PAR(&SQ212);

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

    // Print a start-up note to the first serial port
    Serial.println(F("Single Sensor Example - Apogee SQ-212 PAR"));

    // Set up the sensor and variables
    SQ212.setup();
    SQ212_PAR.setup();
}


// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    // Turn on the LED to show we're taking a reading
    digitalWrite(GREEN_LED, HIGH);

    // Wake up the sensor (also gives power)
    SQ212.wake();

    // Update the sensor value
    SQ212.update();

    // Print the sonar result
    Serial.print("Current PAR: ");
    Serial.println(SQ212_PAR.getValueString());

    // Put the sensor back to sleep (also cuts power)
    SQ212.sleep();

    // Turn off the LED to show we're done with the reading
    digitalWrite(GREEN_LED, LOW);

    // Wait for the next reading
    delay(5000);
}
