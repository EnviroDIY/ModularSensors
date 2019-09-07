// --------------------------------------
// i2c_scanner
//
// Version 1
//    This program (or code that looks like it)
//    can be found in many places.
//    For example on the Arduino.cc forum.
//    The original author is not know.
// Version 2, Juni 2012, Using Arduino 1.0.1
//     Adapted to be as simple as possible by Arduino.cc user Krodal
// Version 3, Feb 26  2013
//    V3 by louarnold
// Version 4, March 3, 2013, Using Arduino 1.0.3
//    by Arduino.cc user Krodal.
//    Changes by louarnold removed.
//    Scanning addresses changed from 0...127 to 1...119,
//    according to the i2c scanner by Nick Gammon
//    http://www.gammon.com.au/forum/?id=10896
// Version 5, March 28, 2013
//    As version 4, but address scans now to 127.
//    A sensor seems to use address 120.
// Version 6, November 27, 2015.
//    Added waiting for the Leonardo serial communication.
//
//
// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.
//

#include <Arduino.h>
#include <Wire.h>
#include <SoftwareWire.h>  // Testato's Software I2C


const int8_t softwareSDA = 5;     // data in pin
const int8_t softwareSCL = 4;     // data out pin
SoftwareWire softWire(5, 4);

template<typename THEWIRE>
THEWIRE createWire(int8_t sda = -1, int8_t scl = -1){
  return THEWIRE(sda, scl);
}
template<>
TwoWire createWire<TwoWire>(int8_t sda, int8_t scl){
  return Wire;
}


template<typename THEWIRE>
void startWire(THEWIRE i2c){
  i2c.begin();
  // i2c.printStatus(Serial);
}


// void scan(int8_t sda = -1, int8_t scl = -1)
// {
//   THEWIRE i2c = createWire<THEWIRE>(sda, scl);
//   i2c.begin();
template<typename THEWIRE>
void scan(THEWIRE i2c){

  byte error, address;
  int nDevices;

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    i2c.beginTransmission(address);
    error = i2c.endTransmission();

    if (error == 0)
    {
      Serial.print("    I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("    Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found");

}


void setup()
{
  pinMode(22, OUTPUT);

  Serial.begin(115200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
  digitalWrite(22, HIGH);
}


void loop()
{

  Serial.println("Hardware I2C Objects:");
  scan<TwoWire>(Wire);
  Serial.print("Software I2C Objects on SDA=:");
  Serial.print(softwareSDA);
  Serial.print(", SCL=");
  Serial.print(softwareSCL);
  Serial.println(":");
  scan<SoftwareWire>(softWire);
  Serial.println();

  delay(5000);           // wait 5 seconds for next scan
}
