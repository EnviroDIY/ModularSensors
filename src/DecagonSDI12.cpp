/*
 *DecagonSDI12.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for all Decagon Devices that communicate with SDI-12
 *It is dependent on the EnviroDIY SDI-12 library.
*/

#define LIBCALL_ENABLEINTERRUPT  // To prevent compiler/linker crashes
#include <EnableInterrupt.h>  // To handle external and pin change interrupts

#include "DecagonSDI12.h"


// The constructor - need the number of measurements the sensor will return, SDI-12 address, the power pin, and the data pin
DecagonSDI12::DecagonSDI12(char SDI12address, int powerPin, int dataPin, int measurementsToAverage,
                           String sensorName, int numReturnedVars,
                           uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t remeasurementTime_ms)
    : Sensor(sensorName, numReturnedVars,
             warmUpTime_ms, stabilizationTime_ms, remeasurementTime_ms,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = SDI12address;
}
DecagonSDI12::DecagonSDI12(char *SDI12address, int powerPin, int dataPin, int measurementsToAverage,
                           String sensorName, int numReturnedVars,
                           uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t remeasurementTime_ms)
    : Sensor(sensorName, numReturnedVars,
             warmUpTime_ms, stabilizationTime_ms, remeasurementTime_ms,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = *SDI12address;
}
DecagonSDI12::DecagonSDI12(int SDI12address, int powerPin, int dataPin, int measurementsToAverage,
                           String sensorName, int numReturnedVars,
                           uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t remeasurementTime_ms)
    : Sensor(sensorName, numReturnedVars,
             warmUpTime_ms, stabilizationTime_ms, remeasurementTime_ms,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = SDI12address + '0';
}


SENSOR_STATUS DecagonSDI12::setup(void)
{
    Sensor::setup();

    // Begin the SDI-12 interface
    _SDI12Internal.begin();

     // SDI-12 protocol says sensors must respond within 15 milliseconds
    _SDI12Internal.setTimeout(15);

    // Allow the SDI-12 library access to interrupts
    enableInterrupt(_dataPin, SDI12::handleInterrupt, CHANGE);

    waitForWarmUp();
    bool isSet = getSensorInfo();

    if (isSet) return SENSOR_READY;
    else return SENSOR_ERROR;
}


SENSOR_STATUS DecagonSDI12::getStatus(void)
{
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(F("Asking for sensor acknowlegement\n"));
    String myCommand = "";
    myCommand += (char) _SDI12address;
    myCommand += "!"; // sends 'acknowledge active' command [address][!]
    _SDI12Internal.sendCommand(myCommand);
    MS_DBG(F(">>"), myCommand, F("\n"));
    delay(30);

    // wait for acknowlegement with format:
    // [address]<CR><LF>
    String sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    MS_DBG(F("<<"), sdiResponse, F("\n"));

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    if (sdiResponse == String(_SDI12address)) return SENSOR_READY;
    else return SENSOR_ERROR;
}


// A helper function to run the "sensor info" SDI12 command
bool DecagonSDI12::getSensorInfo(void)
{
    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    // Wait until the sensor is warmed up
    waitForWarmUp();

    // Check that the sensor is there and responding
    if (getStatus() == SENSOR_ERROR) return false;

    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(F("Getting sensor info\n"));
    String myCommand = "";
    myCommand += (char) _SDI12address;
    myCommand += "I!"; // sends 'info' command [address][I][!]
    _SDI12Internal.sendCommand(myCommand);
    MS_DBG(F(">>"), myCommand, F("\n"));
    delay(30);

    // wait for acknowlegement with format:
    // [address][SDI12 version supported (2 char)][vendor (8 char)][model (6 char)][version (3 char)][serial number (<14 char)]<CR><LF>
    String sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    MS_DBG(F("<<"), sdiResponse, F("\n"));

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    if (sdiResponse.length() > 1)
    {
        // _sensorName = sdiResponse.substring(3,17);
        // _sensorName.trim();
        _sensorVendor = sdiResponse.substring(3,11);
        _sensorVendor.trim();
        _sensorModel = sdiResponse.substring(11,17);
        _sensorModel.trim();
        _sensorVersion = sdiResponse.substring(17,20);
        _sensorVersion.trim();
        _sensorSerialNumber = sdiResponse.substring(20,17);
        _sensorSerialNumber.trim();
        return true;
    }
    else return false;
}


// The sensor vendor
String DecagonSDI12::getSensorVendor(void)
{return _sensorVendor;}

// The sensor model
String DecagonSDI12::getSensorModel(void)
{return _sensorModel;}

// The sensor version
String DecagonSDI12::getSensorVersion(void)
{return _sensorVersion;}

// The sensor serial number
String DecagonSDI12::getSensorSerialNumber(void)
{return _sensorSerialNumber;}


// The sensor installation location on the Mayfly
String DecagonSDI12::getSensorLocation(void)
{
    String sensorLocation = F("SDI12-");
    sensorLocation += String(_SDI12address) + F("_Pin") + String(_dataPin);
    return sensorLocation;
}


// Sending the command to get a concurrent measurement
bool DecagonSDI12::startSingleMeasurement(void)
{
    // Check that the sensor is there and responding
    if (getStatus() == SENSOR_ERROR) return false;

    waitForStability();

    // Empty the buffer
    _SDI12Internal.clearBuffer();

    String startCommand = "";
    startCommand += _SDI12address;
    startCommand += "C!"; // Start concurrant measurement - format  [address]['C'][!]
    _SDI12Internal.sendCommand(startCommand);
    MS_DBG(F(">>"), startCommand, F("\n"));
    delay(30);  // It just needs this little delay

    // wait for acknowlegement with format
    // [address][ttt (3 char, seconds)][number of values to be returned, 0-9]<CR><LF>
    String sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    _SDI12Internal.clearBuffer();
    MS_DBG(F("<<"), sdiResponse, F("\n"));

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // // Verify the number of results the sensor will send
    // int numVariables = sdiResponse.substring(4,5).toInt();
    // if (numVariables != _numReturnedVars)
    // {
    //     MS_DBG(numVariables, F(" results expected\n"));
    //     MS_DBG(F("This differs from the sensor's standard design of "));
    //     MS_DBG(_numReturnedVars, F(" measurements!!\n"));
    // }

    return true;
}


bool DecagonSDI12::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a reading to finish
    waitForMeasurementCompletion();

    // Empty the buffer
    _SDI12Internal.clearBuffer();

    String getDataCommand = "";
    getDataCommand += _SDI12address;
    getDataCommand += "D0!";  // SDI-12 command to get data [address][D][dataOption][!]
    _SDI12Internal.sendCommand(getDataCommand);
    MS_DBG(F(">>"), getDataCommand, F("\n"));
    delay(30);  // It just needs this little delay

    MS_DBG(F("Receiving data\n"));
    _SDI12Internal.read();  // ignore the repeated SDI12 address
    for (int i = 0; i < _numReturnedVars; i++)
    {
        float result = _SDI12Internal.parseFloat();
        // If the result becomes garbled or the probe is disconnected, the parseFloat function returns 0.
        if (result == 0) result = -9999;
        MS_DBG(F("Result #"), i, F(": "), result, F("\n"));
        verifyAndAddMeasurementResult(i, result);
    }
    // String sdiResponse = _SDI12Internal.readStringUntil('\n');
    // sdiResponse.trim();
    // _SDI12Internal.clearBuffer();
    // MS_DBG(F("<<"), sdiResponse, F("\n"));

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // Return true when finished
    return true;
}
