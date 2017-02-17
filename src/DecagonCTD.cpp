/*
 *This file is part of the DecagonCTD library for Arduino
 *It is dependent on the EnviroDIY SDI-12 library.
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon CTD-10 can be found at:
 *http://manuals.decagon.com/Integration%20Guides/CTD%20Integrators%20Guide.pdf
*/

#include "DecagonCTD.h"
#include <SDI12_Mod.h>

DecagonCTD::DecagonCTD(void)
  : Sensor<float>("Decagon CTD-10")
  // : Sensor<float>("temperature")
  // : Sensor<float>("waterDepth")
{
    // Do nothing. Not because we can't, but we don't need to.
}


// Uses SDI-12 to communicate with a Decagon Devices CTD
bool DecagonCTD::update(char CTDaddress, int numReadings, int dataPin)
{
  _CTDaddress = CTDaddress;
  _numReadings = numReadings;
  _dataPin = dataPin;

  float sensorValue[3];

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
      float junk = mySDI12.parseFloat();  // First return is the sensor address
      int x = mySDI12.parseInt();  // Depth measurement in millimeters
      float y = mySDI12.parseFloat();  // Temperature measurement in °C
      int z = mySDI12.parseInt();  // Bulk Electrical Conductivity measurement in μS/cm.

      sensorValue[0] += x;
      sensorValue[1] += y;
      sensorValue[2] += z;
    }

    mySDI12.flush();
  }     // end of averaging loop

  float numRead_f = (float) _numReadings;
  sensorValue[0] /= numRead_f ;
  sensorValue[1] /= numRead_f ;
  sensorValue[2] /= numRead_f ;

  return true;
}

String DecagonCTD::getValueAsString()
{
  return String(sensorValue);
}
