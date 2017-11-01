/*
 *DecagonSDI12.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).

 *
 *This file is for the Decagon Devices 5TM Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library.
*/

#define LIBCALL_ENABLEINTERRUPT  // To prevent compiler/linker crashes
#include <EnableInterrupt.h>  // To handle external and pin change interrupts
#include <SDI12_ExtInts.h>

#include "DecagonSDI12.h"

// The constructor - need the number of measurements the sensor will return, SDI-12 address, the power pin, and the data pin
DecagonSDI12::DecagonSDI12(char SDI12address, int powerPin, int dataPin,
                           int numReadings, String sensName,
                           int numMeasurements, int WarmUpTime_ms)
    : Sensor(powerPin, dataPin, sensName, numMeasurements, WarmUpTime_ms)
{
    _SDI12address = SDI12address;
    _numReadings = numReadings;
}
DecagonSDI12::DecagonSDI12(char *SDI12address, int powerPin, int dataPin,
                           int numReadings, String sensName,
                           int numMeasurements, int WarmUpTime_ms)
    : Sensor(powerPin, dataPin, sensName, numMeasurements, WarmUpTime_ms)
{
    _SDI12address = *SDI12address;
    _numReadings = numReadings;
}
DecagonSDI12::DecagonSDI12(int SDI12address, int powerPin, int dataPin,
                           int numReadings, String sensName,
                           int numMeasurements, int WarmUpTime_ms)
    : Sensor(powerPin, dataPin, sensName, numMeasurements, WarmUpTime_ms)
{
    _SDI12address = SDI12address + '0';
    _numReadings = numReadings;
}


SENSOR_STATUS DecagonSDI12::setup(void)
{
    if (_powerPin > 0) pinMode(_powerPin, OUTPUT);
    pinMode(_dataPin, INPUT_PULLUP);

    bool isSet = getSensorInfo();

    if (isSet)
    {
        MS_DBG(F("Set up "), getSensorName(), F(" attached at "), getSensorLocation());
        MS_DBG(F(" which can return up to "), _numReturnedVars, F(" variable[s].\n"));
        return SENSOR_READY;
    }
    else return SENSOR_ERROR;
}


