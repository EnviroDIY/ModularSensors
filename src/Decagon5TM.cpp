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
}

// The function to set up connection to a sensor.
SENSOR_STATUS Decagon5TM::setup(void)
{
    pinMode(_dataPin, INPUT);
    pinMode(_powerPin, OUTPUT);
    digitalWrite(_powerPin, LOW);
    return SENSOR_READY;
}

// The function to put the sensor to sleep
bool Decagon5TM::sleep(void)
{
    digitalWrite(_powerPin, LOW);
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

  SDI12 TMSDI12(_dataPin);
  // TMSDI12.setDiagStream(Serial);  // For debugging
  TMSDI12.begin();
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

    String command = "";
    sensorValue_Ea = 0.0;
    sensorValue_temp = 0.0;
    sensorValue_VWC = 0.0;
    command += _TMaddress;
    command += "M!"; // SDI-12 measurement command format  [address]['M'][!]
    TMSDI12.sendCommand(command);
    delay(500); // wait a while
    TMSDI12.flush(); // we don't care about what it sends back

    command = "";
    command += _TMaddress;
    command += "D0!"; // SDI-12 command to get data [address][D][dataOption][!]
    TMSDI12.sendCommand(command);
    delay(500);

       if(TMSDI12.available() > 0)
       {
        //   Serial.println(F("Recieving data"));
          TMSDI12.parseFloat();
          sensorValue_Ea = TMSDI12.parseFloat();
          sensorValue_temp = TMSDI12.parseFloat();

          //the TOPP equation used to calculate VWC
          sensorValue_VWC = (4.3e-6*(sensorValue_Ea*sensorValue_Ea*sensorValue_Ea))
                                 - (5.5e-4*(sensorValue_Ea*sensorValue_Ea))
                                 + (2.92e-2 * sensorValue_Ea)
                                 - 5.3e-2 ;

       }
        TMSDI12.flush();

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
