/*
 *Decagon5TM.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.

 *
 *This file is for the Decagon Devices 5TM Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library.
*/

#include <Arduino.h>
#include <SDI12_PCINT3.h>
#include "Decagon5TM.h"

// The constructor - need the SDI-12 address, the power pin, and the data pin
Decagon5TM::Decagon5TM(char TMaddress, int powerPin, int dataPin) : SensorBase()
{
  _TMaddress = TMaddress;
  _powerPin = powerPin;
  _dataPin = dataPin;
  setup();
}

// The function to set up connection to a sensor.
SENSOR_STATUS Decagon5TM::setup(void)
{
    pinMode(_powerPin, OUTPUT);
    digitalWrite(_powerPin, LOW);
    return SENSOR_READY;
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

// Uses SDI-12 to communicate with a Decagon Devices CTD
bool Decagon5TM::update(){

  SDI12 mySDI12(_dataPin);

  // Turn on power to the sensor
  delay(500);
  digitalWrite(_powerPin, HIGH);
  delay(1000);

    String command = "";
    sensorValue_Ea = 0.0;
    sensorValue_temp = 0.0;
    sensorValue_VWC = 0.0;
    command += _dataPin;
    command += "M!"; // SDI-12 measurement command format  [address]['M'][!]
    mySDI12.sendCommand(command);
    delay(500); // wait a while
    mySDI12.flush(); // we don't care about what it sends back

    command = "";
    command += _dataPin;
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

  // Prints for debugging
  Serial.print(F("------updated "));
  Serial.print(getSensorName());
  Serial.println(F(" sensor------"));

  // Turn off power to the sensor
  digitalWrite(_powerPin, LOW);
  delay(100);

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
    return sensorValue_Ea;
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
    return sensorValue_temp;
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
    return sensorValue_VWC;
}

String Decagon5TM_VWC::getDreamHost(void)
{
String column = F("soilVWC");
return column;
}
