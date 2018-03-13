/*
 *SDI12Sensors.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for all Decagon Devices that communicate with SDI-12
 *It is dependent on the EnviroDIY SDI-12 library.
*/

#define LIBCALL_ENABLEINTERRUPT  // To prevent compiler/linker crashes
#include <EnableInterrupt.h>  // To handle external and pin change interrupts

#include "SDI12Sensors.h"


// The constructor - need the number of measurements the sensor will return, SDI-12 address, the power pin, and the data pin
SDI12Sensors::SDI12Sensors(char SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage,
                           String sensorName, uint8_t numReturnedVars,
                           uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t measurementTime_ms)
    : Sensor(sensorName, numReturnedVars,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = SDI12address;
}
SDI12Sensors::SDI12Sensors(char *SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage,
                           String sensorName, uint8_t numReturnedVars,
                           uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t measurementTime_ms)
    : Sensor(sensorName, numReturnedVars,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = *SDI12address;
}
SDI12Sensors::SDI12Sensors(int SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage,
                           String sensorName, uint8_t numReturnedVars,
                           uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t measurementTime_ms)
    : Sensor(sensorName, numReturnedVars,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = SDI12address + '0';
}


SENSOR_STATUS SDI12Sensors::setup(void)
{
    SENSOR_STATUS retVal = Sensor::setup();

    // Begin the SDI-12 interface
    _SDI12Internal.begin();

     // SDI-12 protocol says sensors must respond within 15 milliseconds
    _SDI12Internal.setTimeout(150);
    // Force the timeout value to be -9999
    _SDI12Internal.setTimeoutValue(-9999);

    // Allow the SDI-12 library access to interrupts
    enableInterrupt(_dataPin, SDI12::handleInterrupt, CHANGE);

    bool isSet = getSensorInfo();

    if (isSet and retVal == SENSOR_READY) return SENSOR_READY;
    else return SENSOR_ERROR;
}


SENSOR_STATUS SDI12Sensors::getStatus(void)
{
    waitForWarmUp();

    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(F("   Asking for sensor acknowlegement\n"));
    String myCommand = "";
    myCommand += (char) _SDI12address;
    myCommand += "!"; // sends 'acknowledge active' command [address][!]
    _SDI12Internal.sendCommand(myCommand);
    MS_DBG(F("      >>> "), myCommand, F("\n"));
    delay(30);

    // wait for acknowlegement with format:
    // [address]<CR><LF>
    String sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    MS_DBG(F("      <<< "), sdiResponse, F("\n"));

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    if (sdiResponse == String(_SDI12address))
    {
        MS_DBG(F("   "), getSensorName(), F(" replied as expected.\n"));
        return SENSOR_READY;
    }
    else
    {
        MS_DBG(F("   "), getSensorName(), F(" did not reply!\n"));
        return SENSOR_ERROR;
    }
}


// A helper function to run the "sensor info" SDI12 command
bool SDI12Sensors::getSensorInfo(void)
{
    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}

    // Check that the sensor is there and responding
    // The getStatus() function includes the waitForWarmUp()
    SENSOR_STATUS stat = SENSOR_ERROR;
    uint8_t ntries = 0;
    while (stat == SENSOR_ERROR && ntries < 4)
    {
        stat = getStatus();
        ntries++;
    }
    if (stat == SENSOR_ERROR) return false;

    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(F("   Getting sensor info\n"));
    String myCommand = "";
    myCommand += (char) _SDI12address;
    myCommand += "I!"; // sends 'info' command [address][I][!]
    _SDI12Internal.sendCommand(myCommand);
    MS_DBG(F("      >>> "), myCommand, F("\n"));
    delay(30);

    // wait for acknowlegement with format:
    // [address][SDI12 version supported (2 char)][vendor (8 char)][model (6 char)][version (3 char)][serial number (<14 char)]<CR><LF>
    String sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    MS_DBG(F("      <<< "), sdiResponse, F("\n"));

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    if (sdiResponse.length() > 1)
    {
        String sdi12Address = sdiResponse.substring(0,1);
        MS_DBG(F("   SDI12 Address:"), sdi12Address);
        float sdi12Version = sdiResponse.substring(1,3).toFloat();
        sdi12Version /= 10;
        MS_DBG(F(", SDI12 Version:"), sdi12Version);
        _sensorVendor = sdiResponse.substring(3,11);
        _sensorVendor.trim();
        MS_DBG(F(", Sensor Vendor:"), _sensorVendor);
        _sensorModel = sdiResponse.substring(11,17);
        _sensorModel.trim();
        MS_DBG(F(", Sensor Model:"), _sensorModel);
        _sensorVersion = sdiResponse.substring(17,20);
        _sensorVersion.trim();
        MS_DBG(F(", Sensor Version:"), _sensorVersion);
        _sensorSerialNumber = sdiResponse.substring(20);
        _sensorSerialNumber.trim();
        MS_DBG(F(", Sensor Serial Number:"), _sensorSerialNumber,'\n');
        return true;
    }
    else return false;
}


