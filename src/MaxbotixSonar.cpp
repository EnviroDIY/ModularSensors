 /*
 *MaxBotixSonar.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the MaxBotix Sonar Library
 *It is dependent on Software Serial.
 *
 * Information on the MaxBotix Sonar communication protocol can be found here:
 * http://www.MaxBotix.com/documents/HRXL-MaxSonar-WR_Datasheet.pdf
 */

#include <Arduino.h>
#include "MaxBotixSonar.h"


// The constructor - need the power pin, the excite pin, and the data pin
MaxBotixSonar::MaxBotixSonar(int powerPin, int dataPin) : SensorBase()
{
    _powerPin = powerPin;
    _dataPin = dataPin;
}

// The function to set up connection to a sensor.
SENSOR_STATUS MaxBotixSonar::setup(void)
{
    pinMode(_powerPin, OUTPUT);
    pinMode(_dataPin, INPUT);
    digitalWrite(_powerPin, LOW);
    return SENSOR_READY;
}

// The function to put the sensor to sleep
bool MaxBotixSonar::sleep(void)
{
    digitalWrite(_powerPin, LOW);
    return true;
}

// The function to wake up the sensor
bool MaxBotixSonar::wake(void)
{
    digitalWrite(_powerPin, HIGH);
    return true;
}

// The sensor name
String MaxBotixSonar::getSensorName(void)
{
    sensorName = F("MaxBotixMaxSonar");
    return sensorName;
}

// The sensor installation location on the Mayfly
String MaxBotixSonar::getSensorLocation(void)
{
    sensorLocation = String(_powerPin) + "_" + String(_dataPin);
    return sensorLocation;
}

// The static variables that need to be updated
float MaxBotixSonar::sensorValue_depth = 0;


// Uses TLL Communication to get data from MaxBotix
bool MaxBotixSonar::update(){

    // define serial port for recieving data
    // output from maxSonar is inverted requiring true to be set.
    SoftwareSerialMod sonarSerial(_dataPin, -1);
    sonarSerial.begin(9600);

    // Sonar sends a result just above it's max range when it gets a bad reading
    // For 10m models, this is 9999, for 5m models it's 4999
    int badResult = 9999;
    int result = badResult;  // initialize the result with a bad reading
    char inData[5];  // char array to read data into
    int index = 0;
    bool stringComplete = false;
    int rangeAttempts = 0;

    // Check if the power is on, turn it on if not
    bool wasOff = false;
    if (bitRead(digitalPinToPort(_powerPin), digitalPinToBitMask(_powerPin)) == LOW)
    {
        wasOff = true;
        pinMode(_powerPin, OUTPUT);
        digitalWrite(_powerPin, HIGH);
        delay(1000);
    }

    // Serial.println(F("Beginning detection for Sonar"));  // debug line
    sonarSerial.flush();  // Clear cache ready for next reading
    while (stringComplete == false)
    {
        while (sonarSerial.available())
        {
            delay(3);  // It just works better with this delay.  4 is too much, 2 is too little.
            char rByte = sonarSerial.read();  //read serial input for "R" to mark start of data
            if(rByte == 'R')
            {
                // Serial.println(F("'R' Byte found, reading next 4 characters:")); // Debug line
                while (index < 4)  //read next three character for range from sensor
                {
                    if (sonarSerial.available())
                    {
                        inData[index] = sonarSerial.read();
                        // Serial.print(inData[index]);  // Debug line
                        index++;  // Increment where to write next
                    }
                }
                inData[index] = 0x00;  //add a padding byte at end for atoi() function
                // Serial.println();  // Debug line
            }
            rByte = 0;  // Reset the rByte ready for next reading
            index = 0;  // Reset index ready for next reading

            stringComplete = true;  // Set completion of read to true
            result = atoi(inData);  // Changes string data into an integer for use
            if ((result == 300 || result == 500 || result == 4999 || result == 9999) && rangeAttempts < 20)
            {
                // Serial.println(F("Bad or Suspicious Result, Retrying")); // Debug line
                stringComplete = false;
                rangeAttempts++;
            }
        }
    }

    // Turn the power back off it it had been turned on
    if (wasOff)
        {digitalWrite(_powerPin, LOW);}

    MaxBotixSonar::sensorValue_depth = result;

    // Return true when finished
    return true;
}




MaxBotixSonar_Depth::MaxBotixSonar_Depth(int powerPin, int dataPin)
: MaxBotixSonar(powerPin, dataPin)
{
    _powerPin = powerPin;
    _dataPin = dataPin;
    setup();
}

String MaxBotixSonar_Depth::getVarName(void)
{
    varName = F("waterDepth");
    return varName;
}

String MaxBotixSonar_Depth::getVarUnit(void)
{
    String unit = F("millimeter");
    return unit;
}

float MaxBotixSonar_Depth::getValue(void)
{
    return sensorValue_depth;
}

String MaxBotixSonar_Depth::getDreamHost(void)
{
    String column = F("SonarRange");
    return column;
}
