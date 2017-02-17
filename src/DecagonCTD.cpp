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

#include <Arduino.h>
#include "DecagonCTD.h"
#include <SDI12_Mod.h>

// The constructor - need the SDI-12 address, the number of readings to average, and the datapin
DecagonCTD::DecagonCTD(char CTDaddress, int numReadings, int dataPin)
{
  _CTDaddress = CTDaddress;
  _numReadings = numReadings;
  _dataPin = dataPin;
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

// Uses SDI-12 to communicate with a Decagon Devices CTD
bool DecagonCTD::update(){

  SDI12 mySDI12(_dataPin);

  for (int j = 0; j < _numReadings; j++) {   //averages x readings in this one loop

    String command = "";
    command += _CTDaddress;
    command += "M!"; // SDI-12 measurement command format  [address]['M'][!]
    mySDI12.sendCommand(command);
    delay(500); // wait a while
    mySDI12.flush(); // we don't care about what it sends back

    command = "";
    command += _CTDaddress;
    command += "D0!"; // SDI-12 command to get data [address][D][dataOption][!]
    mySDI12.sendCommand(command);
    delay(500);
    if (mySDI12.available() > 0) {
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

  return true;
}




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
    return sensorValue_cond;
}




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
    return sensorValue_temp;
}




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
    return sensorValue_depth;
}
