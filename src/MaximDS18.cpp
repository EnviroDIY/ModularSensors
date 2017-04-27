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

#include "MaximDS18.h"


// The constructor - if the hex address is known - also need the power pin and the data pin
MaximDS18::MaximDS18(DeviceAddress OneWireAddress, int powerPin, int dataPin)
  : Sensor(powerPin, dataPin, F("MaximDS18"), DS18_NUM_MEASUREMENTS),
    oneWire(dataPin), tempSensors(&oneWire)
{
    _OneWireAddress = OneWireAddress;
    _addressKnown = true;
}
// The constructor - if the hex address is NOT known - only need the power pin and the data pin
// Can only use this if there is only a single sensor on the pin
MaximDS18::MaximDS18(int powerPin, int dataPin)
  : Sensor(powerPin, dataPin, F("MaximDS18"), DS18_NUM_MEASUREMENTS),
    oneWire(dataPin), tempSensors(&oneWire)
{
    _OneWireAddress = {0};
    _addressKnown = false;
}

// Turns the address into a printable string
String MaximDS18::getAddressString(DeviceAddress OneWireAddress)
{
    String addrStr = F("Pin");
    addrStr += (_dataPin);
    addrStr += (F("{"));
    for (uint8_t i = 0; i < 8; i++)
    {
        addrStr += ("0x");
        if (OneWireAddress[i] < 0x10) addrStr += ("0");
        addrStr += String(OneWireAddress[i], HEX);
        if (i < 7) addrStr += (",");
    }
    addrStr += (F("}"));

    return addrStr;
}

// The function to set up connection to a sensor.
// By default, sets pin modes and returns ready
SENSOR_STATUS MaximDS18::getStatus(void)
{
    // Make sure the address is valid
    if (!tempSensors.validAddress(_OneWireAddress))
    {
        Serial.print(F("This sensor address is not valid: "));  // For debugging
        Serial.println(getAddressString(_OneWireAddress));  // For debugging
        return SENSOR_ERROR;
    }

    // Make sure the sensor is connected
    if (!tempSensors.isConnected(_OneWireAddress))
    {
        Serial.print(F("This sensor is not currently connected: "));  // For debugging
        Serial.println(getAddressString(_OneWireAddress));  // For debugging
        return SENSOR_ERROR;
    }

    // Set resolution to 12 bit
    if (!tempSensors.setResolution(_OneWireAddress, 12))
    {
        Serial.print(F("Unable to set the resolution of this sensor: "));  // For debugging
        Serial.println(getAddressString(_OneWireAddress));  // For debugging
        return SENSOR_ERROR;
    }

    return SENSOR_READY;
}

// The function to set up connection to a sensor.
// By default, sets pin modes and returns ready
SENSOR_STATUS MaximDS18::setup(void)
{
    pinMode(_powerPin, OUTPUT);
    pinMode(_dataPin, INPUT);
    digitalWrite(_powerPin, LOW);

    // Start up the maxim sensor library
    Serial.println(F("Starting the Maxim sensors"));
    tempSensors.begin();

    // Find the address if it's not known
    if (!_addressKnown)
    {
        Serial.println(F("Probe address is not known!"));  // For debugging

        uint8_t address[8];
        if (!oneWire.search(address))
        {
            Serial.print(F("Unable to find address for DS18 on pin "));  // For debugging
            Serial.println(_dataPin);  // For debugging
        }
        else
        {
            Serial.print(F("Sensor found at "));  // For debugging
            Serial.println(getAddressString(address));  // For debugging

            for (int i = 0; i < 8; i++) _OneWireAddress[i] = address[i];
            _addressKnown = true;  // Now we know the address
        }
    }

    // Serial.print(F("Set up "));  // for debugging
    // Serial.print(getSensorName());  // for debugging
    // Serial.print(F(" attached at "));  // for debugging
    // Serial.print(getSensorLocation());  // for debugging
    // Serial.print(F(" which can return up to "));  // for debugging
    // Serial.print(_numReturnedVars);  // for debugging
    // Serial.println(F(" variable[s]."));  // for debugging

    return getStatus();
}


// This gets the place the sensor is installed ON THE MAYFLY (ie, pin number)
String MaximDS18::getSensorLocation(void)
{
    return getAddressString(_OneWireAddress);
}


// Uses TLL Communication to get data from MaxBotix
bool MaximDS18::update()
{
    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}  // powerUp function includes a 500ms delay

    // Clear values before starting loop
    clearValues();

    // Send the command to get temperatures
    Serial.println(F("Asking sensor to take a measurement"));  // For debugging
    tempSensors.requestTemperaturesByAddress(_OneWireAddress);


    Serial.println(F("Requesting temperature result"));  // For debugging
    float result = tempSensors.getTempC(_OneWireAddress);

    Serial.print(F("Sending value of "));
    Serial.print(result);
    Serial.println(F(" °C to the sensorValues array"));
    sensorValues[DS18_TEMP_VAR_NUM] = result;
    Serial.println(sensorValues[DS18_TEMP_VAR_NUM]);  // For debugging

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Update the registered variables with the new values
    Serial.println(F("Notifying registered variables."));
    notifyVariables();

    // Return true when finished
    return true;
}
