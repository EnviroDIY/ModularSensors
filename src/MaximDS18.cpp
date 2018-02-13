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
 *
 * Time to take reading at 12-bit: 750ms
 * Reset time is < 480 µs
 */

#include "MaximDS18.h"


// The constructor - if the hex address is known - also need the power pin and the data pin
MaximDS18::MaximDS18(DeviceAddress OneWireAddress, int powerPin, int dataPin, int measurementsToAverage)
  : Sensor(F("MaximDS18"), DS18_NUM_VARIABLES,
           DS18_WARM_UP_TIME_MS, DS18_STABILIZATION_TIME_MS, DS18_MEASUREMENT_TIME_MS,
           powerPin, dataPin, measurementsToAverage),
    oneWire(dataPin), tempSensors(&oneWire)
{
    for (int i = 0; i < 8; i++) _OneWireAddress[i] = OneWireAddress[i];
    // _OneWireAddress = OneWireAddress;
    _addressKnown = true;
}
// The constructor - if the hex address is NOT known - only need the power pin and the data pin
// Can only use this if there is only a single sensor on the pin
MaximDS18::MaximDS18(int powerPin, int dataPin, int measurementsToAverage)
  : Sensor(F("MaximDS18"), DS18_NUM_VARIABLES,
           DS18_WARM_UP_TIME_MS, DS18_STABILIZATION_TIME_MS, DS18_MEASUREMENT_TIME_MS,
           powerPin, dataPin, measurementsToAverage),
    oneWire(dataPin), tempSensors(&oneWire)
{
    _addressKnown = false;
}


// Turns the address into a printable string
String MaximDS18::makeAddressString(DeviceAddress owAddr)
{
    String addrStr = F("Pin");
    addrStr += (_dataPin);
    addrStr += (F("{"));
    for (uint8_t i = 0; i < 8; i++)
    {
        addrStr += ("0x");
        if (owAddr[i] < 0x10) addrStr += ("0");
        addrStr += String(owAddr[i], HEX);
        if (i < 7) addrStr += (",");
    }
    addrStr += (F("}"));

    return addrStr;
}


// This gets the place the sensor is installed ON THE MAYFLY (ie, pin number)
String MaximDS18::getSensorLocation(void)
{
    return makeAddressString(_OneWireAddress);
}


// The function to set up connection to a sensor.
SENSOR_STATUS MaximDS18::getStatus(void)
{
    bool success = true;

    // Make sure the address is valid
    if (!tempSensors.validAddress(_OneWireAddress))
    {
        MS_DBG(F("This sensor address is not valid: "));
        MS_DBG(makeAddressString(_OneWireAddress), F("\n"));
        success = false;
    }

    // Make sure the sensor is connected
    if (!tempSensors.isConnected(_OneWireAddress))
    {
        MS_DBG(F("This sensor is not currently connected: "));
        MS_DBG(makeAddressString(_OneWireAddress), F("\n"));
        success = false;
    }

    if (success) return SENSOR_READY;
    else return SENSOR_ERROR;
}


// The function to set up connection to a sensor.
// By default, sets pin modes and returns ready
SENSOR_STATUS MaximDS18::setup(void)
{
    Sensor::setup();
    tempSensors.begin();

    // Find the address if it's not known
    if (!_addressKnown)
    {
        MS_DBG(F("Probe address is not known!\n"));

        DeviceAddress address;
        if (oneWire.search(address))
        {
            MS_DBG(F("Sensor found at "), makeAddressString(address), F("\n"));
            for (int i = 0; i < 8; i++) _OneWireAddress[i] = address[i];
            _addressKnown = true;  // Now we know the address
        }
        else
        {
            MS_DBG(F("Unable to find address for DS18 on pin "), _dataPin, F("\n"));
        }
    }

    SENSOR_STATUS stat = getStatus();

    // Set resolution to 12 bit
    // All variable resolution sensors start up at 12 bit resolution by default
    if (!tempSensors.setResolution(_OneWireAddress, 12))
    {
        MS_DBG(F("Unable to set the resolution of this sensor: "));
        MS_DBG(makeAddressString(_OneWireAddress), F("\n"));
        return SENSOR_ERROR;
    }

    // Tell the sensor that we do NOT want to wait for conversions to finish
    // That is, we're in ASYNC mode and will get values when we're ready
    tempSensors.setWaitForConversion(false);

    return stat;
}


// Sending the device a request to start temp conversion.
// Because we put ourselves in ASYNC mode in setup, we don't have to wait for finish
bool MaximDS18::startSingleMeasurement(void)
{
    waitForWarmUp();
    waitForStability();
    // Send the command to get temperatures
    MS_DBG(F("Asking sensor to take a measurement\n"));
    tempSensors.requestTemperaturesByAddress(_OneWireAddress);
    _lastMeasurementRequested = millis();
    return true;
}


bool MaximDS18::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a reading to finish
    waitForMeasurementCompletion();

    bool goodTemp = true;
    float result = -9999;

    MS_DBG(F("Requesting temperature result\n"));
    result = tempSensors.getTempC(_OneWireAddress);
    MS_DBG(F("Received "), result, F("°C\n"));

    // If a DS18 cannot get a good measurement, it returns 85
    // If the sensor is not properly connected, it returns -127
    if (result == 85 || result == -127)
    {
        goodTemp = false;
        result = -9999;
    }

    MS_DBG(F("Temperature: "), result, F(" °C\n"));
    verifyAndAddMeasurementResult(DS18_TEMP_VAR_NUM, result);

    return goodTemp;
}
