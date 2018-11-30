/*
KellerModbus.h

Written by Anthony Aufdenkampe Modified Neil Hancock

Tested with Acculevel, Nanolevel
- a Keller Series 30, Class 5, Group 20 sensor
- Software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week)

*/

#ifndef KellerModbus_h
#define KellerModbus_h

#include <Arduino.h>
#include <SensorModbusMaster.h>

// The various Keller sensors.
// NOTE: not presently used but in place for future. See use in https://github.com/EnviroDIY/YosemitechModbus
typedef enum kellerModel
{
    KellerAcculevel = 0,
    Nanolevel_kellerModel = 1,
    OTHER   // Use if the sensor model is another model.
} kellerModel;

class keller
{

public:

    // This function sets up the communication
    // It should be run during the arduino "setup" function.
    // The "stream" device must be initialized prior to running this.
    bool begin(kellerModel model, byte modbusSlaveID, Stream *stream, int enablePin = -1);
    bool begin(kellerModel model, byte modbusSlaveID, Stream &stream, int enablePin = -1);

    // This gets the modbus slave ID.
    // NOTE: NOT YET WORKING
    byte getSlaveID(void);

    // This sets a new modbus slave ID
    // NOTE: NOT YET WORKING
//    bool setSlaveID(byte newSlaveID);

    // This gets the instrument serial number as a long integer
    long getSerialNumber(void);

    // This gets the hardware and software version of the sensor
    // The float variables for the hardware and software versions must be
    // initialized prior to calling this function.
    // The reference (&) is needed when declaring this function so that
    // the function is able to modify the actual input floats rather than
    // create and destroy copies of them.
    // There is no need to add the & when actually using the function.
    // NOTE: NOT YET WORKING
//   bool getVersion(float &hardwareVersion, float &softwareVersion);


    // This gets values back from the sensor
    bool getValues(float &valueP1, float &valueTOB1);
    bool getValueLastTempC(float &value);
//    bool getValues(float &parmValue, float &tempValue, byte &errorCode);

    float calcWaterDepthM(float &waterPressureBar, float &waterTempertureC);

    // This sets a stream for debugging information to go to;
    void setDebugStream(Stream *stream){modbus.setDebugStream(stream);}
    void stopDebugging(void){modbus.stopDebugging();}


private:
    byte _model;
    byte _slaveID;
    float _LastTOB1;

    modbusMaster modbus;
};

#endif
