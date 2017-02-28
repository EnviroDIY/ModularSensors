/*
 *DecagonSDI12.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.

 *
 *This file is for the Decagon Devices 5TM Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library.
*/

#include "DecagonSDI12.h"

// The constructor - need the SDI-12 address, the power pin, and the data pin
DecagonSDI12::DecagonSDI12(char SDI12address, int powerPin, int dataPin, int numReadings)
    : SensorBase(), mySDI12(dataPin)
{
    _SDI12address = SDI12address;
    _powerPin = powerPin;
    _dataPin = dataPin;
    _numReadings = numReadings;
}

// The function to set up connection to a sensor.
SENSOR_STATUS DecagonSDI12::setup(void)
{
    pinMode(_dataPin, INPUT);
    pinMode(_powerPin, OUTPUT);
    digitalWrite(_powerPin, HIGH);  // Need power to check connection

    // mySDI12.setDiagStream(Serial);  // For debugging
    mySDI12.begin();
    delay(500); // allow things to settle


    String myCommand = "";
    myCommand = "I";
    myCommand += (char) _SDI12address; // sends basic 'info' command [address][!]
    myCommand += "!";
    int i = 0;

    while (!mySDI12.available() and i < 3)// goes through three rapid contact attempts
    {
        mySDI12.sendCommand(myCommand);
        delay(500);
        i++;
    }
    if(mySDI12.available()) // if it hears anything it assumes a sensor is there
    {
        mySDI12.flush();
        Serial.print(F("Successfully connected to Decagon SDI-12 device at pin "));
        Serial.print(_dataPin);
        Serial.print(F(" and SDI-12 address "));
        Serial.println(_SDI12address);
        return SENSOR_READY;
    }
    else {   // otherwise it is vacant.
        mySDI12.flush();
        Serial.print(F("Failed to connect to Decagon SDI-12 device expected at pin "));
        Serial.print(_dataPin);
        Serial.print(F(" and SDI-12 address "));
        Serial.println(_SDI12address);
        return SENSOR_ERROR;
    }
    digitalWrite(_powerPin, LOW);  // Turn the power back off.
}

// The function to put the sensor to sleep
bool DecagonSDI12::sleep(void)
{
    mySDI12.flush();
    digitalWrite(_powerPin, LOW);
    return true;
}

// The function to wake up the sensor
bool DecagonSDI12::wake(void)
{
    digitalWrite(_powerPin, HIGH);
    return true;
}

// The sensor name
String DecagonSDI12::getSensorName(void)
{
    // Check if the power is on, turn it on if not
    bool wasOff = false;
    int powerBitNumber = log(digitalPinToBitMask(_powerPin))/log(2);
    if (bitRead(*portInputRegister(digitalPinToPort(_powerPin)), powerBitNumber) == LOW)
    {
        wasOff = true;
        digitalWrite(_powerPin, HIGH);
        delay(1000);
    }

    String myCommand = "";
    myCommand = "I";
    myCommand += (char) _SDI12address; // sends basic 'info' command [address][!]
    myCommand += "!";

    // wait for acknowlegement with format:
    // [address][SDI12 support (2 char)][vendor (8 char)][model (6 char)][version (3 char)][serial number (<14 char)]
    while(!mySDI12.available()>5);
    delay(100);

    char infoResponse[34];
    for (int i = 0; i < 33; i++)
    {
        infoResponse[i] = mySDI12.read();
    }

    DecagonSDI12::sensorName = infoResponse[3-17];

    // Turn the power back off it it had been turned on
    if (wasOff)
        {digitalWrite(_powerPin, LOW);}

    return DecagonSDI12::sensorName;
}

// The sensor installation location on the Mayfly
String DecagonSDI12::getSensorLocation(void)
{
    sensorLocation = String(_SDI12address) + "_" + String(_dataPin);
    return sensorLocation;
}


// Uses SDI-12 to communicate with a Decagon Devices 5TM
bool DecagonSDI12::update()
{
    // Check if the power is on, turn it on if not
    bool wasOff = false;
    int powerBitNumber = log(digitalPinToBitMask(_powerPin))/log(2);
    if (bitRead(*portInputRegister(digitalPinToPort(_powerPin)), powerBitNumber) == LOW)
    {
        wasOff = true;
        digitalWrite(_powerPin, HIGH);
        delay(1000);
    }

    String command = "";
    // averages x readings in this one loop
    for (int j = 0; j < _numReadings; j++)
    {
        command += _SDI12address;
        command += "M!"; // SDI-12 measurement command format  [address]['M'][!]
        mySDI12.sendCommand(command);

         // wait for acknowlegement with format [address][ttt (3 char, seconds)][number of measurments available, 0-9]
        while(!mySDI12.available()>5);
        delay(100);

        mySDI12.read(); // consume address

        // find out how long we have to wait (in seconds).
        unsigned int wait = 0;
        wait += 100 * mySDI12.read()-'0';
        wait += 10 * mySDI12.read()-'0';
        wait += 1 * mySDI12.read()-'0';

        DecagonSDI12::numMeasurements = mySDI12.read();
        mySDI12.read();  // ignore carriage return
        mySDI12.read();  // ignore line feed

        unsigned long timerStart = millis();
        while((millis() - timerStart) > (1000 * wait * 1.5))  //  Add a little extra to the wait
        {
            if(mySDI12.available()) break;  // sensor can interrupt us to let us know it is done early
        }

        // Wait for and discard anything else
        mySDI12.flush();

        command = "";
        command += _SDI12address;
        command += "D0!";  // SDI-12 command to get data [address][D][dataOption][!]
        mySDI12.sendCommand(command);
        delay(500);

        if(mySDI12.available())
        {
            //   Serial.println(F("Recieving data"));
            mySDI12.read();  // ignore the repeated SDI12 aSddress
            for (int i = 0; i < DecagonSDI12::numMeasurements; i++)
            {
                DecagonSDI12::sensorValues[i] += mySDI12.parseFloat();
            }
        }
            // Wait for and discard anything else
        mySDI12.flush();
    }

    // Average over the number of readings
    for (int i = 0; i < DecagonSDI12::numMeasurements; i++)
    {
        DecagonSDI12::sensorValues[i] /=  DecagonSDI12::numMeasurements;
    }

    // Make note of the last time updated
    DecagonSDI12::sensorLastUpdated = millis();

    // Turn the power back off it it had been turned on
    if (wasOff)
        {digitalWrite(_powerPin, LOW);}

    // Return true when finished
    return true;
}
