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
    // Check if the power is on, turn it on if not (Need power to get status)
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}  // powerUp function includes a 500ms delay

    // Make sure the address is valid
    if (!tempSensors.validAddress(_OneWireAddress))
    {
        DBGM(F("This sensor address is not valid: "));
        DBGM(getAddressString(_OneWireAddress), F("\n"));
        return SENSOR_ERROR;
    }

    // Make sure the sensor is connected
    if (!tempSensors.isConnected(_OneWireAddress))
    {
        DBGM(F("This sensor is not currently connected: "));
        DBGM(getAddressString(_OneWireAddress), F("\n"));
        return SENSOR_ERROR;
    }

    // Set resolution to 12 bit
    if (!tempSensors.setResolution(_OneWireAddress, 12))
    {
        DBGM(F("Unable to set the resolution of this sensor: "));
        DBGM(getAddressString(_OneWireAddress), F("\n"));
        return SENSOR_ERROR;
    }

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    return SENSOR_READY;
}

// The function to set up connection to a sensor.
// By default, sets pin modes and returns ready
SENSOR_STATUS MaximDS18::setup(void)
{
    pinMode(_powerPin, OUTPUT);
    pinMode(_dataPin, INPUT);
    digitalWrite(_powerPin, LOW);

    // Check if the power is on, turn it on if not  (Need power to get address)
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}  // powerUp function includes a 500ms delay

    // Start up the maxim sensor library
    tempSensors.begin();

    // Find the address if it's not known
    if (!_addressKnown)
    {
        DBGM(F("Probe address is not known!\n"));

        uint8_t address[8];
        if (!oneWire.search(address))
        {
            DBGM(F("Unable to find address for DS18 on pin "), _dataPin, F("\n"));
        }
        else
        {
            DBGM(F("Sensor found at "), getAddressString(address), F("\n"));

            for (int i = 0; i < 8; i++) _OneWireAddress[i] = address[i];
            _addressKnown = true;  // Now we know the address
        }
    }

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    DBGM(F("Set up "), getSensorName(), F(" attached at "), getSensorLocation());
    DBGM(F(" which can return up to "), _numReturnedVars, F(" variable[s].\n"));

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

    bool goodTemp = false;
    int rangeAttempts = 0;
    float result = 85;  // This is a "bad" result returned from a DS18 sensor

    DBGM(F("Beginning detection for Temperature\n"));
    while (goodTemp == false && rangeAttempts < 50)
    {
        // Send the command to get temperatures
        DBGM(F("Asking sensor to take a measurement\n"));
        tempSensors.requestTemperaturesByAddress(_OneWireAddress);

        DBGM(F("Requesting temperature result\n"));
        result = tempSensors.getTempC(_OneWireAddress);
        rangeAttempts++;

        // If a DS18 cannot get a goot measurement, it returns 85
        // If the sensor is not properly connected, it returns -127
        if (result == 85 || result == -127)
        {
            DBGM(F("Bad or Suspicious Result, Retry Attempt #"), rangeAttempts, F("\n"));
        }
        else
        {
            DBGM(F("Good result found\n"));
            goodTemp = true;  // Set completion of read to true
        }
    }

    DBGM(F("Sending value of "), result, F(" °C to the sensorValues array\n"));
    sensorValues[DS18_TEMP_VAR_NUM] = result;
    DBGM(sensorValues[DS18_TEMP_VAR_NUM], F("\n"));

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Update the registered variables with the new values
    DBGM(F("Notifying registered variables.\n"));
    notifyVariables();

    // Return true when finished
    return true;
}
