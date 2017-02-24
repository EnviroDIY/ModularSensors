/*
 *Decagon5TM.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.

 *
 *This file is for the Decagon Devices 5TM Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon 5TM can be found at:
 *http://manuals.decagon.com/Integration%20Guides/5TM%20Integrators%20Guide.pdf
*/

#include "Decagon5TM.h"

// The constructor - need the SDI-12 address, the power pin, and the data pin
Decagon5TM::Decagon5TM(char TMaddress, int powerPin, int dataPin) : SensorBase(), mySDI12(dataPin)
{
  _TMaddress = TMaddress;
  _powerPin = powerPin;
  _dataPin = dataPin;
  Serial.println(F("Constructor for 5TM"));
  // setup();
}

// The function to set up connection to a sensor.
// This starts the SDI12 and verifies the sensor is connected
SENSOR_STATUS Decagon5TM::setup(void)
{
    pinMode(_dataPin, INPUT);
    pinMode(_powerPin, OUTPUT);
    digitalWrite(_powerPin, HIGH);  // Need power to check connection

    // mySDI12.setDiagStream(Serial);  // For debugging
    mySDI12.begin();
    delay(500); // allow things to settle


    String myCommand = "";
    myCommand = "";
    myCommand += (char) _TMaddress; // sends basic 'acknowledge' command [address][!]
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
            Serial.print(F("Successfully connected to Decagon 5TM at pin "));
            Serial.print(_dataPin);
            Serial.print(F(" and SDI-12 address "));
            Serial.println(_TMaddress);
        return SENSOR_READY;
    }
    else {   // otherwise it is vacant.
        mySDI12.flush();
        Serial.print(F("Failed to connect to Decagon 5TM expected at pin "));
        Serial.print(_dataPin);
        Serial.print(F(" and SDI-12 address "));
        Serial.println(_TMaddress);
        return SENSOR_ERROR;
    }
    digitalWrite(_powerPin, LOW);  // Turn the power back off.
}

// The function to put the sensor to sleep
bool Decagon5TM::sleep(void)
{
    digitalWrite(_powerPin, LOW);
    mySDI12.flush();
    return true;
}

// The function to wake up the sensor
bool Decagon5TM::wake(void)
{
    digitalWrite(_powerPin, HIGH);
    return true;
}

// The sensor name
String Decagon5TM::getSensorName(void)
{
    sensorName = F("Decagon5TM");
    return sensorName;
}

// The sensor installation location on the Mayfly
String Decagon5TM::getSensorLocation(void)
{
    sensorLocation = String(_TMaddress) + "_" + String(_dataPin);
    return sensorLocation;
}

// The static variables that need to be updated
float Decagon5TM::sensorValue_Ea = 0;
float Decagon5TM::sensorValue_temp = 0;
float Decagon5TM::sensorValue_VWC = 0;
unsigned long Decagon5TM::sensorLastUpdated;

// Uses SDI-12 to communicate with a Decagon Devices 5TM
bool Decagon5TM::update(){

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
    sensorValue_Ea = 0.0;
    sensorValue_temp = 0.0;
    sensorValue_VWC = 0.0;
    command += _TMaddress;
    command += "M!"; // SDI-12 measurement command format  [address]['M'][!]
    mySDI12.sendCommand(command);
    delay(500); // wait while the measurment is taken.
    // It will return approximately how long it will take to take a measurement
    // We aren't intereted in that number and will let the data flush.
    mySDI12.flush();

    command = "";
    command += _TMaddress;
    command += "D0!"; // SDI-12 command to get data [address][D][dataOption][!]
    mySDI12.sendCommand(command);
    delay(500);

    if(mySDI12.available() > 0)
    {
        mySDI12.parseFloat();
        sensorValue_Ea = mySDI12.parseFloat();
        sensorValue_temp = mySDI12.parseFloat();

        //the TOPP equation used to calculate VWC
        sensorValue_VWC = (4.3e-6*(sensorValue_Ea*sensorValue_Ea*sensorValue_Ea))
                             - (5.5e-4*(sensorValue_Ea*sensorValue_Ea))
                             + (2.92e-2 * sensorValue_Ea)
                             - 5.3e-2 ;

    }
    mySDI12.flush();

  Decagon5TM::sensorValue_Ea = sensorValue_Ea;
  Decagon5TM::sensorValue_temp = sensorValue_temp;
  Decagon5TM::sensorValue_VWC = sensorValue_VWC;
  Decagon5TM::sensorLastUpdated = millis();

  // Turn the power back off it it had been turned on
  if (wasOff)
      {digitalWrite(_powerPin, LOW);}

  // Return true when finished
  return true;
}




Decagon5TM_Ea::Decagon5TM_Ea(char TMaddress, int powerPin, int dataPin)
 : Decagon5TM(TMaddress, powerPin, dataPin)
{}

String Decagon5TM_Ea::getVarName(void)
{
    varName = F("waterPotential");
    return varName;
}

String Decagon5TM_Ea::getVarUnit(void)
{
    String unit = F("kilopascal");
    return unit;
}

float Decagon5TM_Ea::getValue(void)
{
    if (millis() > 30000 and millis() > Decagon5TM::sensorLastUpdated + 30000)
        {Decagon5TM::update();}
    return Decagon5TM::sensorValue_Ea;
}

String Decagon5TM_Ea::getDreamHost(void)
{
String column = F("soilEa");
return column;
}




Decagon5TM_Temp::Decagon5TM_Temp(char TMaddress, int powerPin, int dataPin)
 : Decagon5TM(TMaddress, powerPin, dataPin)
{}

String Decagon5TM_Temp::getVarName(void)
{
    varName = F("temperature");
    return varName;
}

String Decagon5TM_Temp::getVarUnit(void)
{
    String unit = F("degreeCelsius");
    return unit;
}

float Decagon5TM_Temp::getValue(void)
{
    if (millis() > 30000 and millis() > Decagon5TM::sensorLastUpdated + 30000)
        {Decagon5TM::update();}
    return Decagon5TM::sensorValue_temp;
}

String Decagon5TM_Temp::getDreamHost(void)
{
String column = F("soiltemp");
return column;
}




Decagon5TM_VWC::Decagon5TM_VWC(char TMaddress, int powerPin, int dataPin)
 : Decagon5TM(TMaddress, powerPin, dataPin)
{}

String Decagon5TM_VWC::getVarName(void)
{
    varName = F("volumetricWaterContent");
    return varName;
}

String Decagon5TM_VWC::getVarUnit(void)
{
    String unit = F("percent");
    return unit;
}

float Decagon5TM_VWC::getValue(void)
{
    if (millis() > 30000 and millis() > Decagon5TM::sensorLastUpdated + 30000)
        {Decagon5TM::update();}
    return Decagon5TM::sensorValue_VWC;
}

String Decagon5TM_VWC::getDreamHost(void)
{
String column = F("soilVWC");
return column;
}
