 /*
 * MaximDS18.cpp
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 * This file is for the Maxim DS18B20 Temperature sensor.
 * It is dependent on Dallas Temperature library, which itself is dependent on
 * the OneWire library.
 *
 * The resolution of the temperature sensor is user-configurable to
 * 9, 10, 11, or 12 bits, corresponding to increments of
 * 0.5°C, 0.25°C, 0.125°C, and 0.0625°C, respectively.
 * The default resolution at power-up is 12-bit.
 */

#include <Arduino.h>
#include "MaximDS18.h"


// The constructor - if the hex address is known - also need the power pin and the data pin
MaximDS18_Temp::MaximDS18_Temp(DeviceAddress OneWireAddress, int powerPin, int dataPin)
  : SensorBase(dataPin, powerPin, F("MaximDS18"), F("temperature"), F("degreeCelsius"), DS18_TEMP_RESOLUTION, F("DS18Temp"))
{
    _OneWireAddress = OneWireAddress;
    _addressKnown = true;
}
// The constructor - if the hex address is NOT known - only need the power pin and the data pin
// Can only use this if there is only a single sensor on the pin
MaximDS18_Temp::MaximDS18_Temp(int powerPin, int dataPin)
  : SensorBase(dataPin, powerPin, F("MaximDS18"), F("temperature"), F("degreeCelsius"), DS18_TEMP_RESOLUTION, F("DS18Temp"))
{
    _OneWireAddress = {0};
    _addressKnown = false;
}


// Uses TLL Communication to get data from MaxBotix
bool MaximDS18_Temp::update(){

    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}  // powerUp function includes a 500ms delay

    // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
    OneWire oneWire(_dataPin);

    // Pass our oneWire reference to Dallas Temperature.
    DallasTemperature sensors(&oneWire);

    // Start up the sensor library
    sensors.begin();

    // Find the address if it's not known
    if (!_addressKnown){
        oneWire.reset_search();  // Reset the search index
        uint8_t address[8];
        if (!oneWire.search(address)) {
            Serial.print(F("Unable to find address for DS18 on pin "));  // For debugging
            Serial.println(_dataPin);  // For debugging
        }
        else {
            Serial.print(F("Sensor found on pin "));  // For debugging
            Serial.print(_dataPin);  // For debugging
            Serial.print(F(" with address {"));  // For debugging
            for (uint8_t i = 0; i < 8; i++)  // For debugging
            {  // For debugging
                Serial.print("0x");  // For debugging
                if (address[i] < 0x10) Serial.print("0");  // For debugging
                Serial.print(address[i], HEX);  // For debugging
                if (i < 7) Serial.print(", ");  // For debugging
            }  // For debugging
            Serial.println(F("}"));  // For debugging
            for (int i = 0; i < 8; i++) _OneWireAddress[i] = address[i];
            _addressKnown = true;  // Now we know the address
        }
    }

    // Send the command to get temperatures
    sensors.requestTemperatures();

    result = sensors.getTempC(_OneWireAddress);

    // Serial.println(result);  // For debugging
    sensorValue_temp = result;
    // Serial.println(sensorValue_temp);  // For debugging

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Return true when finished
    sensorLastUpdated = millis();
    return true;
}


float MaximDS18_Temp::getValue(void)
{
    checkForUpdate(sensorLastUpdated);
    return sensorValue_temp;
}
