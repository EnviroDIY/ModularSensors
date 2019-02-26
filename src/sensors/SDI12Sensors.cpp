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
                           const char *sensorName, const uint8_t numReturnedVars,
                           uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t measurementTime_ms)
    : Sensor(sensorName, numReturnedVars,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = SDI12address;
}
SDI12Sensors::SDI12Sensors(char *SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage,
                           const char *sensorName, const uint8_t numReturnedVars,
                           uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t measurementTime_ms)
    : Sensor(sensorName, numReturnedVars,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = *SDI12address;
}
SDI12Sensors::SDI12Sensors(int SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage,
                           const char *sensorName, const uint8_t numReturnedVars,
                           uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t measurementTime_ms)
    : Sensor(sensorName, numReturnedVars,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, dataPin, measurementsToAverage),
    _SDI12Internal(dataPin)
{
    _SDI12address = SDI12address + '0';
}
// Destructor
SDI12Sensors::~SDI12Sensors(){}


bool SDI12Sensors::setup(void)
{
    bool retVal = Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    bool wasOn = checkPowerOn();
    if (!wasOn) {powerUp();}
    waitForWarmUp();

    // Begin the SDI-12 interface
    _SDI12Internal.begin();

    // Library default timeout should be 150ms, which is 10 times that specified
    // by the SDI-12 protocol for a sensor response.
    // May want to bump it up even further here.
    _SDI12Internal.setTimeout(150);
    // Force the timeout value to be -9999 (This should be library default.)
    _SDI12Internal.setTimeoutValue(-9999);

    #ifdef __AVR__
    // Allow the SDI-12 library access to interrupts
    MS_DBG(F("Enabling interrupts for SDI12 on pin"), _dataPin);
    enableInterrupt(_dataPin, SDI12::handleInterrupt, CHANGE);
    #endif

    retVal &= getSensorInfo();

    // Empty the SDI-12 buffer
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    _SDI12Internal.end();

    // Turn the power back off it it had been turned on
    if (!wasOn) {powerDown();}

    if (!retVal)  // if set-up failed
    {
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
        // UN-set the set-up bit (bit 0) since setup failed!
        _sensorStatus &= 0b11111110;
    }

    return retVal;
}


bool SDI12Sensors::requestSensorAcknowledgement(void)
{
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(F("  Asking for sensor acknowlegement"));
    String myCommand = "";
    myCommand += (char) _SDI12address;
    myCommand += "!";  // sends 'acknowledge active' command [address][!]

    bool didAcknowledge = false;
    uint8_t ntries = 0;
    while (!didAcknowledge && ntries < 5)
    {

        _SDI12Internal.sendCommand(myCommand);
        MS_DBG(F("    >>>"), myCommand);
        delay(30);

        // wait for acknowlegement with format:
        // [address]<CR><LF>
        String sdiResponse = _SDI12Internal.readStringUntil('\n');
        sdiResponse.trim();
        MS_DBG(F("    <<<"), sdiResponse);

        // Empty the buffer again
        _SDI12Internal.clearBuffer();

        if (sdiResponse == String(_SDI12address))
        {
            MS_DBG(F("   "), getSensorNameAndLocation(), F("replied as expected."));
            didAcknowledge = true;
        }
        else if (sdiResponse.startsWith(String(_SDI12address)))
        {
            MS_DBG(F("   "), getSensorNameAndLocation(), F("replied, unexpectedly"));
            didAcknowledge = true;
        }
        else
        {
            MS_DBG(F("   "), getSensorNameAndLocation(), F("did not reply!"));
            didAcknowledge = false;
        }

        ntries++;
    }

    return didAcknowledge;
}


