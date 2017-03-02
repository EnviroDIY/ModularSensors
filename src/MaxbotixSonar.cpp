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
float MaxBotixSonar::sensorValue_depth = 0.00;
unsigned long MaxBotixSonar::sensorLastUpdated;


// Uses TLL Communication to get data from MaxBotix
bool MaxBotixSonar::update(){

    // define serial port for recieving data
    // output from maxSonar is inverted requiring true to be set.
    SoftwareSerial sonarSerial(_dataPin, -1);
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
    int powerBitNumber = log(digitalPinToBitMask(_powerPin))/log(2);
    if (bitRead(*portInputRegister(digitalPinToPort(_powerPin)), powerBitNumber) == LOW)
    {
        wasOff = true;
        pinMode(_powerPin, OUTPUT);
        digitalWrite(_powerPin, HIGH);
        delay(1000);
    }

    // Serial.println(F("Beginning detection for Sonar"));  // For debugging
    while (stringComplete == false && rangeAttempts < 50)
    {
        while (sonarSerial.available())
        {
            bool Rread = false;
            delay(3); // Let the buffer fill a little
            char rByte = sonarSerial.read();  //read serial input for "R" to mark start of data
            if(rByte == 'R')
            {
                // Serial.println(F("'R' Byte found, reading next 4 characters:")); // For debugging
                while (index < 4)  //read next three character for range from sensor
                {
                    delay(3); // Let the buffer fill a little
                    inData[index] = sonarSerial.read();
                    // Serial.print(inData[index]);  // For debugging
                    index++;  // Increment where to write next
                }
                inData[index] = 0x00;  //add a padding byte at end for atoi() function
                Rread = true;
                // Serial.println();  // For debugging
                // Serial.print(F("inData[0]:")); // For debugging
                // Serial.println(inData[0]);  // For debugging
                // Serial.print(F("inData[1]:")); // For debugging
                // Serial.println(inData[1]);  // For debugging
                // Serial.print(F("inData[2]:")); // For debugging
                // Serial.println(inData[2]);  // For debugging
                // Serial.print(F("inData[3]:")); // For debugging
                // Serial.println(inData[3]);  // For debugging
                // Serial.print(F("inData[4]:")); // For debugging
                // Serial.println(inData[4]);  // For debugging
            }
            rByte = 0;  // Reset the rByte ready for next reading
            index = 0;  // Reset index ready for next reading

            // Make sure R is not part of the header, part number, or RoHS warning line
            // ie, "HRXL-MaxSonar-WRL" or "RoHS 1.8b078  0713"
            if (Rread && inData[0] != 'R' && inData[1] != 'X' && inData[1] != 'L' && inData[1] != 'S'
                      && inData[1] != 'o' && inData[1] != 'H' && inData[1] != '\r')
            {
                result = atoi(inData);  // Changes string data into an integer for use
                // Serial.print(F("This converts to: ")); // For debugging
                // Serial.println(result);  // For debugging
                memset(&inData[0], 0, sizeof(inData));  // Empty the inData array.
                if (result == 300 || result == 500 || result == 4999 || result == 9999)
                {
                    result = badResult;
                    MaxBotixSonar::sensorValue_depth = result;
                    stringComplete = false;
                    rangeAttempts++;
                    // Serial.print(F("Bad or Suspicious Result, Retry Attempt #")); // For debugging
                    // Serial.println(rangeAttempts); // For debugging
                }
                else
                {
                    stringComplete = true;  // Set completion of read to true
                    // Serial.println(F("Good result found"));  // For debugging
                }
            }
            else if (Rread)
            {
                // Serial.println(F("Ignoring header line")); // For debugging
                memset(&inData[0], 0, sizeof(inData));  // Empty the inData array.
            }
        }
    }

    MaxBotixSonar::sensorValue_depth = result;
    // Serial.println(MaxBotixSonar::sensorValue_depth);  // For debugging

    // Turn the power back off it it had been turned on
    if (wasOff)
        {digitalWrite(_powerPin, LOW);}

    // Return true when finished
    sonarSerial.flush();  // Clear cache ready for next reading
    MaxBotixSonar::sensorLastUpdated = millis();
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
    if ((millis() > 30000 and millis() > MaxBotixSonar::sensorLastUpdated + 30000) or MaxBotixSonar::sensorLastUpdated == 0)
    {
        Serial.println(F("Value out of date, updating"));  // For debugging
        MaxBotixSonar::update();
    }
    return MaxBotixSonar::sensorValue_depth;
}

String MaxBotixSonar_Depth::getDreamHost(void)
{
    String column = F("SonarRange");
    return column;
}
