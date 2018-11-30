/*
KellerModbus.cpp

Written by Anthony Aufdenkampe

Tested with Acculevel, Nanolevel (Neil Hancock)
- a Keller Series 30, Class 5, Group 20 sensor
- Software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week)

*/

#include "KellerModbus.h"


//----------------------------------------------------------------------------
//                          PUBLIC SENSOR FUNCTIONS
//----------------------------------------------------------------------------


// This function sets up the communication
// It should be run during the arduino "setup" function.
// The "stream" device must be initialized and begun prior to running this.
bool keller::begin(kellerModel model,byte modbusSlaveID, Stream *stream, int enablePin)
{
    // Give values to variables;
    _slaveID = modbusSlaveID;
    _model = model;
    // Start up the modbus instance
    bool success = modbus.begin(modbusSlaveID, stream, enablePin);
    return success;
}
bool keller::begin(kellerModel model,byte modbusSlaveID, Stream &stream, int enablePin)
{return begin(model,modbusSlaveID, &stream, enablePin);}


// This gets the modbus slave ID.
// For Keller, slaveID is in register 0x020D (525), or regNum = 0x020D
// regType = 0x03 for all Keller Modbus Register Read functions
// NOTE: NOT YET WORKING
byte keller::getSlaveID(void)
{
    return modbus.byteFromRegister(0x03, 0x020D, 2); // byte byteFromRegister(byte regType, int regNum, int byteNum)
}


// This sets a new modbus slave ID
// For Keller, The slaveID is in register 0x020D (525), or regNum = 0x020D
// NOTE: NOT YET TESTED
// bool keller::setSlaveID(byte newSlaveID)
// {
//     return modbus.byteToRegister(0x020D, 2, newSlaveID); //bool byteToRegister(int regNum, int byteNum, byte value, bool forceMultiple=false);
// }


// This gets the instrument serial number as a 32-bit unsigned integer (as specified by Keller)
// For Keller, Serial number is in holding registers 0x0202 and 0x0203 (2304) and occupies 4 registers (8 characters)
long keller::getSerialNumber(void)
{
    return modbus.uint32FromRegister(0x03, 0x0202); // uint32_t uint32FromRegister(byte regType, int regNum, endianness endian=bigEndian);
}


// This gets the hardware and software version of the sensor
// This data begins in holding register 0x020E (??) and continues for 2 registers
// bool keller::getVersion(float &ClassGroup, float &YearWeek)
// {
//     // Parse into version numbers, as a string "Class.Group-Year:Week"
//     // These aren't actually little endian responses.
//     // The first byte is the Class
//     // The second byte is the Group
//     if (modbus.getRegisters(0x03, 0x020E, 2))
//     {
//         ClassGroup = modbus.byteFromFrame(3) + (float)modbus.byteFromFrame(4) / 100;
//         YearWeek = modbus.byteFromFrame(5) + (float)modbus.byteFromFrame(6) / 100;
//         return true;
//     }
//     else return false;
// }

// This returns previously fetched value
bool keller::getValueLastTempC(float &value)
{
    value =  _LastTOB1;
    return true;
}
// This gets values back from the sensor
// Uses Keller Process Value Read Range (0x0100) 32bit floating point,
// which is Same as 0x0000 .. 0x000B but different mapping for accessing data in one cycle (e.g. P1 and TOB1)
// P1 is in register 0x0100 & TOB1 (Temperature of sensor1) is in 0x0102
bool keller::getValues(float &valueP1, float &valueTOB1)
{
    // Set values to -9999 and error flagged before asking for the result
    valueP1   = -9999;  // Pressure (bar) for sensor1
    valueTOB1 = -9999;  // Temperature (C) on board sensor 1

    switch(_model)
    {
        case Nanolevel_kellerModel:  // This gets two values, but as seperate messages
        {
            if (modbus.getRegisters(0x03, 0x0002, 2))
            {
                valueP1 = modbus.float32FromFrame(bigEndian, 3);
                if (modbus.getRegisters(0x03, 0x0006, 2))
                {
                   valueTOB1 = modbus.float32FromFrame(bigEndian, 3);
                   break;
                }
                else return false;
            }
            else return false;
        }
        default:  // for all other sensors get two values in one message
        {
            if (modbus.getRegisters(0x03, 0x0100, 4))
            {
                valueP1 = modbus.float32FromFrame(bigEndian, 3);
                valueTOB1 = modbus.float32FromFrame(bigEndian, 7);
                break;
            }
            else return false;
        }
    }
    
    _LastTOB1 = valueTOB1;
    return true;
}

float keller::calcWaterDepthM(float &waterPressureBar, float &waterTempertureC)
{
    /// Initialize variables
    float waterPressurePa; // in Pascals (kg/m/s2)
    float waterDensity;    // in kmg/m2
    float waterDepthM;     // in m
    const float gravitationalConstant = 9.80665; // m/s2, meters per second squared

    if (waterPressureBar == -9999)
    {
        waterDepthM = -9999;  // error or sensor not connected
    }
    else
    {
        waterPressurePa = 1e5 * waterPressureBar;
        // Water density (kg/m3) from equation 6 from JonesHarris1992-NIST-DensityWater.pdf
        waterDensity =  + 999.84847
                        + 6.337563e-2 * waterTempertureC
                        - 8.523829e-3 * pow(waterTempertureC,2)
                        + 6.943248e-5 * pow(waterTempertureC,3)
                        - 3.821216e-7 * pow(waterTempertureC,4)
                        ;
        waterDepthM = waterPressurePa/(waterDensity * gravitationalConstant);  // from P = rho * g * h
    }

    return waterDepthM;
}
