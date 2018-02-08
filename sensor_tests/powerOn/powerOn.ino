#include <Arduino.h>

int powerPin = 22;

void setup(){
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
}

void loop() {}
