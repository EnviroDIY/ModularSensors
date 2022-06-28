/**
 * @file MaximDS18.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the MaximDS18 class.
 */

#include "MaximDS18.h"


// The constructor - if the hex address is known - also need the power pin and
// the data pin
MaximDS18::MaximDS18(DeviceAddress OneWireAddress, int8_t powerPin,
                     int8_t dataPin, uint8_t measurementsToAverage)
    : Sensor("MaximDS18", DS18_NUM_VARIABLES, DS18_WARM_UP_TIME_MS,
             DS18_STABILIZATION_TIME_MS, DS18_MEASUREMENT_TIME_MS, powerPin,
             dataPin, measurementsToAverage, DS18_INC_CALC_VARIABLES),
      _addressKnown(true),
      _internalOneWire(dataPin),
      _internalDallasTemp(&_internalOneWire) {
    for (uint8_t i = 0; i < 8; i++) _OneWireAddress[i] = OneWireAddress[i];
}
// The constructor - if the hex address is NOT known - only need the power pin
// and the data pin Can only use this if there is only a single sensor on the
// pin
MaximDS18::MaximDS18(int8_t powerPin, int8_t dataPin,
                     uint8_t measurementsToAverage)
    : Sensor("MaximDS18", DS18_NUM_VARIABLES, DS18_WARM_UP_TIME_MS,
             DS18_STABILIZATION_TIME_MS, DS18_MEASUREMENT_TIME_MS, powerPin,
             dataPin, measurementsToAverage, DS18_INC_CALC_VARIABLES),
      _addressKnown(false),
      _internalOneWire(dataPin),
      _internalDallasTemp(&_internalOneWire) {}
// Destructor
MaximDS18::~MaximDS18() {}


// Turns the address into a printable string
String MaximDS18::makeAddressString(DeviceAddress owAddr) {
    String addrStr = F("Pin");
    addrStr += (_dataPin);
    addrStr += '{';
    for (uint8_t i = 0; i < 8; i++) {
        addrStr += "0x";
        if (owAddr[i] < 0x10) addrStr += "0";
        addrStr += String(owAddr[i], HEX);
        if (i < 7) addrStr += ",";
    }
    addrStr += '}';

    return addrStr;
}


// This gets the place the sensor is installed ON THE MAYFLY (ie, pin number)
String MaximDS18::getSensorLocation(void) {
    return makeAddressString(_OneWireAddress);
}


// The function to set up connection to a sensor.
// By default, sets pin modes and returns ready
bool MaximDS18::setup(void) {
    uint8_t ntries = 0;

    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // Need to power up for setup
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    _internalDallasTemp.begin();

    // Find the address if it's not known
    if (!_addressKnown) {
        MS_DBG(F("Address of DS18 on pin"), _dataPin, F("is not known!"));

        DeviceAddress
            address;  // create a variable to put the found address into
        ntries          = 0;
        bool gotAddress = false;
        // Try 5 times to get an address
        while (!gotAddress && ntries < 5) {
            gotAddress = _internalOneWire.search(address);
            ntries++;
        }
        if (gotAddress) {
            MS_DBG(F("Sensor found at"), makeAddressString(address));
            for (uint8_t i = 0; i < 8; i++) _OneWireAddress[i] = address[i];
            _addressKnown = true;  // Now we know the address
        } else {
            MS_DBG(F("Unable to find address for DS18 on pin"), _dataPin);
            retVal = false;
        }
    } else {  // If the address is known, make sure the given address is valid
        if (!_internalDallasTemp.validAddress(_OneWireAddress)) {
            MS_DBG(F("This sensor address is not valid:"),
                   makeAddressString(_OneWireAddress));
            retVal = false;
        }

        // And then make 5 attempts to connect to the sensor
        ntries              = 0;
        bool madeConnection = false;
        while (retVal && !madeConnection && ntries < 5) {
            madeConnection = _internalDallasTemp.isConnected(_OneWireAddress);
            ntries++;
        }
        if (!madeConnection) {
            MS_DBG(F("This sensor is not currently connected:"),
                   makeAddressString(_OneWireAddress));
            retVal = false;
        }
    }

    // Set resolution to 12 bit
    // All variable resolution sensors start up at 12 bit resolution by default
    if (!_internalDallasTemp.setResolution(_OneWireAddress, 12)) {
        MS_DBG(F("Unable to set the resolution of this sensor:"),
               makeAddressString(_OneWireAddress));
        // We're not setting the error bit if this fails because not all sensors
        // have variable resolution.
    }

    // Tell the sensor that we do NOT want to wait for conversions to finish
    // That is, we're in ASYNC mode and will get values when we're ready
    _internalDallasTemp.setWaitForConversion(false);

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    if (!retVal) {  // if set-up failed
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
        // UN-set the set-up bit (bit 0) since setup failed!
        _sensorStatus &= 0b11111110;
    }

    return retVal;
}


// Sending the device a request to start temp conversion.
// Because we put ourselves in ASYNC mode in setup, we don't have to wait for
// finish
bool MaximDS18::startSingleMeasurement(void) {
    // Sensor::startSingleMeasurement() checks that if it's awake/active and
    // sets the timestamp and status bits.  If it returns false, there's no
    // reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    // Send the command to get temperatures
    MS_DBG(F("Asking DS18 to take a measurement"));
    bool success =
        _internalDallasTemp.requestTemperaturesByAddress(_OneWireAddress);

    if (success) {
        // Update the time that a measurement was requested
        _millisMeasurementRequested = millis();
    } else {
        // Otherwise, make sure that the measurement start time and success bit
        // (bit 6) are unset
        MS_DBG(getSensorNameAndLocation(),
               F("did not successfully start a measurement."));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
    }

    return success;
}


bool MaximDS18::addSingleMeasurementResult(void) {
    bool success = false;

    // Initialize float variable
    float result = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
        result = _internalDallasTemp.getTempC(_OneWireAddress);
        MS_DBG(F("  Received"), result, F("°C"));

        // If a DS18 cannot get a good measurement, it returns 85
        // If the sensor is not properly connected, it returns -127
        if (result == 85 || result == -127) {
            result = -9999;
        } else {
            success = true;
        }
        MS_DBG(F("  Temperature:"), result, F("°C"));
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    // Put value into the array
    verifyAndAddMeasurementResult(DS18_TEMP_VAR_NUM, result);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}
