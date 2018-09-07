/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// #define BME_SCK 13
// #define BME_MISO 12
// #define BME_MOSI 11
// #define BME_CS 10

uint8_t BMEi2c_addr = 0x76;  // The BME280 can be addressed either as 0x76 or 0x77
const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)



#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

void setup() {
    Serial.begin(57600);
    Serial.println(F("BME280 test"));

    // Turn on switched power
    pinMode(I2CPower, OUTPUT);
    digitalWrite(I2CPower, HIGH);

    bool status;

    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin(BMEi2c_addr);
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }

    Serial.println("-- Timing Test --");
    delayTime = 1100;

    Serial.println();

    // Print table headers
    Serial.println("  Time,  Temp, Humid,   Press,   Alt");
    Serial.println("    ms,    *C,     %,      Pa,     m");

}


void loop() {

    for (int i=0; i <= 30; i++)
    {
        Serial.print("  ");
        Serial.print(millis());
        Serial.print(", ");
        Serial.print(bme.readTemperature());
        Serial.print(", ");
        Serial.print(bme.readHumidity());
        Serial.print(", ");
        Serial.print(bme.readPressure());
        Serial.print(", ");
        Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
        Serial.println();
        delay(delayTime);
    }

    delay(100000);
}
