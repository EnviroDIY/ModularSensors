#include <Arduino.h>
#include <SensorBase.h>
#include <DecagonES2.h>

// Decagon ES2: pin settings
// sdi-12 data pin is usually, pin 7 on shield 3.0
const char *ES2SDI12address1 = "1";  // The SDI-12 Address of the ES2
const char *ES2SDI12address2 = "2";  // The SDI-12 Address of the ES2
const char *ES2SDI12address3 = "3";  // The SDI-12 Address of the ES2
const int ES2Data = 7;  // The pin the ES2 is attached to
const int switchedPower = 22;  // sensor power is pin 22 on Mayfly

DecagonES2 ES2_1(1, switchedPower, ES2Data);
// DecagonES2 ES2_2(2, switchedPower, ES2Data);
// DecagonES2 ES2_3(3, switchedPower, ES2Data);


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
}


// -----------------------------------------------
// Main setup function
// -----------------------------------------------
void setup()
{
    // Start the primary serial connection
    Serial.begin(9600);

    // Set up pins for the LED's
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenred4flash();

    // Print a start-up note to the first serial port
    Serial.println(F("WebSDL Device: EnviroDIY Mayfly"));
}

// -----------------------------------------------
// Main loop function
// -----------------------------------------------
void loop()
{
    // Print a line to show new reading
    Serial.println(F("------------------------------------------"));
    // Turn on the LED
    digitalWrite(GREEN_LED, HIGH);
    // Power the sensors;
    digitalWrite(switchedPower, HIGH);

    Serial.print("ES2_1.Cond->getValue() ");
    Serial.println(ES2_1.Cond->getValue());
    delay(random(5000));
    Serial.print("ES2_1.Temp->getValue() ");
    Serial.println(ES2_1.Temp->getValue());
    // delay(random(5000));
    // Serial.print("ES2_2.Cond->getValue() ");
    // Serial.println(ES2_2.Cond->getValue());
    // Serial.print("ES2_3.Cond->getValue() ");
    // Serial.println(ES2_3.Cond->getValue());

    // Cut Power to the sensors;
    digitalWrite(switchedPower, LOW);
    // Turn off the LED
    digitalWrite(GREEN_LED, LOW);
    // Print a to close it off
    Serial.println(F("------------------------------------------\n"));

    // Wait for the next reading
    delay(random(10000));
}
