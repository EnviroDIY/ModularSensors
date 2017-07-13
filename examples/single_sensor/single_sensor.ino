/*****************************************************************************
single_sensor.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of getting data from a single sensor, in this case, a
MaxBotix Ultrasonic Range Finder

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
//    Maxbotix HRXL
// ==========================================================================
#include <MaxBotixSonar.h>

// Define a serial port for receiving data - in this case, using software serial
// Because the standard software serial library uses interrupts that conflict
// with several other libraries used within this program, we must use a
// version of software serial that has been stripped of interrupts and define
// the interrrupts for it using the enableInterrup library.

// If enough hardware serial ports are available on your processor, you should
// use one of those instead.  If the proper pins are avaialbe, AltSoftSerial
// by Paul Stoffregen is also superior to SoftwareSerial for this sensor.
// Neither hardware serial nor AltSoftSerial require any modifications to
// deal with interrupt conflicts.

const int SonarData = 11;     // data  pin
const int SonarTrigger = -1;   // Trigger pin
const int SonarPower = 22;   // excite (power) pin

#if defined __AVR__
#include <SoftwareSerial_ExtInts.h>  // for the stream communication
SoftwareSerial_ExtInts sonarSerial(SonarData, -1);  // No Tx pin is required, only Rx

// Create a new instance of the sonar sensor;
MaxBotixSonar sonar(SonarPower, sonarSerial, SonarTrigger) ;

#else
// Create a new instance of the sonar sensor;
HardwareSerial &sonarSerial = Serial;
MaxBotixSonar sonar(SonarPower, sonarSerial, SonarTrigger) ;
#endif

// Create a new instance of the range variable;
MaxBotixSonar_Range sonar_range(&sonar);

// ---------------------------------------------------------------------------
// Board setup info
// ---------------------------------------------------------------------------
const long SERIAL_BAUD = 57600;  // Serial port baud rate
const int GREEN_LED = 8;  // Pin for the green LED
const int RED_LED = 9;  // Pin for the red LED

// Flashes to Mayfly's LED's
void greenredflash(int numFlash = 4)
{
  for (int i = 0; i < numFlash; i++) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    delay(75);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(75);
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
    // Start the stream for the sonar
    sonarSerial.begin(9600);
    // Allow interrupts for software serial
    #if defined SoftwareSerial_ExtInts_h
    enableInterrupt(SonarData, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
    #endif

    // Set up pins for the LED's
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Print a start-up note to the first serial port
    Serial.println(F("Single Sensor Example - Sonar Ranging"));

    // Set up the sensor and variables
    sonar.setup();
    sonar_range.setup();
}


// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    // Turn on the LED to show we're taking a reading
    digitalWrite(GREEN_LED, HIGH);

    // Wake up the sensor (also gives power)
    sonar.wake();

    // Update the sensor value
    sonar.update();

    // Print the sonar result
    Serial.print("Current sonar range: ");
    Serial.println(sonar_range.getValueString());

    // Put the sensor back to sleep (also cuts power)
    sonar.sleep();

    // Turn off the LED to show we're done with the reading
    digitalWrite(GREEN_LED, LOW);

    // Wait for the next reading
    delay(5000);
}
