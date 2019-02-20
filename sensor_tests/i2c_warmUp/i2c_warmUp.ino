#include <Arduino.h>
#include <Wire.h>

byte address = 0x66;


void setup()
{
    Wire.begin();

    Serial.begin(115200);
    while (!Serial);
    Serial.println("\nI2C Warm Up Timing Test");

    pinMode(22, OUTPUT);
}


void loop()
{
    uint32_t start=millis();
    digitalWrite(22, HIGH);
    byte error;

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
        Serial.print("I2C device replied at address 0x");
        if (address<16)
        Serial.print("0");
        Serial.print(address,HEX);
        Serial.print(" after ");
        Serial.print(start-millis());
        Serial.println(" ms!");
    }
    else if (error==4)
    {
        Serial.print("Unknown error at address 0x");
        if (address<16)
        Serial.print("0");
        Serial.println(address,HEX);
    }
}