SENSOR_STATUS DecagonSDI12::getStatus(void)
{
    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    // Wait until the sensor is warmed up
    waitForWarmUp();

    SDI12 mySDI12(_dataPin);
    mySDI12.begin();
    mySDI12.setTimeout(15);  // SDI-12 protocol says sensors must respond within 15 milliseconds
    enableInterrupt(_dataPin, SDI12::handleInterrupt, CHANGE);

    MS_DBG(F("Asking for sensor acknowlegement\n"));
    String myCommand = "";
    myCommand += (char) _SDI12address;
    myCommand += "!"; // sends 'acknowledge active' command [address][!]
    mySDI12.sendCommand(myCommand);
    MS_DBG(F(">>"), myCommand, F("\n"));
    delay(30);

    // wait for acknowlegement with format:
    // [address]<CR><LF>
    String sdiResponse = mySDI12.readStringUntil('\n');
    sdiResponse.trim();
    MS_DBG(F("<<"), sdiResponse, F("\n"));

    // Kill the SDI-12 Object
    disableInterrupt(_dataPin);
    mySDI12.clearBuffer();
    mySDI12.forceHold();
    mySDI12.end();

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

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

    // Start a new SDI-12 instance
    SDI12 mySDI12(_dataPin);
    mySDI12.begin();
    mySDI12.setTimeout(15);  // SDI-12 protocol says sensors must respond within 15 milliseconds
    enableInterrupt(_dataPin, SDI12::handleInterrupt, CHANGE);

    MS_DBG(F("Getting sensor info\n"));
    String myCommand = "";
    myCommand += (char) _SDI12address;
    myCommand += "I!"; // sends 'info' command [address][I][!]
    mySDI12.sendCommand(myCommand);
    MS_DBG(F(">>"), myCommand, F("\n"));
    delay(30);

    // wait for acknowlegement with format:
    // [address][SDI12 version supported (2 char)][vendor (8 char)][model (6 char)][version (3 char)][serial number (<14 char)]<CR><LF>
    String sdiResponse = mySDI12.readStringUntil('\n');
    sdiResponse.trim();
    MS_DBG(F("<<"), sdiResponse, F("\n"));

    // Kill the SDI-12 Object
    disableInterrupt(_dataPin);
    mySDI12.clearBuffer();
    mySDI12.forceHold();
    mySDI12.end();

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

// Uses SDI-12 to communicate with a Decagon Devices 5TM
bool DecagonSDI12::update()
{
    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    // Wait until the sensor is warmed up
    waitForWarmUp();

    // Clear values before starting loop
    clearValues();

    // Check that the sensor is there and responding
    if (getStatus() == SENSOR_ERROR) return false;

    // Start a new SDI-12 instance
    SDI12 mySDI12(_dataPin);
    mySDI12.begin();
    mySDI12.setTimeout(15);  // SDI-12 protocol says sensors must respond within 15 milliseconds
    enableInterrupt(_dataPin, SDI12::handleInterrupt, CHANGE);

    // averages x readings in this one loop
    for (int j = 0; j < _numReadings; j++)
    {
        MS_DBG(F("Taking reading #"), j, F("\n"));
        String myCommand = "";
        myCommand += _SDI12address;
        myCommand += "M!"; // SDI-12 measurement myCommand format  [address]['M'][!]
        mySDI12.sendCommand(myCommand);
        MS_DBG(F(">>"), myCommand, F("\n"));
        delay(30);  // It just needs this little delay

        // wait for acknowlegement with format
        // [address][ttt (3 char, seconds)][number of measurments available, 0-9]<CR><LF>
        String sdiResponse = mySDI12.readStringUntil('\n');
        sdiResponse.trim();
        mySDI12.clearBuffer();
        MS_DBG(F("<<"), sdiResponse, F("\n"));

        // find out how long we have to wait (in seconds).
        unsigned int wait = 0;
        wait = sdiResponse.substring(1,4).toInt();
        MS_DBG(F("Waiting "), wait, F(" seconds for measurement\n"));

        // Set up the number of results to expect
        int numMeasurements = sdiResponse.substring(4,5).toInt();
        MS_DBG(numMeasurements, F(" results expected\n"));
        if (numMeasurements != _numReturnedVars)
        {
            MS_DBG(F("This differs from the sensor's standard design of "));
            MS_DBG(_numReturnedVars, F(" measurements!!\n"));
        }

        unsigned long timerStart = millis();
        while((millis() - timerStart) < (1000 * wait))
        {
            if(mySDI12.available())  // sensor can interrupt us to let us know it is done early
            {
                MS_DBG(F("<<"), mySDI12.readString());  // Read the service request (the address again)
                MS_DBG("Wait interrupted!", F("\n"));
                mySDI12.clearBuffer();
                delay(5);  // Necessary for reasons unbeknownst to me (else it just fails sometimes..)
                break;
            }
        }

        myCommand = "";
        myCommand += _SDI12address;
        myCommand += "D0!";  // SDI-12 command to get data [address][D][dataOption][!]
        mySDI12.sendCommand(myCommand);
        MS_DBG(F(">>"), myCommand, F("\n"));
        delay(30);  // It just needs this little delay

        MS_DBG(F("Receiving data\n"));
        mySDI12.read();  // ignore the repeated SDI12 address
        for (int i = 0; i < _numReturnedVars; i++)
        {
            float result = mySDI12.parseFloat();
            sensorValues[i] += result;
            MS_DBG(F("Result #"), i, F(": "), result, F("\n"));
        }
        mySDI12.clearBuffer();
    }

    // Average over the number of readings
    MS_DBG(F("Averaging over "), _numReadings, F(" readings\n"));
    for (int i = 0; i < _numReturnedVars; i++)
    {
        sensorValues[i] /=  _numReadings;
        MS_DBG(F("Result #"), i, F(": "), sensorValues[i], F("\n"));
    }

    // Kill the SDI-12 Object
    disableInterrupt(_dataPin);
    mySDI12.clearBuffer();
    mySDI12.forceHold();
    mySDI12.end();

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Update the registered variables with the new values
    notifyVariables();

    // Return true when finished
    return true;
}
