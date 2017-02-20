 /*
 *MaxbotixSonar.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Maxbotix Sonar Library
 *It is dependent on Software Serial.
 */

#include <Arduino.h>
#include "MaxbotixSonar.h"


// The constructor - need the power pin, the excite pin, and the data pin
MaxbotixSonar::MaxbotixSonar(int excitePin, int dataPin) : SensorBase()
{
    _excitePin = excitePin;
    _dataPin = dataPin;
    Serial.println("Setup start1");
    setup();
    Serial.println("Setup end2");
}

// The function to set up connection to a sensor.
SENSOR_STATUS MaxbotixSonar::setup(void)
{
    // define serial port for recieving data
    // output from maxSonar is inverted requiring true to be set.
    Serial.println("Setup start2");
    SoftwareSerialMod sonarSerial(_excitePin, -1);
    _sonarSerial = &sonarSerial;
    _sonarSerial->begin(9600);
    pinMode(_excitePin, OUTPUT);
    digitalWrite(_excitePin, LOW);
    Serial.println("Setup end1");
    return SENSOR_READY;
}

// The function to put a sensor to sleep
// Need to flush the serial port
bool MaxbotixSonar::sleep(void)
{
    _sonarSerial->flush();
    return true;
}

// The sensor name
String MaxbotixSonar::getSensorName(void)
{
    sensorName = F("MaxbotixSonar");
    return sensorName;
}

// The sensor installation location on the Mayfly
String MaxbotixSonar::getSensorLocation(void)
{
    sensorLocation = String(_excitePin) + "_" + String(_dataPin);
    return sensorLocation;
}

// The static variables that need to be updated
float MaxbotixSonar::sensorValue_depth = 0;

// Uses SDI-12 to communicate with a Decagon Devices CTD
bool MaxbotixSonar::update(){

    int range_try = 0;
    int result;
    char inData[5];  //char array to read data into
    int index = 0;
    bool stringComplete = false;

    digitalWrite(_excitePin, HIGH);
    delay(1000);

    _sonarSerial->flush();  // Clear cache ready for next reading

    while (stringComplete == false)
    {
        // Serial.print("OK reading! ");  //debug line

        if (_sonarSerial->available())
        {
            char rByte = _sonarSerial->read();  //read serial input for "R" to mark start of data
            if(rByte == 'R')
            {
                //Serial.println("rByte set");
                while (index < 4)  //read next three character for range from sensor
                {
                    if (_sonarSerial->available())
                    {
                        inData[index] = _sonarSerial->read();
                        //Serial.println(inData[index]);  //Debug line

                        index++;  // Increment where to write next
                    }
                }
                inData[index] = 0x00;  //add a padding byte at end for atoi() function
            }

            rByte = 0;  //reset the rByte ready for next reading
            index = 0;  // Reset index ready for next reading

            stringComplete = true;  // Set completion of read to true
            result = atoi(inData);  // Changes string data into an integer for use
            if (result == 300 && range_try < 20)
            {
                stringComplete = false;
                range_try++;
            }
        }
    }

    digitalWrite(_excitePin, LOW);

    MaxbotixSonar::sensorValue_depth = result;

    // Return true when finished
    return true;
}




MaxbotixSonar_Depth::MaxbotixSonar_Depth(int excitePin, int dataPin)
: MaxbotixSonar(excitePin, dataPin)
{
    _excitePin = excitePin;
    _dataPin = dataPin;
    setup();
}

String MaxbotixSonar_Depth::getVarName(void)
{
    varName = F("waterDepth");
    return varName;
}

String MaxbotixSonar_Depth::getVarUnit(void)
{
    String unit = F("millimeter");
    return unit;
}

float MaxbotixSonar_Depth::getValue(void)
{
    return sensorValue_depth;
}

String MaxbotixSonar_Depth::getDreamHost(void)
{
    String column = F("SonarRange");
    return column;
}
