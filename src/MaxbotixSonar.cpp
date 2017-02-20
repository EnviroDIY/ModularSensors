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
}

// The function to set up connection to a sensor.
SENSOR_STATUS MaxbotixSonar::setup(void)
{
    pinMode(_excitePin, OUTPUT);
    digitalWrite(_excitePin, LOW);
    return SENSOR_READY;
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

    // define serial port for recieving data
    // output from maxSonar is inverted requiring true to be set.
    SoftwareSerialMod sonarSerial(_excitePin, -1);
    sonarSerial.begin(9600);

    Serial.println("starting maxbotix update");
    int range_try = 0;
    int result;
    char inData[5];  //char array to read data into
    int index = 0;
    bool stringComplete = false;

    Serial.println("Turning on excite pin");
    digitalWrite(_excitePin, HIGH);
    delay(1000);

    int timeout = 150; // only try for 15 seconds
    while ((timeout > 0) && stringComplete == false)
    {
        if (sonarSerial.available())
        {
            Serial.println("Looking for reading");  //debug line
            char rByte = sonarSerial.read();  //read serial input for "R" `to mark start of data
            if(rByte == 'R')
            {
                Serial.println("rByte set");
                while (index < 4)  //read next three character for range from sensor
                {
                    if (sonarSerial.available())
                    {
                        inData[index] = sonarSerial.read();
                        Serial.println(inData[index]);  //Debug line

                        index++;  // Increment where to write next
                    }
                }
                inData[index] = 0x00;  //add a padding byte at end for atoi() function
            }

            rByte = 0;  //reset the rByte ready for next reading
            index = 0;  // Reset index ready for next reading

            stringComplete = true;  // Set completion of read to true
            result = atoi(inData);  // Changes string data into an integer for use
            Serial.println("Result recieved");  //debug line
            if (result == 300 && range_try < 20)
            {
                Serial.println("Bad result, retrying");  //debug line
                stringComplete = false;
                range_try++;
            }
        }
        else
        {
            delay(10);
            timeout--;
        }
    }

    Serial.println("Turning off excite pin");
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
