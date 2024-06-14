/** =========================================================================
 * @file powerOn.ino
 * @brief Testing sketch that simply turns on power to the sensors on the
 * Mayfly.
 * ======================================================================= */

#include <Arduino.h>

int8_t powerPin = 22;

void setup() {
    pinMode(powerPin, OUTPUT);
    digitalWrite(powerPin, HIGH);
    pinMode(A5, OUTPUT);
    digitalWrite(A5, HIGH);
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
}

void loop() {}
