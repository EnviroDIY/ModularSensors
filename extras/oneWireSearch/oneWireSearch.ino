/** =========================================================================
 *    @file oneWireSearch.ino
 *  @author Rob Tillaart
 * VERSION: 0.1.02
 * @brief scan for 1-Wire devices + code snippet generator
 *    DATE: 2015-june-30
 *     URL: http://forum.arduino.cc/index.php?topic=333923
 *
 * inspired by
 * http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
 *
 * Released to the public domain
 *
 * 0.1.00 initial version
 * 0.1.01 first published version
 * 0.1.02 small output changes
 * ======================================================================= */

#include <Arduino.h>
#include <OneWire.h>

uint8_t findDevices(int pin) {
    OneWire ow(pin);

    uint8_t address[8];
    uint8_t count = 0;


    if (ow.search(address)) {
        Serial.print("\nuint8_t pin");
        Serial.print(pin, DEC);
        Serial.println("[][8] = {");
        do {
            count++;
            Serial.print("  {");
            for (uint8_t i = 0; i < 8; i++) {
                Serial.print("0x");
                if (address[i] < 0x10) Serial.print("0");
                Serial.print(address[i], HEX);
                if (i < 7) Serial.print(", ");
            }
            Serial.println("},");
        } while (ow.search(address));

        Serial.println("};");
        Serial.print("// nr devices found: ");
        Serial.println(count);
    }

    return count;
}

void setup() {
    Serial.begin(9600);
    Serial.println(
        "//\n// Start oneWireSearch.ino \n// -----------------------");

    // Power the sensors;
    pinMode(22, OUTPUT);
    digitalWrite(22, HIGH);
    delay(2000);

    for (uint8_t pin = 2; pin < 37; pin++) { findDevices(pin); }
    Serial.println("\n//\n// End oneWireSearch.ino \n// ---------------------");

    // Cut power
    digitalWrite(22, LOW);
}

void loop() {}
