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
MeaSpecMS5803::MeaSpecMS5803(int8_t powerPin, uint8_t i2cAddressHex,
                             int16_t maxPressure, uint8_t measurementsToAverage)
     : Sensor("MeaSpecMS5803", MS5803_NUM_VARIABLES,
              MS5803_WARM_UP_TIME_MS, MS5803_STABILIZATION_TIME_MS, MS5803_MEASUREMENT_TIME_MS,
              powerPin, -1, measurementsToAverage)
{
    _i2cAddressHex = i2cAddressHex;
    _maxPressure = maxPressure;
}
// Destructor
MeaSpecMS5803::~MeaSpecMS5803(){}


String MeaSpecMS5803::getSensorLocation(void)
{
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool MeaSpecMS5803::setup(void)
{
    bool retVal = Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    waitForWarmUp();

    // This doesn't return anything to indicate failure or success, we just have to hope
    MS5803_internal.begin(_i2cAddressHex, _maxPressure);
    MS5803_internal.reset();

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    return retVal;
}


bool MeaSpecMS5803::addSingleMeasurementResult(void)
{
    bool success = false;

    // Initialize float variables
    float temp = -9999;
    float press = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6))
    {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
        // Read values
        // NOTE:  These functions actually include the request to begin
        // a measurement and the wait for said measurement to finish.
        // It's pretty fast (max of 11 ms) so we'll just wait.
        temp = MS5803_internal.getTemperature(CELSIUS, ADC_512);
        press = MS5803_internal.getPressure(ADC_4096);

        if (isnan(temp)) temp = -9999;
        if (isnan(press)) press = -9999;
        if (temp < -50 || temp > 95)  // Range is -40°C to +85°C
        {
            temp = -9999;
            press = -9999;
        }
        if (press == 0)  // Returns 0 when disconnected, which is highly unlikely to be a real value.
        {
            temp = -9999;
            press = -9999;
        }

        MS_DBG(F("  Temperature:"), temp);
        MS_DBG(F("  Pressure:"), press);
    }
    else MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));

    verifyAndAddMeasurementResult(MS5803_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(MS5803_PRESSURE_VAR_NUM, press);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}
