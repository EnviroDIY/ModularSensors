/*****************************************************************************
single_sensor.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.23.15

This sketch is an example of getting data from a single sensor, in this case, a
MaxBotix Ultrasonic Range Finder
DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ==========================================================================
//    Defines for the Arduino IDE
//    In PlatformIO, set these build flags in your platformio.ini
// ==========================================================================

// ==========================================================================
// Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <SensorBase.h>
#include <VariableBase.h>

// The library version this example was written for
const char *libraryVersion = "0.23.15";
// The name of this file
const char *sketchName = "single_sensor.ino";

// ==========================================================================
// Board setup info
// ==========================================================================

const long serialBaud = 115200;  // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;  // Pin for the green LED
const int8_t redLED = 9;  // Pin for the red LED

// ==========================================================================
// Set up the sensor object
// ==========================================================================
#include <sensors/MaxBotixSonar.h>

// Create a reference to the serial port for the sonar
HardwareSerial &sonarSerial = Serial1;  // Use hardware serial if possible

const int8_t SonarPower = 22;   // excite (power) pin
const int SonarTrigger = -1;   // Trigger pin

// Create a new instance of the sonar sensor;
MaxBotixSonar sonar(sonarSerial, SonarPower, SonarTrigger);

// Create a new instance of the range variable;
MaxBotixSonar_Range sonar_range(&sonar);

// Create a function to calculate the water depth from the sonar range
// For this example, we'll assume that the sonar is mounted 5m above the stream bottom
float calcDepth(void)
{
    float mountHeight = 5000;
    float sonarRange = sonar_range.getValue();
    return mountHeight - sonarRange;
}
// Create a calculated variable for the water depth
// Variable calcVar(functionName, VariableName, VariableUnit, Resolution, UUID, Code);
// VariableName must be a value from http://vocabulary.odm2.org/variablename/
// VariableUnit must be a value from http://vocabulary.odm2.org/units/
Variable waterDepth(calcDepth, 0,
                    "waterDepth", "millimeter",
                    "sonarDepth", "12345678-abcd-1234-ef00-1234567890ab");



// ==========================================================================
//    Working Functions
// ==========================================================================

// Flashes to Mayfly's LED's
void greenredflash(int numFlash = 4)
{
  for (int i = 0; i < numFlash; i++) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    delay(75);
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    delay(75);
  }
  digitalWrite(redLED, LOW);
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

    // Start the stream for the sonar
    sonarSerial.begin(9600);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Print a start-up note to the first serial port
    Serial.println(F("Single Sensor Example - Sonar Ranging"));

    // Set up the sensor
    sonar.setup();
}


// ==========================================================================
// Main loop function
// ==========================================================================
void loop()
{
    // Turn on the LED to show we're taking a reading
    digitalWrite(greenLED, HIGH);

    // Send power to the sensor
    sonar.powerUp();

    // Wake up the sensor
    sonar.wake();

    // Update the sensor value
    sonar.update();

    // Print the sonar result
    Serial.print("Current sonar range: ");
    Serial.println(sonar_range.getValueString());
    Serial.print("Calculated water depth: ");
    Serial.println(waterDepth.getValueString());

    // Put the sensor back to sleep
    sonar.sleep();

    // Cut the sensor power
    sonar.powerDown();

    // Turn off the LED to show we're done with the reading
    digitalWrite(greenLED, LOW);

    // Wait for the next reading
    delay(5000);
}
