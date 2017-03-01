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
    : SensorBase()
{
    _SDI12address = SDI12address;
    _powerPin = powerPin;
    _dataPin = dataPin;
    _numReadings = numReadings;
    setup();
}

// The function to set up connection to a sensor.
SENSOR_STATUS DecagonSDI12::setup(void)
{
    pinMode(_dataPin, INPUT);
    pinMode(_powerPin, OUTPUT);
    digitalWrite(_powerPin, LOW);
    return SENSOR_READY;
}

// The function to put the sensor to sleep
bool DecagonSDI12::sleep(void)
{
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
    SDI12 mySDI12(_dataPin);
    // mySDI12.setDiagStream(Serial);  // For debugging
    mySDI12.begin();
    delay(500); // allow things to settle

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

    sensorName = infoResponse[3-17];

    // Turn the power back off it it had been turned on
    if (wasOff)
        {digitalWrite(_powerPin, LOW);}

    return sensorName;
}

// The sensor installation location on the Mayfly
String DecagonSDI12::getSensorLocation(void)
{
    sensorLocation = String(_SDI12address) + "_" + String(_dataPin);
    return sensorLocation;
}

int DecagonSDI12::numMeasurements = 0;
float DecagonSDI12::sensorValues[9] = {0};  // Know that all Decagon SDI12 sensors will return 9 or fewer measurements

// Uses SDI-12 to communicate with a Decagon Devices 5TM
bool DecagonSDI12::update()
{
    SDI12 mySDI12(_dataPin);
    // mySDI12.setDiagStream(Serial);  // For debugging
    mySDI12.begin();
    delay(500); // allow things to settle

    // Check if the power is on, turn it on if not
    bool wasOff = false;
    int powerBitNumber = log(digitalPinToBitMask(_powerPin))/log(2);
    if (bitRead(*portInputRegister(digitalPinToPort(_powerPin)), powerBitNumber) == LOW)
    {
        wasOff = true;
        Serial.println(F("Powering on Sensor"));  // Debug line
        digitalWrite(_powerPin, HIGH);
        delay(1000);
    }

    String command = "";
    String sdiResponse = "";
    // averages x readings in this one loop
    for (int j = 0; j < _numReadings; j++)
    {
        Serial.print(F("Taking reading #"));  // Debug line
        Serial.println(j);  // Debug line
        command = "";
        command += _SDI12address;
        command += "M!"; // SDI-12 measurement command format  [address]['M'][!]
        mySDI12.sendCommand(command);
        Serial.println(command);  // Debug line
        delay(100);

        // wait for acknowlegement with format [address][ttt (3 char, seconds)][number of measurments available, 0-9]
        while (mySDI12.available())  // build response string
        {
            char c = mySDI12.read();
            if ((c != '\n') && (c != '\r'))
            {
                sdiResponse += c;
                delay(5);
            }
        }
        if (sdiResponse.length() > 1) Serial.println(sdiResponse);  // Debug line
        mySDI12.flush();
        Serial.println(sdiResponse.substring(1,4));
        Serial.println(sdiResponse.substring(4,5));

        // find out how long we have to wait (in seconds).
        unsigned int wait = 0;
        wait = sdiResponse.substring(1,4).toInt();
        Serial.print(F("Waiting "));  // Debug line
        Serial.print(wait);  // Debug line
        Serial.println(F(" seconds for measurement"));  // Debug line

        // Set up the number of results to expect
        DecagonSDI12::numMeasurements =  sdiResponse.substring(4,5).toInt();
        Serial.print(DecagonSDI12::numMeasurements);  // Debug line
        Serial.println(F(" results expected"));  // Debug line

        unsigned long timerStart = millis();
        while((millis() - timerStart) > (1000 * wait))
        {
            if(mySDI12.available()) break;  // sensor can interrupt us to let us know it is done early
        }

        Serial.println(F("Requesting data"));  // Debug line
        command = "";
        command += _SDI12address;
        command += "D0!";  // SDI-12 command to get data [address][D][dataOption][!]
        mySDI12.sendCommand(command);
        Serial.println(command);  // Debug line
        delay(100);

/*
        sdiResponse = "";
        while (mySDI12.available())  // build response string
        {
            char c = mySDI12.read();
            if ((c != '\n') && (c != '\r'))
            {
                sdiResponse += c;
                delay(5);
            }
        }
        if (sdiResponse.length() > 1) Serial.println(sdiResponse); //write the response to the screen
        mySDI12.flush();
*/

        while(mySDI12.available())
        {
            //   Serial.println(F("Recieving data"));
            mySDI12.read();  // ignore the repeated SDI12 aSddress
            for (int i = 0; i < DecagonSDI12::numMeasurements; i++)
            {
                float result = mySDI12.parseFloat();
                DecagonSDI12::sensorValues[i] += result;
                Serial.print(F("Result #"));  // Debug line
                Serial.print(i);  // Debug line
                Serial.print(F(": "));  // Debug line
                Serial.println(result);  // Debug line
            }
        }
            // Wait for and discard anything else
        mySDI12.flush();
    }
/*
    // Average over the number of readings
    Serial.println(F("Averaging over readings"));  // Debug line
    for (int i = 0; i < DecagonSDI12::numMeasurements; i++)
    {
        DecagonSDI12::sensorValues[i] /=  DecagonSDI12::numMeasurements;
        Serial.print(F("Result #"));  // Debug line
        Serial.print(i);  // Debug line
        Serial.print(F(": "));  // Debug line
        Serial.println(sensorValues[i]);  // Debug line
    }
*/

    // Turn the power back off it it had been turned on
    if (wasOff)
        {
            digitalWrite(_powerPin, LOW);
            Serial.println(F("Turning off Power"));  // Debug line
        }

    // Return true when finished
    return true;
}
