#include <Arduino.h>
#include <Wire.h>

byte address = 0x66;

uint32_t start;
bool firstE1 = true;
bool firstE2 = true;
bool firstE3 = true;
bool firstE4 = true;


void setup()
{
    Wire.begin();

    Serial.begin(115200);
    while (!Serial);
    Serial.println("I2C Warm Up Timing Test");

    // Make sure we start un-powered
    Serial.print("Wait");
    pinMode(22, OUTPUT);
    digitalWrite(22, LOW);
    for (uint32_t dstart = millis(); millis() - dstart < 5000L;)
    {
        Serial.print(".");
        delay(250);
    }
    Serial.println(".");
    start = millis();
}


void loop()
{
    digitalWrite(22, HIGH);
    int i2cStatus;

    Wire.beginTransmission(address);
    Wire.write('r');  // Write "R" to start a reading
    i2cStatus = Wire.endTransmission();

    switch (i2cStatus)
    {
        case 0:
            Serial.print("I2C device replied at address 0x");
            if (address<16)
            Serial.print("0");
            Serial.print(address,HEX);
            Serial.print(" after ");
            Serial.print(millis() - start);
            Serial.println(" ms!");
            while (true) {};  // Stop
        case 1:
            if (firstE1)
            {
                Serial.print("I2C device replied at address 0x");
                if (address<16)
                Serial.print("0");
                Serial.print(address,HEX);
                Serial.print(" after ");
                Serial.print(millis() - start);
                Serial.println(" ms, but data is too long for transmit buffer.");
                firstE1 = false;
            }
            break;
        case 2:
            if (firstE2)
            {
                Serial.print("I2C device replied at address 0x");
                if (address<16)
                Serial.print("0");
                Serial.print(address,HEX);
                Serial.print(" after ");
                Serial.print(millis() - start);
                Serial.println(" ms, but received NACK on transmit of address");
                firstE2 = false;
            }
            break;
        case 3:
            if (firstE3)
            {
                Serial.print("I2C device replied at address 0x");
                if (address<16)
                Serial.print("0");
                Serial.print(address,HEX);
                Serial.print(" after ");
                Serial.print(millis() - start);
                Serial.println(" ms, but received NACK on transmit of data");
                firstE3 = false;
            }
            break;
        case 4:
        default:
            if (firstE4)
            {
                Serial.print("I2C device replied at address 0x");
                if (address<16)
                Serial.print("0");
                Serial.print(address,HEX);
                Serial.print(" after ");
                Serial.print(millis() - start);
                Serial.println(" ms, but unknown error occurred");
                firstE4 = false;
            }
            break;
    }
}
