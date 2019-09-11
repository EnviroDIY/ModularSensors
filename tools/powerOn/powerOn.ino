#include <Arduino.h>

int8_t powerPin = 22;

void setup(){
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
    pinMode(A5, OUTPUT);
    digitalWrite(A5, HIGH);
}

void loop() {}
