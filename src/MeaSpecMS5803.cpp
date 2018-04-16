/*
 *MeaSpecMS5803.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Aufdenkampe <aaufdenkampe@limno.com>.
 * with help from Beth Fisher, Evan Host and Bobby Schulz
 *
 *This file is for the Measurement Specialties MS5803 pressure sensor,
  for which is used in SparkFun Pressure Sensor Breakout - MS5803-14BA.
 *It is dependent on the https://github.com/EnviroDIY/MS5803 library, which was
  modified for EnviroDIY_Modular sensors based on a fork
  from the https://github.com/NorthernWidget/MS5803 library, which expanded on
 *https://github.com/sparkfun/SparkFun_MS5803-14BA_Breakout_Arduino_Library *
 *Documentation for the sensor can be found at:
 *https://www.sparkfun.com/products/12909
 *https://cdn.sparkfun.com/datasheets/Sensors/Weather/ms5803_14ba.pdf
 *
 * For Pressure (sensor designed for water pressure):
 *  Resolution is 1 / 0.6 / 0.4 / 0.3 / 0.2 mbar (where 1 mbar = 100 pascals)
 *  at oversampling ratios: 256 / 512 / 1024 / 2048 / 4096, respectively.
 *  Accuracy 0 to +40°C is ±20mbar
 *  Accuracy -40°C to +85°C is ±40mbar
 *  Range is 0 to 14 bar
 *  Long term stability is -20 mbar/yr
 *
 * For Temperature:
 *  Resolution is <0.01°C
 *  Accuracy is ±0.8°C
 *  Range is -40°C to +85°C
 *
 * Sensor takes about 0.5 / 1.1 / 2.1 / 4.1 / 8.22 ms to respond
 *  at oversampling ratios: 256 / 512 / 1024 / 2048 / 4096, respectively.
 * Assume sensor is immediately stable
*/

#include "MeaSpecMS5803.h"


// The constructor - because this is I2C, only need the power pin
MeaSpecMS5803::MeaSpecMS5803(int8_t powerPin, uint8_t i2cAddressHex, uint8_t measurementsToAverage)
     : Sensor(F("MeaSpecMS5803"), MS5803_NUM_VARIABLES,
              MS5803_WARM_UP_TIME_MS, MS5803_STABILIZATION_TIME_MS, MS5803_MEASUREMENT_TIME_MS,
              powerPin, -1, measurementsToAverage)
{
    _i2cAddressHex  = i2cAddressHex;
}


String MeaSpecMS5803::getSensorLocation(void)
{
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool MeaSpecMS5803::setup(void)
{
    MS5803_internal.begin(_i2cAddressHex, 14);
    return Sensor::setup();  // this will set timestamp and status bit
}


bool MeaSpecMS5803::addSingleMeasurementResult(void)
{
    bool success = false;

    // Initialize float variables
    float temp = -9999;
    float press = -9999;

    if (_millisMeasurementRequested > 0)
    {
        // Read values
        float temp = MS5803_internal.getTemperature(CELSIUS, ADC_512);
        float press = MS5803_internal.getPressure(ADC_4096);

        if (isnan(temp)) temp = -9999;
        if (isnan(press)) press = -9999;

        MS_DBG(F("Temperature: "), temp);
        MS_DBG(F("Pressure: "), press);
    }
    else MS_DBG(F("Sensor is not currently measuring!\n"));

    verifyAndAddMeasurementResult(MS5803_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(MS5803_PRESSURE_VAR_NUM, press);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    return success;
}
