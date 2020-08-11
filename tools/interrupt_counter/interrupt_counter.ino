#include <Arduino.h>
#include <EnableInterrupt.h>

int8_t   interruptPin     = 10;
uint32_t numberInterrupts = 0;


void pinInterrupt() {
    numberInterrupts++;
}


void setup() {
    Serial.begin(115200);
    pinMode(interruptPin, INPUT_PULLUP);
    enableInterrupt(interruptPin, pinInterrupt, CHANGE);
}

void loop() {
    Serial.print("By ");
    Serial.print(millis());
    Serial.print(" total changes on pin ");
    Serial.print(interruptPin);
    Serial.print(" is ");
    Serial.println(numberInterrupts);
    delay(1000);
}
