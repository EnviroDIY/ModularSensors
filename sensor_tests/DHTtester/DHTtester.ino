// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"

#define DHTPIN 6     // what digital pin we're connected to
#define powerPin 22

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
// #define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// #define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    pinMode(powerPin, OUTPUT);
    digitalWrite(powerPin, HIGH);

    Serial.begin(115200);
    Serial.println("DHTxx test!");

    // Start up the sensor
    dht.begin();
}

void loop() {
    // Turn on sensor power
    digitalWrite(powerPin, HIGH);

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)

    float humid_val, temp_val, hi_val = 0;
    // First read the humidity
    humid_val = dht.readHumidity();
    // Read temperature as Celsius (the default)
    temp_val = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(humid_val) || isnan(temp_val)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Turn off sensor power
    digitalWrite(powerPin, LOW);

    // Compute heat index in Celsius (isFahreheit = false)
    hi_val = dht.computeHeatIndex(temp_val, humid_val, false);

    Serial.print("Humidity: ");
    Serial.print(humid_val);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temp_val);
    Serial.print(" °C\t");
    Serial.print("Heat index: ");
    Serial.print(hi_val);
    Serial.print(" °C\n");

    // Wait a few seconds between measurements.
    delay(10000);

}
