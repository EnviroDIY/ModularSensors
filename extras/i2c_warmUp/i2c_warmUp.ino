/** =========================================================================
 * @file i2c_warmUp.ino
 * @brief Testing sketch to see how long an attached I2C device takes to
 * begin to respond to commands.
 * ======================================================================= */

#include <Arduino.h>
#include <Wire.h>

int address = 0x66;

uint32_t start;
bool     firstSuccess = true;
bool     firstE1      = true;
bool     firstE2      = true;
bool     firstE3      = true;
bool     firstE4      = true;

int        i2cStatus   = 4;
const char commands[4] = "iri";
uint8_t    index       = 0;

void printTime() {
    Serial.print("I2C device replied at address 0x");
    if (address < 16) Serial.print("0");
    Serial.print(address, HEX);
    Serial.print(" after ");
    Serial.print(millis() - start);
    Serial.print(" ms, code: ");
    Serial.println(i2cStatus);
}


void setup() {
    Wire.begin();

    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("I2C Warm Up Timing Test");
}


void loop() {
    // Make sure we start un-powered
    Serial.print("Wait");
    pinMode(22, OUTPUT);
    digitalWrite(22, LOW);
    for (uint32_t dstart = millis(); millis() - dstart < 5000L;) {
        Serial.print(".");
        delay(250);
    }
    Serial.println(".");
    start = millis();

    // Serial.print("Attempting to write: ");
    // Serial.println(commands[index]);

    digitalWrite(22, HIGH);

    bool gotResult = false;
    while (!gotResult) {
        Wire.beginTransmission(address);
        Wire.write(commands[index]);
        i2cStatus = Wire.endTransmission();

        switch (i2cStatus) {
            case 0:
                printTime();
                Serial.print(commands[index]);
                Serial.println(" successfully written");
                start = millis();
                while (true) {
                    Wire.requestFrom(address, 40, true);
                    uint8_t code = Wire.read();
                    if (code == 1) {
                        Serial.print("Result available after ");
                        Serial.print(millis() - start);
                        Serial.print(" ms: ");
                        Serial.println(Wire.readStringUntil('\0'));
                        gotResult = true;
                        break;
                    }
                }
                break;
            case 1:
                if (firstE1) {
                    printTime();
                    Serial.println("Data is too long for transmit buffer.");
                    firstE1 = false;
                }
                break;
            case 2:
                if (firstE2) {
                    printTime();
                    Serial.println("Received NACK on transmit of address");
                    firstE2 = false;
                }
                break;
            case 3:
                if (firstE3) {
                    printTime();
                    Serial.println(" Received NACK on transmit of data");
                    firstE3 = false;
                }
                break;
            case 4:
            default:
                if (firstE4) {
                    printTime();
                    Serial.println("Unknown error occurred");
                    firstE4 = false;
                }
                break;
        }
        // Serial.println("resending");
    }

    Serial.print("Moving to next character - ");
    index++;                    // go to next character
    if (index == 3) index = 0;  // reset
}