// A helper function to run the "sensor info" SDI12 command
bool SDI12Sensors::getSensorInfo(void)
{
    // MS_DBG(F("   Activating SDI-12 instance for"), getSensorNameAndLocation());
    // Check if this the currently active SDI-12 Object
    bool wasActive = _SDI12Internal.isActive();
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set correctly.
    // if (wasActive) {MS_DBG(F("   SDI-12 instance for"), getSensorNameAndLocation(),
    //                       F("was already active!"));}
    if (!wasActive) _SDI12Internal.begin();
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    // Check that the sensor is there and responding
    if (!requestSensorAcknowledgement()) return false;

    MS_DBG(F("  Getting sensor info"));
    String myCommand = "";
    myCommand += (char) _SDI12address;
    myCommand += "I!";  // sends 'info' command [address][I][!]
    _SDI12Internal.sendCommand(myCommand);
    MS_DBG(F("    >>>"), myCommand);
    delay(30);

    // wait for acknowlegement with format:
    // [address][SDI12 version supported (2 char)][vendor (8 char)][model (6 char)][version (3 char)][serial number (<14 char)]<CR><LF>
    String sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    MS_DBG(F("    <<<"), sdiResponse);

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    if (!wasActive) _SDI12Internal.end();

    if (sdiResponse.length() > 1)
    {
        String sdi12Address = sdiResponse.substring(0,1);
        MS_DBG(F("  SDI12 Address:"), sdi12Address);
        float sdi12Version = sdiResponse.substring(1,3).toFloat();
        sdi12Version /= 10;
        MS_DBG(F("  SDI12 Version:"), sdi12Version);
        _sensorVendor = sdiResponse.substring(3,11);
        _sensorVendor.trim();
        MS_DBG(F("  Sensor Vendor:"), _sensorVendor);
        _sensorModel = sdiResponse.substring(11,17);
        _sensorModel.trim();
        MS_DBG(F("  Sensor Model:"), _sensorModel);
        _sensorVersion = sdiResponse.substring(17,20);
        _sensorVersion.trim();
        MS_DBG(F("  Sensor Version:"), _sensorVersion);
        _sensorSerialNumber = sdiResponse.substring(20);
        _sensorSerialNumber.trim();
        MS_DBG(F("  Sensor Serial Number:"), _sensorSerialNumber);
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
    // Sensor::startSingleMeasurement() checks that if it's awake/active and sets
    // the timestamp and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    String startCommand;
    String sdiResponse;
    bool wasActive;

    // MS_DBG(F("   Activating SDI-12 instance for"), getSensorNameAndLocation());
    // Check if this the currently active SDI-12 Object
    wasActive = _SDI12Internal.isActive();
    // if (wasActive) {MS_DBG(F("   SDI-12 instance for"), getSensorNameAndLocation(),
    //                       F("was already active!"));}
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set correctly.
    if (!wasActive) _SDI12Internal.begin();
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    // Check that the sensor is there and responding
    if (!requestSensorAcknowledgement())
    {
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
        return false;
    }

    MS_DBG(F("  Beginning concurrent measurement on"), getSensorNameAndLocation());
    startCommand = "";
    startCommand += _SDI12address;
    startCommand += "C!";  // Start concurrent measurement - format  [address]['C'][!]
    _SDI12Internal.sendCommand(startCommand);
    delay(30);  // It just needs this little delay
    MS_DBG(F("    >>>"), startCommand);

    // wait for acknowlegement with format
    // [address][ttt (3 char, seconds)][number of values to be returned, 0-9]<CR><LF>
    sdiResponse = _SDI12Internal.readStringUntil('\n');
    sdiResponse.trim();
    _SDI12Internal.clearBuffer();
    MS_DBG(F("    <<<"), sdiResponse);

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    if (!wasActive) _SDI12Internal.end();

    // Verify the number of results the sensor will send
    // uint8_t numVariables = sdiResponse.substring(4,5).toInt();
    // if (numVariables != _numReturnedVars)
    // {
    //     MS_DBG(numVariables, F("results expected"),
    //            F("This differs from the sensor's standard design of"),
    //            numReturnedVars, F("measurements!!"));
    // }

    // Set the times we've activated the sensor and asked for a measurement
    if (sdiResponse.length() > 0)
    {
        MS_DBG(F("    Concurrent measurement started."));
        // Update the time that a measurement was requested
        _millisMeasurementRequested = millis();
        // Set the status bit for measurement start success (bit 6)
        _sensorStatus |= 0b01000000;
        return true;
    }
    else
    {
        MS_DBG(getSensorNameAndLocation(), F("did not respond to measurement request!"));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
        return false;
    }
}


bool SDI12Sensors::addSingleMeasurementResult(void)
{
    bool success = false;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6))
    {
        // MS_DBG(F("   Activating SDI-12 instance for"), getSensorNameAndLocation());
        // Check if this the currently active SDI-12 Object
        bool wasActive = _SDI12Internal.isActive();
        // if (wasActive) {MS_DBG(F("   SDI-12 instance for"), getSensorNameAndLocation(),
        //                       F("was already active!"));}
        // If it wasn't active, activate it now.
        // Use begin() instead of just setActive() to ensure timer is set correctly.
        if (!wasActive) _SDI12Internal.begin();
        // Empty the buffer
        _SDI12Internal.clearBuffer();

        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
        String getDataCommand = "";
        getDataCommand += _SDI12address;
        getDataCommand += "D0!";  // SDI-12 command to get data [address][D][dataOption][!]
        _SDI12Internal.sendCommand(getDataCommand);
        delay(30);  // It just needs this little delay
        MS_DBG(F("    >>>"), getDataCommand);

        uint32_t startTime = millis();
        while (_SDI12Internal.available() < 3 && (millis() - startTime) < 1500) {}
        MS_DBG(F("  Receiving results from"), getSensorNameAndLocation());
        _SDI12Internal.read();  // ignore the repeated SDI12 address
        for (uint8_t i = 0; i < _numReturnedVars; i++)
        {
            float result = _SDI12Internal.parseFloat();
            // The SDI-12 library should return -9999 on timeout
            if (result == -9999 or isnan(result)) result = -9999;
            MS_DBG(F("    <<< Result #"), i, ':', result);
            verifyAndAddMeasurementResult(i, result);

        }
        // String sdiResponse = _SDI12Internal.readStringUntil('\n');
        // sdiResponse.trim();
        // _SDI12Internal.clearBuffer();
        // MS_DBG(F("    <<<"), sdiResponse);

        // Empty the buffer again
        _SDI12Internal.clearBuffer();

        // De-activate the SDI-12 Object
        // Use end() instead of just forceHold to un-set the timers
        if (!wasActive) _SDI12Internal.end();

        success  = true;
    }
    else
    {
        // If there's no measurement, need to make sure we send over all
        // of the "failed" result values
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
       for (uint8_t i = 0; i < _numReturnedVars; i++)
       {
           verifyAndAddMeasurementResult(i, (float)-9999);
       }
    }

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

     return success;
}
