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
MaxBotixSonar_Range::MaxBotixSonar_Range(int powerPin, int dataPin, int triggerPin /* = -1*/)
  : SensorBase(dataPin, powerPin, F("MaxBotixMaxSonar"), F("distance"), F("millimeter"), F("SonarRange"))
{_triggerPin = triggerPin;}

// The function to set up connection to a sensor.
// Need to override to get the trigger pin set
SENSOR_STATUS MaxBotixSonar_Range::setup(void)
{
    pinMode(_powerPin, OUTPUT);
    pinMode(_triggerPin, OUTPUT);
    pinMode(_dataPin, INPUT);
    digitalWrite(_powerPin, LOW);
    digitalWrite(_triggerPin, LOW);
    return SENSOR_READY;
}

// The static variables that need to be updated
unsigned long MaxBotixSonar_Range::sensorLastUpdated;

// Uses TLL Communication to get data from MaxBotix
bool MaxBotixSonar_Range::update(){

    // define serial port for recieving data
    // output from maxSonar is inverted requiring true to be set.
    SoftwareSerial sonarSerial(_dataPin, -1);
    sonarSerial.begin(9600);

    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}

    // Sonar sends a result just above it's max range when it gets a bad reading
    // For 10m models, this is 9999, for 5m models it's 4999
    int badResult = 9999;
    int result = badResult;  // initialize the result with a bad reading
    char inData[5];  // char array to read data into
    int index = 0;
    bool stringComplete = false;
    int rangeAttempts = 0;
    unsigned long timerStart = millis();

    // Trigger the reading to begin
    Serial.println(F("Triggering Sonar"));  // For debugging
    digitalWrite(_triggerPin, HIGH);
    delay(1);
    digitalWrite(_triggerPin, LOW);

    Serial.println(F("Beginning detection for Sonar"));  // For debugging
    while (/*stringComplete == false && */rangeAttempts < 50 && (millis() - timerStart) < 10000)
    {
        while (sonarSerial.available())
        {
            bool Rread = false;
            delay(3); // Let the buffer fill a little
            char rByte = sonarSerial.read();  //read serial input for "R" to mark start of data
            if(rByte == 'R')
            {
                Serial.println(F("'R' Byte found, reading next 4 characters:")); // For debugging
                while (index < 4)  //read next three character for range from sensor
                {
                    delay(3); // Let the buffer fill a little
                    inData[index] = sonarSerial.read();
                    Serial.print(inData[index]);  // For debugging
                    index++;  // Increment where to write next
                }
                inData[index] = 0x00;  //add a padding byte at end for atoi() function
                Rread = true;
                Serial.println();  // For debugging
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
            if (Rread && inData[1] != 'X' && inData[1] != 'L' && inData[1] != 'S'
                      && inData[1] != 'o' && inData[1] != 'H' && inData[1] != '\r'
                      && inData[0] != 'X' && inData[0] != 'L' && inData[0] != 'S'
                      && inData[0] != 'o' && inData[0] != 'H' && inData[0] != '\r'
                      && inData[0] != 'R' && inData[1] != 'R' && inData[2] != 'R'
                      && inData[3] != 'R' && inData[4] != 'R')
            {
                result = atoi(inData);  // Changes string data into an integer for use
                Serial.print(F("This converts to: ")); // For debugging
                Serial.println(result);  // For debugging
                memset(&inData[0], 0, sizeof(inData));  // Empty the inData array.
                if (result == 300 || result == 500 || result == 4999 || result == 9999)
                {
                    result = badResult;
                    sensorValue_depth = result;
                    stringComplete = false;
                    rangeAttempts++;
                    Serial.print(F("Bad or Suspicious Result, Retry Attempt #")); // For debugging
                    Serial.println(rangeAttempts); // For debugging
                }
                else
                {
                    stringComplete = true;  // Set completion of read to true
                    Serial.println(F("Good result found"));  // For debugging
                }
            }
            else if (Rread)
            {
                Serial.println(F("Ignoring header or garbled line")); // For debugging
                memset(&inData[0], 0, sizeof(inData));  // Empty the inData array.
            }
        }
    }

    sensorValue_depth = result;
    Serial.println(sensorValue_depth);  // For debugging

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Return true when finished
    MaxBotixSonar_Range::sensorLastUpdated = millis();
    return true;
}


float MaxBotixSonar_Range::getValue(void)
{
    checkForUpdate(MaxBotixSonar_Range::sensorLastUpdated);
    return sensorValue_depth;
}
