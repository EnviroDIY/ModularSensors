/*
 *AOSongDHT.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the AOSong Digital-output relative humidity & temperature sensor/modules:
 *DHT11, DHT21(AM2301), and DHT 22 (AM2302).  It is dependent on the Adafruit DHT Library
 *
 *Documentation for the sensor can be found at:
 *http://www.aosong.com/en/products/details.asp?id=117
 *
 * For Relative Humidity:
 *  Resolution is 0.1 % RH
 *  Accuracy is ± 2 % RH
 *  Range is 0 to 100 % RH
 *
 * For Temperature:
 *  Resolution is 0.1°C
 *  Accuracy is ±0.5°C
 *  Range is -40°C to +80°C
 *
 * Warm up/sampling time: 1.7sec
*/

#include "PaleoTerraRedox.h"


// The constructor - need the power pin, data pin, and type of DHT
PaleoTerraRedox::PaleoTerraRedox(int8_t powerPin, int8_t dataPin, int8_t clockPin, uint8_t measurementsToAverage)
    : Sensor(F("PaleoTerraRedox"), PTR_NUM_VARIABLES,
             PTR_WARM_UP_TIME_MS, PTR_STABILIZATION_TIME_MS, PTR_MEASUREMENT_TIME_MS,
             powerPin, dataPin, measurementsToAverage),
    i2c_soft(dataPin, clockPin, 0)
{
    _dataPin = dataPin;
    _clockPin = clockPin;
}


bool PaleoTerraRedox::setup(void)
{
    return Sensor::setup();  // this will set timestamp and status bit
}

String PaleoTerraRedox::getSensorLocation(void)
{
    String sensorLocation = F("pins ");
    sensorLocation += String(_dataPin);
    sensorLocation += F(",");
    sensorLocation += String(_clockPin);
    return sensorLocation;
}


bool PaleoTerraRedox::addSingleMeasurementResult(void)
{
    bool success = false;

    byte res1 = 0;  //Data transfer values
    byte res2 = 0;
    byte res3 = 0;

    float res = 0;  //Calculated voltage in uV

    if (_millisMeasurementRequested > 0)
    {
        i2c_soft.beginTransmission(MCP3421_ADR);
        i2c_soft.write(B10001100);  // initiate conversion, One-Shot mode, 18 bits, PGA x1
        i2c_soft.endTransmission();

        delay(300);

        i2c_soft.requestFrom(MCP3421_ADR);
        res1 = i2c_soft.read();
        res2 = i2c_soft.read();
        res3 = i2c_soft.readLast();
        i2c_soft.endTransmission();

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

    //ADD FAILURE CONDITIONS!!

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
