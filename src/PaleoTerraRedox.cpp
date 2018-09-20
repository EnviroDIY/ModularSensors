/*
 *PaleoTerraRedox.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the I2C Redox sensors made by Paleo Terra.  Because these
 *sensors all ship with the same I2C address, this module is also dependent on
 *a software I2C library to allow the use of multiple sensors.
 *
 *Documentation for the sensor can be found at:
 *https://paleoterra.nl/
*/

#include "PaleoTerraRedox.h"


// The constructor for software mode- need the power pin, data pin, and type of DHT
PaleoTerraRedox::PaleoTerraRedox(int8_t powerPin, int8_t dataPin, int8_t clockPin, uint8_t measurementsToAverage)
    : Sensor("PaleoTerraRedox", PTR_NUM_VARIABLES,
             PTR_WARM_UP_TIME_MS, PTR_STABILIZATION_TIME_MS, PTR_MEASUREMENT_TIME_MS,
             powerPin, dataPin, measurementsToAverage),
    i2c_soft(dataPin, clockPin, 0)
{
    _dataPin = dataPin;
    _clockPin = clockPin;
}

// The constructor for hardware mode- need the power pin, and ADR
PaleoTerraRedox::PaleoTerraRedox(int8_t powerPin, uint8_t ADR, uint8_t measurementsToAverage)
    : Sensor("PaleoTerraRedox", PTR_NUM_VARIABLES,
             PTR_WARM_UP_TIME_MS, PTR_STABILIZATION_TIME_MS, PTR_MEASUREMENT_TIME_MS,
             powerPin, measurementsToAverage)
{
    Wire.begin();
    _ADR = ADR; //Copy slave address
    HardwareI2C = true; //Set hardware flag
}


String PaleoTerraRedox::getSensorLocation(void)
{
    String sensorLocation = "";
    if(HardwareI2C) {  //If using hardware, return address
        sensorLocation = F("I2C_0x");
        sensorLocation += String(_ADR, HEX);
    }

    else{  //If using software, return pins
        sensorLocation = F("I2C");
        sensorLocation += String(_dataPin);
        sensorLocation += F(",");
        sensorLocation += String(_clockPin);
    }
    return sensorLocation;
}


bool PaleoTerraRedox::addSingleMeasurementResult(void)
{
    bool success = false;

    byte res1 = 0;  //Data transfer values
    byte res2 = 0;
    byte res3 = 0;
    byte config = 0;

    float res = 0;  //Calculated voltage in uV

    byte i2c_status = -1;
    if (_millisMeasurementRequested > 0)
    {
        if(HardwareI2C) {
            Wire.beginTransmission(_ADR);
            Wire.write(B10001100);  // initiate conversion, One-Shot mode, 18 bits, PGA x1
            i2c_status = Wire.endTransmission();

            delay(300);

            Wire.requestFrom(_ADR, 4); //Get 4 bytes from device
            res1 = Wire.read();
            res2 = Wire.read();
            res3 = Wire.read();
            config = Wire.read();
        }

        else {
            i2c_status = i2c_soft.beginTransmission(MCP3421_ADR);
            i2c_soft.write(B10001100);  // initiate conversion, One-Shot mode, 18 bits, PGA x1
            i2c_soft.endTransmission();

            delay(300);

            i2c_soft.requestFrom(MCP3421_ADR);
            res1 = i2c_soft.read();
            res2 = i2c_soft.read();
            res3 = i2c_soft.read();
            config = i2c_soft.readLast();
            i2c_soft.endTransmission();
        }

        res = 0;
        int sign = bitRead(res1,1); // one but least significant bit
        if (sign==1){
            res1 = ~res1; res2 = ~res2; res3 = ~res3; // two's complements
            res = bitRead(res1,0) * -1024; // 256 * 256 * 15.625 uV per LSB = 16
            res -= res2 * 4;
            res -= res3 * 0.015625;
            res -= 0.015625;
            } else {
            res = bitRead(res1,0) * 1024; // 256 * 256 * 15.625 uV per LSB = 16
            res += res2 * 4;
            res += res3 * 0.015625;
        }
    }
    else MS_DBG(F("Sensor is not currently measuring!\n"));

    // ADD FAILURE CONDITIONS!!
    if(isnan(res)) res = -9999;  // list a failure if the sensor returns nan (not sure how this would happen, keep to be safe)
    else if(res == 0 && i2c_status == 0 && config == 0) res = -9999;  // List a failure when the sensor is not connected
    // Store the results in the sensorValues array
    verifyAndAddMeasurementResult(PTR_VOLT_VAR_NUM, res);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    return success;
}
