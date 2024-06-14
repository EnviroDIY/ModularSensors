/** =========================================================================
 * @file interrupt_counter.ino
 * @brief Testing sketch counting pin change interrupts.
 * ======================================================================= */

#include <Arduino.h>

#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>

const uint8_t firstInterruptPin = 2;
const uint8_t lastInterruptPin  = 31;
int           interrupCounts[lastInterruptPin - firstInterruptPin + 1];

char buffer[12];
void pinInterrupt() {
    interrupCounts[arduinoInterruptedPin - firstInterruptPin] =
        interrupCounts[arduinoInterruptedPin - firstInterruptPin] + 1;
    Serial.print(arduinoInterruptedPin);
    Serial.print("-->");
    Serial.println(arduinoPinState);
}


void setup() {
    Serial.begin(115200);
    Serial.print("Time,    ");
    for (int i = 0; i < lastInterruptPin - firstInterruptPin; i++) {
        sprintf(buffer, "%3d", i + firstInterruptPin);
        Serial.print(buffer);
        Serial.print(",");
    }
    sprintf(buffer, "%3d", lastInterruptPin);
    Serial.println(buffer);
    for (int i = 0; i <= lastInterruptPin - firstInterruptPin; i++) {
        pinMode(i + firstInterruptPin, INPUT);
        enableInterrupt(i + firstInterruptPin, pinInterrupt, CHANGE);
        interrupCounts[i] = 0;
    }
}

void loop() {
    sprintf(buffer, "%8u", millis());
    Serial.print(buffer);
    Serial.print(",");
    for (int i = 0; i < lastInterruptPin - firstInterruptPin; i++) {
        sprintf(buffer, "%3d", interrupCounts[i]);
        Serial.print(buffer);
        Serial.print(",");
    }
    sprintf(buffer, "%3d",
            interrupCounts[lastInterruptPin - firstInterruptPin]);
    Serial.println(buffer);
    Serial.print("Pin 21 is ");
    Serial.println(digitalRead(21));
    delay(1000);
}
