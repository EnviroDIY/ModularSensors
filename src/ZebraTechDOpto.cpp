/*
 *ZebraTechDOpto.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the ZebraTech D-Opto digital dissolved oxygen sensor
 *This sensor communicates via SDI-12
 *
*/

#define LIBCALL_ENABLEINTERRUPT  // To prevent compiler/linker crashes
#include <EnableInterrupt.h>  // To handle external and pin change interrupts

#include "ZebraTechDOpto.h"


// The constructor - need the number of measurements the sensor will return, SDI-12 address, the power pin, and the data pin
ZebraTechDOpto::ZebraTechDOpto(char SDI12address, int powerPin, int dataPin, int measurementsToAverage)
    : Sensor(F("ZebraTech D-Opto"), DOPTO_NUM_VARIABLES,
             DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS, DOPTO_MEASUREMENT_TIME_MS,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = SDI12address;
}
ZebraTechDOpto::ZebraTechDOpto(char *SDI12address, int powerPin, int dataPin, int measurementsToAverage)
    : Sensor(F("ZebraTech D-Opto"), DOPTO_NUM_VARIABLES,
             DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS, DOPTO_MEASUREMENT_TIME_MS,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = *SDI12address;
}
ZebraTechDOpto::ZebraTechDOpto(int SDI12address, int powerPin, int dataPin, int measurementsToAverage)
    : Sensor(F("ZebraTech D-Opto"), DOPTO_NUM_VARIABLES,
             DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS, DOPTO_MEASUREMENT_TIME_MS,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = SDI12address + '0';
}


SENSOR_STATUS ZebraTechDOpto::setup(void)
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


SENSOR_STATUS ZebraTechDOpto::getStatus(void)
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
bool ZebraTechDOpto::getSensorInfo(void)
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
String ZebraTechDOpto::getSensorVendor(void)
{return _sensorVendor;}

// The sensor model
String ZebraTechDOpto::getSensorModel(void)
{return _sensorModel;}

// The sensor version
String ZebraTechDOpto::getSensorVersion(void)
{return _sensorVersion;}

// The sensor serial number
String ZebraTechDOpto::getSensorSerialNumber(void)
{return _sensorSerialNumber;}


// The sensor installation location on the Mayfly
String ZebraTechDOpto::getSensorLocation(void)
{
    String sensorLocation = F("SDI12-");
    sensorLocation += String(_SDI12address) + F("_Pin") + String(_dataPin);
    return sensorLocation;
}


// Sending the command to get a concurrent measurement
bool ZebraTechDOpto::startSingleMeasurement(void)
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

    return true;
}


bool ZebraTechDOpto::addSingleMeasurementResult(void)
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
    for (int i = 0; i < DOPTO_NUM_VARIABLES; i++)
    {
        float result = _SDI12Internal.parseFloat();
        sensorValues[i] += result;
        MS_DBG(F("Result #"), i, F(": "), result, F("\n"));
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
