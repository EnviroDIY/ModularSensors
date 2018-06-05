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
MaximDS18::MaximDS18(DeviceAddress OneWireAddress, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage)
  : Sensor("MaximDS18", DS18_NUM_VARIABLES,
           DS18_WARM_UP_TIME_MS, DS18_STABILIZATION_TIME_MS, DS18_MEASUREMENT_TIME_MS,
           powerPin, dataPin, measurementsToAverage),
    _internalOneWire(dataPin), _internalDallasTemp(&_internalOneWire)
{
    for (int i = 0; i < 8; i++) _OneWireAddress[i] = OneWireAddress[i];
    // _OneWireAddress = OneWireAddress;
    _addressKnown = true;
}
// The constructor - if the hex address is NOT known - only need the power pin and the data pin
// Can only use this if there is only a single sensor on the pin
MaximDS18::MaximDS18(int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage)
  : Sensor("MaximDS18", DS18_NUM_VARIABLES,
           DS18_WARM_UP_TIME_MS, DS18_STABILIZATION_TIME_MS, DS18_MEASUREMENT_TIME_MS,
           powerPin, dataPin, measurementsToAverage),
    _internalOneWire(dataPin), _internalDallasTemp(&_internalOneWire)
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
// By default, sets pin modes and returns ready
bool MaximDS18::setup(void)
{
    uint8_t ntries = 0;

    bool retVal = Sensor::setup();  // this will set timestamp and status bit
    _internalDallasTemp.begin();

    // Find the address if it's not known
    if (!_addressKnown)
    {
        MS_DBG(F("Probe address is not known!\n"));

        DeviceAddress address;  // create a variable to put the found address into
        ntries = 0;
        bool gotAddress = false;
        // Try 5 times to get an address
        while (!gotAddress and ntries <5)
        {
            gotAddress = _internalOneWire.search(address);
            ntries++;
        }
        if (gotAddress)
        {
            MS_DBG(F("Sensor found at "), makeAddressString(address), F("\n"));
            for (int i = 0; i < 8; i++) _OneWireAddress[i] = address[i];
            _addressKnown = true;  // Now we know the address
        }
        else
        {
            MS_DBG(F("Unable to find address for DS18 on pin "), _dataPin, F("\n"));
            // set the status error bit! (bit 7)
            _sensorStatus |= 0b10000000;
            return false;
        }
    }
    // If the address is known, make sure the given address is valid
    else
    {
        if (!_internalDallasTemp.validAddress(_OneWireAddress))
        {
            MS_DBG(F("This sensor address is not valid: "));
            MS_DBG(makeAddressString(_OneWireAddress), F("\n"));
            // set the status error bit! (bit 7)
            _sensorStatus |= 0b10000000;
            return false;
        }

        // And then make 5 attempts to connect to the sensor
        ntries = 0;
        bool madeConnection = false;
        while (!madeConnection and ntries <5)
        {
            madeConnection = _internalDallasTemp.isConnected(_OneWireAddress);
            ntries++;
        }
        if (!madeConnection)
        {
            MS_DBG(F("This sensor is not currently connected: "));
            MS_DBG(makeAddressString(_OneWireAddress), F("\n"));
            // set the status error bit! (bit 7)
            _sensorStatus |= 0b10000000;
            return false;
        }
    }

    // Set resolution to 12 bit
    // All variable resolution sensors start up at 12 bit resolution by default
    if (!_internalDallasTemp.setResolution(_OneWireAddress, 12))
    {
        MS_DBG(F("Unable to set the resolution of this sensor: "));
        MS_DBG(makeAddressString(_OneWireAddress), F("\n"));
        // We're not setting the error bit if this fails because not all sensors
        // have variable resolution.
    }

    // Tell the sensor that we do NOT want to wait for conversions to finish
    // That is, we're in ASYNC mode and will get values when we're ready
    _internalDallasTemp.setWaitForConversion(false);

    return retVal;
}


// Sending the device a request to start temp conversion.
// Because we put ourselves in ASYNC mode in setup, we don't have to wait for finish
bool MaximDS18::startSingleMeasurement(void)
{
    bool success = true;

    // Check if activated, only go on if it is
    if (_millisSensorActivated > 0 && bitRead(_sensorStatus, 3))
    {
        // Send the command to get temperatures
        MS_DBG(F("Asking DS18 to take a measurement\n"));
        success = _internalDallasTemp.requestTemperaturesByAddress(_OneWireAddress);

        // Mark the time that a measurement was requested
        _millisMeasurementRequested = millis();
    }
    // Make sure that the time of a measurement request is not set
    else _millisMeasurementRequested = 0;

    // Even if we failed to start a measurement, we still want to set the status
    // bit to show that we attempted to start the measurement.
    // Set the status bits for measurement requested (bit 5)
    _sensorStatus |= 0b00100000;
    // Verify that the status bit for a single measurement completion is not set (bit 6)
    _sensorStatus &= 0b10111111;
    return success;
}


bool MaximDS18::addSingleMeasurementResult(void)
{
    bool success = false;

    // Initialize float variable
    float result = -9999;

    if (_millisMeasurementRequested > 0)
    {
        MS_DBG(F("Requesting temperature result\n"));
        result = _internalDallasTemp.getTempC(_OneWireAddress);
        MS_DBG(F("Received "), result, F("°C\n"));

        // If a DS18 cannot get a good measurement, it returns 85
        // If the sensor is not properly connected, it returns -127
        if (result == 85 || result == -127) result = -9999;
        else success = true;
        MS_DBG(F("Temperature: "), result, F(" °C\n"));
    }
    else MS_DBG(F("Sensor is not currently measuring!\n"));

    // Put value into the array
    verifyAndAddMeasurementResult(DS18_TEMP_VAR_NUM, result);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    return success;
}
