/*
 *Initial library developement by Sara Damiano (sdamiano@stroudcenter.org).
 *
*/

#include "AtlasScientificCond.h"
// #include "AtlasScientificRTD.h"
#include <Wire.h>


// The constructor - because this is I2C, only need the power pin
// This sensor has a set I2C address of 0X64, or 100
AtlasScientificCond::AtlasScientificCond(int8_t powerPin, uint8_t i2cAddressHex, uint8_t measurementsToAverage)
    : Sensor("AtlasScientificCond", ATLAS_COND_NUM_VARIABLES,
             ATLAS_COND_WARM_UP_TIME_MS, ATLAS_COND_STABILIZATION_TIME_MS, ATLAS_COND_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage),
      _i2cAddressHex(i2cAddressHex)
{}
AtlasScientificCond::~AtlasScientificCond(){}


String AtlasScientificCond::getSensorLocation(void){return F("I2C_0x64");}


bool AtlasScientificCond::setup(void)
{
    Wire.begin();  // Start the wire library (sensor power not required)
    return Sensor::setup();  // this will set pin modes and the setup status bit
}

bool AtlasScientificCond::addSingleMeasurementResult(void) {
    byte code=0;                     //used to hold the I2C response code.
    char Cond_data[20];               //we make a 20 byte character array to hold incoming data from the Cond circuit.
    byte in_char=0;                  //used as a 1 byte buffer to store in bound bytes from the Cond Circuit.
    byte i=0;                        //counter used for Cond_data array.
    int time_=600;                   //used to change the delay needed depending on the command sent to the EZO Class Cond Circuit.

    Wire.beginTransmission(_i2cAddressHex); //call the circuit by its ID number.
    Wire.write('r');                    //transmit the command that was sent through the serial port.
    Wire.endTransmission();             //end the I2C data transmission.

    delay(time_);                       //wait the correct amount of time for the circuit to complete its instruction.

    Wire.requestFrom(_i2cAddressHex,20,1);  //call the circuit and request 20 bytes (this may be more than we need)
    code=Wire.read();                   //the first byte is the response code, we read this separately.

    switch (code){                      //switch case based on what the response code is.
        case 1:                         //decimal 1.
            Serial.println("Success");  //means the command was successful.
        break;                          //exits the switch case.

        case 2:                         //decimal 2.
            Serial.println("Failed");   //means the command has failed.
        break;                          //exits the switch case.

        case 254:                       //decimal 254.
            Serial.println("Pending");  //means the command has not yet been finished calculating.
        break;                          //exits the switch case.

        case 255:                       //decimal 255.
            Serial.println("No Data");  //means there is no further data to send.
        break;                          //exits the switch case.
        }

    while(Wire.available()){            //are there bytes to receive.
        in_char = Wire.read();          //receive a byte.
        Cond_data[i]= in_char;           //load this byte into our array.
        i+=1;                           //incur the counter for the array element.
            if(in_char==0){             //if we see that we have been sent a null command.
                i=0;                    //reset the counter i to 0.

                Wire.endTransmission(); //end the I2C data transmission.
                break;                  //exit the while loop.
            }
    }

Serial.println(Cond_data);          //print the data.

Wire.beginTransmission(_i2cAddressHex); //call the circuit by its ID number.
Wire.write("Sleep");                    //transmit the command that was sent through the serial port.
Wire.endTransmission();             //end the I2C data transmission.

//   return atof(Cond_data);

float tmp_float = atof(Cond_data);

verifyAndAddMeasurementResult(ATLAS_COND_VAR_NUM, tmp_float);

// Unset the time stamp for the beginning of this measurement
_millisMeasurementRequested = 0;
// Unset the status bits for a measurement request (bits 5 & 6)
 _sensorStatus &= 0b10011111;

// return true;
}