// The sensor vendor
String SDI12Sensors::getSensorVendor(void)
{return _sensorVendor;}

// The sensor model
String SDI12Sensors::getSensorModel(void)
{return _sensorModel;}

// The sensor version
String SDI12Sensors::getSensorVersion(void)
{return _sensorVersion;}

// The sensor serial number
String SDI12Sensors::getSensorSerialNumber(void)
{return _sensorSerialNumber;}


// The sensor installation location on the Mayfly
String SDI12Sensors::getSensorLocation(void)
{
    String sensorLocation = F("SDI12-");
    sensorLocation += String(_SDI12address) + F("_Pin") + String(_dataPin);
    return sensorLocation;
}


// Sending the command to get a concurrent measurement
bool SDI12Sensors::startSingleMeasurement(void)
{
    // Check that the sensor is there and responding
    // The getStatus() function includes the waitForWarmUp()
    SENSOR_STATUS stat = SENSOR_ERROR;
    uint8_t ntries = 0;
    while (stat == SENSOR_ERROR && ntries < 4)
    {
        stat = getStatus();
        ntries++;
    }
    if (stat == SENSOR_ERROR)
    {
        _millisSensorActivated = 0;
        _millisMeasurementRequested = 0;
        return false;
    }

    // These sensors should be stable at the first reading they are able to return
    // BUT... we'll put this in for safety
    waitForStability();

    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(F("   Beginning concurrent measurement on "), getSensorName(), '\n');
    String startCommand = "";
    startCommand += _SDI12address;
    startCommand += "C!"; // Start concurrent measurement - format  [address]['C'][!]
    _SDI12Internal.sendCommand(startCommand);
    delay(30);  // It just needs this little delay
    MS_DBG(F("      >>> "), startCommand, F("\n"));

    // wait for acknowlegement with format
    // [address][ttt (3 char, seconds)][number of values to be returned, 0-9]<CR><LF>
    String sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    _SDI12Internal.clearBuffer();
    MS_DBG(F("      <<< "), sdiResponse, F("\n"));

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // Verify the number of results the sensor will send
    // int numVariables = sdiResponse.substring(4,5).toInt();
    // if (numVariables != _numReturnedVars)
    // {
    //     MS_DBG(numVariables, F(" results expected\n"));
    //     MS_DBG(F("This differs from the sensor's standard design of "));
    //     MS_DBG(_numReturnedVars, F(" measurements!!\n"));
    // }

    // Set the times we've activated the sensor and asked for a measurement
    if (sdiResponse.length() > 0)
    {
        MS_DBG(F("   Concurrent measurement started.\n"));
        _millisSensorActivated = millis();
        _millisMeasurementRequested = millis();
    }
    else
    {
        MS_DBG(F("   "), getSensorName(), F(" did not respond to measurement request!\n"));
        _millisSensorActivated = 0;
        _millisMeasurementRequested = 0;
    }

    return true;
}


bool SDI12Sensors::addSingleMeasurementResult(void)
{

    if (_millisSensorActivated > 0)
    {
        // Make sure we've waited long enough for a reading to finish
        waitForMeasurementCompletion();

        // Empty the buffer
        _SDI12Internal.clearBuffer();

        MS_DBG(F("   Requesting data from "), getSensorName(), '\n');
        String getDataCommand = "";
        getDataCommand += _SDI12address;
        getDataCommand += "D0!";  // SDI-12 command to get data [address][D][dataOption][!]
        _SDI12Internal.sendCommand(getDataCommand);
        delay(30);  // It just needs this little delay
        MS_DBG(F("      >>> "), getDataCommand, F("\n"));

        MS_DBG(F("   Receiving results from "), getSensorName(), '\n');
        _SDI12Internal.read();  // ignore the repeated SDI12 address
        for (int i = 0; i < _numReturnedVars; i++)
        {
            float result = _SDI12Internal.parseFloat();
            // The SDI-12 library should return -9999 on timeout
            if (result == -9999 or isnan(result)) result = -9999;
            MS_DBG(F("      <<< Result #"), i, F(": "), result, F("\n"));
            verifyAndAddMeasurementResult(i, result);
        }
        // String sdiResponse = _SDI12Internal.readStringUntil('\n');
        // sdiResponse.trim();
        // _SDI12Internal.clearBuffer();
        // MS_DBG(F("      <<< "), sdiResponse, F("\n"));

        // Empty the buffer again
        _SDI12Internal.clearBuffer();

        // Mark that we've already recorded the result of the measurement
        _millisMeasurementRequested = 0;

        // Return true when finished
        return true;
    }
    else
    {
        MS_DBG(F("   "), getSensorName(), F(" is not currently measuring!\n"));
        return false;
    }
}
