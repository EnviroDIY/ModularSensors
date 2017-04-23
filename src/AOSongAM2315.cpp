/*
 *AOSongAM2315.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the AOSong AM2315 Capacitive Humidity and Temperature sensor
 *It is dependent on the Adafruit AM2315 Library
 *
 *Documentation for the sensor can be found at: www.aosong.com/asp_bin/Products/en/AM2315.pdf
 *
 * For Relative Humidity:
 *  Resolution is 0.1 % RH (16 bit)
 *  Accuracy is ± 2 % RH
 *
 * For Temperature:
 *  Resolution is 0.1°C (16 bit)
 *  Accuracy is ±0.1°C
 *  Range is -40°C to +125°C
*/

#include "AOSongAM2315.h"
#include <Adafruit_AM2315.h>


void Variable::attachSensor(int varNum, Sensor *parentSensor) {
    Serial.println(F("Attempting to register to parent"));
    parentSensor->registerVariable(varNum, this);
}

float Variable::getValue(void)
{
    // checkForUpdate(parentSensor->sensorLastUpdated);
    return sensorValue;
}


// The constructor - because this is I2C, only need the power pin
AOSongAM2315::AOSongAM2315(void)
{
    // Clear arrays
    variables[MAX_NUMBER_VARIABLES] = {0};
    sensorValues[MAX_NUMBER_VARIABLES] = {0};
}

String AOSongAM2315::getSensorLocation(void){return F("I2C_0xB8");}
String AOSongAM2315::getSensorName(void){return F("AOSongAM2315");}

void AOSongAM2315::registerVariable(int varNum, Variable* var)
{
    variables[varNum] = var;
    Serial.println(F("Variable registration accepted."));
}

bool AOSongAM2315::update(void)
{
    Adafruit_AM2315 am2315;  // create a sensor object
    Wire.begin();  // Start the wire library

    // Clear out values
    // sensorValues[AM2315_NUM_MEASUREMENTS] = {0};

    float temp_val, humid_val;
    bool ret_val = am2315.readTemperatureAndHumidity(temp_val, humid_val);
    sensorValues[temp] = temp_val;
    sensorValues[humidity] = humid_val;
    Serial.print(F("Temp is: "));
    Serial.print(sensorValues[temp]);
    Serial.print(F("°C and humidity is: "));
    Serial.print(sensorValues[humidity]);
    Serial.println(F("%"));
    // Make note of the last time updated
    sensorLastUpdated = millis();

    // Notify variables of update
    for (uint8_t i = 0; i < AM2315_NUM_MEASUREMENTS; i++){
        if (variables[i] != nullptr) {
            Serial.print(F("Sending value update to variable"));
            Serial.println(i);
            variables[i]->onSensorUpdate(this);
        }
    }

    return ret_val;
}





AOSongAM2315_Humidity::AOSongAM2315_Humidity(Sensor *parentSense)
{parentSensor = parentSense;}

void AOSongAM2315_Humidity::setup(void)
{attachSensor(humidity, parentSensor);}

void AOSongAM2315_Humidity::onSensorUpdate(Sensor *parentSense)
{
    sensorValue = parentSense->sensorValues[humidity];
    Serial.print(F("Recieved humidity update of "));
    Serial.println(sensorValue);
}




AOSongAM2315_Temp::AOSongAM2315_Temp(Sensor *parentSense)
{parentSensor = parentSense;}

void AOSongAM2315_Temp::setup(void)
{attachSensor(temp, parentSensor);}

void AOSongAM2315_Temp::onSensorUpdate(Sensor *parentSense)
{
    sensorValue = parentSense->sensorValues[temp];
    Serial.print(F("Recieved temperature update of "));
    Serial.println(sensorValue);
}
