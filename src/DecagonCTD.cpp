/*
 *DecagonCTD.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Decagon Devices CTD-10
 *It is dependent on the EnviroDIY SDI-12 library.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon CTD-10 can be found at:
 *http://manuals.decagon.com/Integration%20Guides/CTD%20Integrators%20Guide.pdf
*/

#include "DecagonCTD.h"


// The constructor - need the SDI-12 address, the number of readings to average,
// the power pin, and the data pin
DecagonCTD::DecagonCTD(int numReadings, char CTDaddress, int powerPin, int dataPin) : SensorBase(), mySDI12(dataPin)
{
  _numReadings = numReadings;
  _CTDaddress = CTDaddress;
  _powerPin = powerPin;
  _dataPin = dataPin;
  // setup();
}

// The function to set up connection to a sensor.
// This starts the SDI12 and verifies the sensor is connected
SENSOR_STATUS DecagonCTD::setup(void)
{
    pinMode(_dataPin, INPUT);
    pinMode(_powerPin, OUTPUT);
    digitalWrite(_powerPin, HIGH);  // Need power to check connection

    // mySDI12.setDiagStream(Serial);  // For debugging
    mySDI12.begin();
    delay(500); // allow things to settle


    String myCommand = "";
    myCommand = "";
    myCommand += (char) _CTDaddress; // sends basic 'acknowledge' command [address][!]
    myCommand += "!";

    for(int j = 0; j < 3; j++)  // goes through three rapid contact attempts
    {
        mySDI12.sendCommand(myCommand);
        if(mySDI12.available()>1) break;
        delay(30);
    }
    if(mySDI12.available()>2) // if it hears anything it assumes the sensor is there
    {
        mySDI12.flush();
            mySDI12.flush();
            Serial.print(F("Successfully connected to DecagonCTD 5TM at pin "));
            Serial.print(_dataPin);
            Serial.print(F(" and SDI-12 address "));
            Serial.println(_CTDaddress);
        return SENSOR_READY;
    }
    else {   // otherwise it is vacant.
        mySDI12.flush();
        Serial.print(F("Failed to connect to DecagonCTD expected at pin "));
        Serial.print(_dataPin);
        Serial.print(F(" and SDI-12 address "));
        Serial.println(_CTDaddress);
        return SENSOR_ERROR;
    }
    digitalWrite(_powerPin, LOW);  // Turn the power back off.
}

// The function to put the sensor to sleep
bool DecagonCTD::sleep(void)
{
    digitalWrite(_powerPin, LOW);
    mySDI12.flush();
    return true;
}

// The function to wake up the sensor
bool DecagonCTD::wake(void)
{
    digitalWrite(_powerPin, HIGH);
    return true;
}

// The sensor name
String DecagonCTD::getSensorName(void)
{
    sensorName = F("DecagonCTD-10");
    return sensorName;
}

// The sensor installation location on the Mayfly
String DecagonCTD::getSensorLocation(void)
{
    sensorLocation = String(_CTDaddress) + "_" + String(_dataPin);
    return sensorLocation;
}

// The static variables that need to be updated
float DecagonCTD::sensorValue_cond = 0;
float DecagonCTD::sensorValue_temp = 0;
float DecagonCTD::sensorValue_depth = 0;
unsigned long DecagonCTD::sensorLastUpdated;

// Uses SDI-12 to communicate with a Decagon Devices CTD
bool DecagonCTD::update(){

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

    // averages x readings in this one loop
    for (int j = 0; j < _numReadings; j++)
    {
        String command = "";
        command += _CTDaddress;
        command += "M!"; // SDI-12 measurement command format  [address]['M'][!]
        mySDI12.sendCommand(command);
        delay(500); // wait while the measurment is taken.
        // It will return approximately how long it will take to take a measurement
        // We aren't intereted in that number and will let the data flush.
        mySDI12.flush();

        command = "";
        command += _CTDaddress;
        command += "D0!"; // SDI-12 command to get data [address][D][dataOption][!]
        mySDI12.sendCommand(command);
        delay(500);
        if (mySDI12.available() > 0)
        {
            mySDI12.parseFloat();  // First return is the sensor address
            int x = mySDI12.parseInt();  // Depth measurement in millimeters
            float y = mySDI12.parseFloat();  // Temperature measurement in °C
            int z = mySDI12.parseInt();  // Bulk Electrical Conductivity measurement in μS/cm.

            sensorValue_depth += x;
            sensorValue_temp += y;
            sensorValue_cond += z;
        }
        mySDI12.flush();
    }     // end of averaging loop

    float numRead_f = (float) _numReadings;
    sensorValue_depth /= numRead_f ;
    sensorValue_temp /= numRead_f ;
    sensorValue_cond /= numRead_f ;

    DecagonCTD::sensorValue_cond = sensorValue_cond;
    DecagonCTD::sensorValue_temp = sensorValue_temp;
    DecagonCTD::sensorValue_depth = sensorValue_depth;
    DecagonCTD::sensorLastUpdated = millis();


    // Turn the power back off it it had been turned on
    if (wasOff)
        {digitalWrite(_powerPin, LOW);}

    // Return true when finished
    return true;
}




DecagonCTD_Cond::DecagonCTD_Cond(int numReadings, char CTDaddress, int powerPin, int dataPin)
 : DecagonCTD(numReadings, CTDaddress, powerPin, dataPin)
{}

String DecagonCTD_Cond::getVarName(void)
{
    varName = F("specificConductance");
    return varName;
}

String DecagonCTD_Cond::getVarUnit(void)
{
    String unit = F("microsiemenPerCentimeter");
    return unit;
}

float DecagonCTD_Cond::getValue(void)
{
    if (millis() > 30000 and millis() > DecagonCTD::sensorLastUpdated + 30000)
        {DecagonCTD::update();}
    return sensorValue_cond;
}

String DecagonCTD_Cond::getDreamHost(void)
{
String column = F("CTDcond");
return column;
}




DecagonCTD_Temp::DecagonCTD_Temp(int numReadings, char CTDaddress, int powerPin, int dataPin)
 : DecagonCTD(numReadings, CTDaddress, powerPin, dataPin)
{}

String DecagonCTD_Temp::getVarName(void)
{
    varName = F("temperature");
    return varName;
}

String DecagonCTD_Temp::getVarUnit(void)
{
    String unit = F("degreeCelsius");
    return unit;
}

float DecagonCTD_Temp::getValue(void)
{
    if (millis() > 30000 and millis() > DecagonCTD::sensorLastUpdated + 30000)
        {DecagonCTD::update();}
    return sensorValue_temp;
}

String DecagonCTD_Temp::getDreamHost(void)
{
String column = F("CTDtemp");
return column;
}




DecagonCTD_Depth::DecagonCTD_Depth(int numReadings, char CTDaddress, int powerPin, int dataPin)
 : DecagonCTD(numReadings, CTDaddress, powerPin, dataPin)
{}

String DecagonCTD_Depth::getVarName(void)
{
    varName = F("waterDepth");
    return varName;
}

String DecagonCTD_Depth::getVarUnit(void)
{
    String unit = F("millimeter");
    return unit;
}

float DecagonCTD_Depth::getValue(void)
{
    if (millis() > 30000 and millis() > DecagonCTD::sensorLastUpdated + 30000)
        {DecagonCTD::update();}
    return sensorValue_depth;
}

String DecagonCTD_Depth::getDreamHost(void)
{
String column = F("CTDdepth");
return column;
}
