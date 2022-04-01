/** =========================================================================
 * @file single_sensor.ino
 * @brief An example using only sensor functions and no logging.
 *
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @copyright (c) 2017-2022 Stroud Water Research Center (SWRC)
 *                          and the EnviroDIY Development Team
 *            This example is published under the BSD-3 license.
 *
 * Build Environment: Visual Studios Code with PlatformIO
 * Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
 *
 * DISCLAIMER:
 * THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
 * ======================================================================= */

// ==========================================================================
// Include the base required libraries
// ==========================================================================
/** Start [includes] */
// The Arduino library is needed for every Arduino program.
#include <Arduino.h>

// EnableInterrupt is used by ModularSensors for external and pin change
// interrupts and must be explicitely included in the main program.
#include <EnableInterrupt.h>

// Include the main header for ModularSensors
#include <ModularSensors.h>
/** End [includes] */

// ==========================================================================
// Board setup info
// ==========================================================================
/** Start [sketch_info] */
// The name of this program file
const char* sketchName = "single_sensor.ino";

const int32_t serialBaud = 115200;  // Baud rate for debugging
const int8_t  greenLED   = 8;       // Pin for the green LED
const int8_t  redLED     = 9;       // Pin for the red LED
/** End [sketch_info] */


// ==========================================================================
// Set up the sensor object
// ==========================================================================
/** Start [sensor] */
#include <sensors/MaxBotixSonar.h>

// Create a reference to the serial port for the sonar
HardwareSerial& sonarSerial = Serial1;  // Use hardware serial if possible

const int8_t SonarPower   = 22;  // excite (power) pin
const int    SonarTrigger = -1;  // Trigger pin

// Create a new instance of the sonar sensor;
MaxBotixSonar sonar(sonarSerial, SonarPower, SonarTrigger);

// Create a new instance of the range variable;
MaxBotixSonar_Range sonar_range(&sonar);
/** End [sensor] */

/* Start [calculated variables] */
// Create a function to calculate the water depth from the sonar range
// For this example, we'll assume that the sonar is mounted 5m above the stream
// bottom
float calcDepth(void) {
    float mountHeight = 5000;
    float sonarRange  = sonar_range.getValue();
    return mountHeight - sonarRange;
}
// Create a calculated variable for the water depth
// Variable calcVar(functionName, VariableName, VariableUnit, Resolution, UUID,
// Code); VariableName must be a value from
// http://vocabulary.odm2.org/variablename/ VariableUnit must be a value from
// http://vocabulary.odm2.org/units/
Variable waterDepth(calcDepth, 0, "waterDepth", "millimeter", "sonarDepth",
                    "12345678-abcd-1234-ef00-1234567890ab");
/** End [calculated_variables] */


// ==========================================================================
//  Working Functions
// ==========================================================================
/** Start [working_functions] */
// Flashes to Mayfly's LED's
void greenredflash(int numFlash = 4) {
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
/** End [working_functions] */


// ==========================================================================
//  Arduino Setup Function
// ==========================================================================
/** Start [setup] */
void setup() {
    // Start the primary serial connection
    Serial.begin(serialBaud);

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.println(sketchName);

    Serial.print(F("Using ModularSensors Library version "));
    Serial.println(MODULAR_SENSORS_VERSION);

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
/** End [setup] */


// ==========================================================================
//  Arduino Loop Function
// ==========================================================================
/** Start [loop] */
void loop() {
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
/** End [loop] */
