#include <Arduino.h>

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    pinMode(23, OUTPUT);
    digitalWrite(23, LOW);
    delay(1000);
    Serial1.print("+++");
    delay(1000);
    Serial1.print("ATRE\r");
    Serial1.print("ATWR\r");
    Serial1.print("ATAC\r");
    Serial1.print("ATGT\r");
    Serial1.print("ATFR\r");
    Serial1.print("ATCN\r");
}

void loop()
{
    while (Serial.available())
    {
        Serial1.write(Serial.read());
    }
    while (Serial1.available())
    {
        Serial.write(Serial1.read());
    }
}
