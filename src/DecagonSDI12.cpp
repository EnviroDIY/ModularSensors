/*
 *DecagonSDI12.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).

 *
 *This file is for the Decagon Devices 5TM Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library.
*/

#include "DecagonSDI12.h"

// The constructor - need the number of measurements the sensor will return, SDI-12 address, the power pin, and the data pin
DecagonSDI12::DecagonSDI12(char SDI12address, int powerPin, int dataPin,
                           int numReadings,
                           String sensName, int numMeasurements)
    : Sensor(powerPin, dataPin, sensName, numMeasurements)
{
    _SDI12address = SDI12address;
    _numReadings = numReadings;
}
DecagonSDI12::DecagonSDI12(char *SDI12address, int powerPin, int dataPin,
                           int numReadings,
                           String sensName, int numMeasurements)
    : Sensor(powerPin, dataPin, sensName, numMeasurements)
{
    _SDI12address = *SDI12address;
    _numReadings = numReadings;
}
DecagonSDI12::DecagonSDI12(int SDI12address, int powerPin, int dataPin,
                           int numReadings,
                           String sensName, int numMeasurements)
    : Sensor(powerPin, dataPin, sensName, numMeasurements)
{
    _SDI12address = SDI12address + '0';
    _numReadings = numReadings;
}


// A helper functeion to run the "sensor info" SDI12 command
void DecagonSDI12::getSensorInfo(void)
{
    SDI12 mySDI12(_dataPin);
    mySDI12.begin();
    delay(500); // allow things to settle

    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}

    DBGM(F("Getting sensor info\n"));
    String myCommand = "";
    myCommand += (char) _SDI12address;
    myCommand += "I!"; // sends 'info' command [address][I][!]
    mySDI12.sendCommand(myCommand);
    DBGM(myCommand, F("\n"));
    delay(30);

    // wait for acknowlegement with format:
    // [address][SDI12 support (2 char)][vendor (8 char)][model (6 char)][version (3 char)][serial number (<14 char)]
    String sdiResponse = "";
    while (mySDI12.available())  // build response string
    {
        char c = mySDI12.read();
        if ((c != '\n') && (c != '\r'))
        {
            sdiResponse += c;
            delay(5);
        }
    }
    if (sdiResponse.length() > 1) DBGM(sdiResponse, F("\n"));
    _sensorName = sdiResponse.substring(3,17);
    _sensorName.trim();
    _sensorVendor = sdiResponse.substring(3,11);
    _sensorVendor.trim();
    _sensorModel = sdiResponse.substring(11,17);
    _sensorModel.trim();
    _sensorVersion = sdiResponse.substring(17,20);
    _sensorVersion.trim();
    _sensorSerialNumber = sdiResponse.substring(20,17);
    _sensorSerialNumber.trim();
    mySDI12.flush();

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

}

// The sensor name
String DecagonSDI12::getSensorVendor(void)
{return _sensorVendor;}

// The sensor name
String DecagonSDI12::getSensorModel(void)
{return _sensorModel;}

// The sensor name
String DecagonSDI12::getSensorVersion(void)
{return _sensorVersion;}

// The sensor name
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
    SDI12 mySDI12(_dataPin);
    mySDI12.begin();
    mySDI12.setTimeout(15);  // SDI-12 protocol says sensors must respond within 15 milliseconds
    // delay(500); // allow things to settle

    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}

    // Clear values before starting loop
    clearValues();

    // averages x readings in this one loop
    for (int j = 0; j < _numReadings; j++)
    {
        DBGM(F("Taking reading #"), j, F("\n"));
        String myCommand = "";
        myCommand += _SDI12address;
        myCommand += "M!"; // SDI-12 measurement myCommand format  [address]['M'][!]
        mySDI12.sendCommand(myCommand);
        DBGM(myCommand, F("\n"));
        mySDI12.flush();
        delay(30);  // It just needs this little delay

        // wait for acknowlegement with format [address][ttt (3 char, seconds)][number of measurments available, 0-9]
        String sdiResponse = mySDI12.readString();
        DBGM(sdiResponse, F("\n"));

        // find out how long we have to wait (in seconds).
        unsigned int wait = 0;
        wait = sdiResponse.substring(1,4).toInt();
        DBGM(F("Waiting "), wait, F(" seconds for measurement\n"));

        // Set up the number of results to expect
        int numMeasurements = sdiResponse.substring(4,5).toInt();
        DBGM(numMeasurements, F(" results expected\n"));
        if (numMeasurements != _numReturnedVars)
        {
            DBGM(F("This differs from the sensor's standard design of "));
            DBGM(_numReturnedVars, F(" measurements!!\n"));
        }

        unsigned long timerStart = millis();
        while((millis() - timerStart) < (1000 * wait))
        {
            if(mySDI12.available())  // sensor can interrupt us to let us know it is done early
            {
                DBGM("Wait interrupted!", F("\n"));
                mySDI12.readString();  // Read the service request (the address again)
                delay(5);  // Necessary for reasons unbeknownst to me (else it just fails sometimes..)
                break;
            }
        }

        myCommand = "";
        myCommand += _SDI12address;
        myCommand += "D0!";  // SDI-12 command to get data [address][D][dataOption][!]
        mySDI12.sendCommand(myCommand);
        DBGM(myCommand, F("\n"));
        mySDI12.flush();
        delay(30);  // It just needs this little delay

        DBGM(F("Receiving data\n"));
        mySDI12.read();  // ignore the repeated SDI12 address
        for (int i = 0; i < _numReturnedVars; i++)
        {
            float result = mySDI12.parseFloat();
            sensorValues[i] += result;
            DBGM(F("Result #"), i, F(": "), result, F("\n"));
        }
    }

    // Average over the number of readings
    DBGM(F("Averaging over "), _numReadings, F(" readings\n"));
    for (int i = 0; i < _numReturnedVars; i++)
    {
        sensorValues[i] /=  _numReadings;
        DBGM(F("Result #"), i, F(": "), sensorValues[i], F("\n"));
    }

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Update the registered variables with the new values
    notifyVariables();

    // Return true when finished
    return true;
}
