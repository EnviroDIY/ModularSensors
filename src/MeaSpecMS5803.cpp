/*
*MeaSpecMS5803.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Aufdenkampe <aaufdenkampe@limno.com>.
 * with help from Beth Fisher and Evan Host
 *
 *This file is for the Measurement Specialties MS5803-14BA pressure sensor,
  as in SparkFun Pressure Sensor Breakout - MS5803-14BA, which uses the .
 *It is dependent on the SparkFun_MS5803-14BA_Breakout_Arduino_Library
 *https://github.com/sparkfun/SparkFun_MS5803-14BA_Breakout_Arduino_Library
 *
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


SENSOR_STATUS MeaSpecMS5803::getStatus(void)
{
    // Check if the power is on, turn it on if not (Need power to get status)
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    // Wait until the sensor is warmed up
    waitForWarmUp();

    // Run begin fxn because it returns true or false for success in contact
    bool status = MS5803_internal.begin(_i2cAddressHex);

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    if (!status) return SENSOR_ERROR;
    else return SENSOR_READY;
}


SENSOR_STATUS MeaSpecMS5803::setup(void)
{
    SENSOR_STATUS setup = Sensor::setup();
    SENSOR_STATUS stat = getStatus();
    if (setup == SENSOR_READY && stat == SENSOR_READY) return SENSOR_READY;
    else return SENSOR_ERROR;
}


bool MeaSpecMS5803::wake(void)
{
    Sensor::wake();
    waitForWarmUp();
    // Restart always needed after power-up

    MS5803_internal.begin(_i2cAddressHex);

    // When the ???? library is updated to remove the built-in delay after
    // forcing a sample, it would be better to operate in forced mode.
    MS5803_internal.setSampling(SparkFun_MS5803_I2C::MODE_NORMAL,  // sensor mode
    // MS5803_internal.setSampling(SparkFun_MS5803_I2C::MODE_FORCED,  // sensor mode
                             SparkFun_MS5803_I2C::SAMPLING_X16,  // temperature oversampling
                             SparkFun_MS5803_I2C::SAMPLING_X16,  //  pressure oversampling
                             SparkFun_MS5803_I2C::SAMPLING_X16,  //  humidity oversampling
                             SparkFun_MS5803_I2C::FILTER_OFF, // built-in IIR filter
                             SparkFun_MS5803_I2C::STANDBY_MS_1000);  // sleep time between measurements (N/A in forced mode)
    delay(100);  // Need this delay after changing sampling mode

    // Mark that the sensor is now active
    _millisSensorActivated = millis();

    return true;
}

// For operating in forced mode
// bool MeaSpecMS5803::startSingleMeasurement(void)
// {
//     // waitForWarmUp();  // already done in wake
//     waitForStability();
//     MS5803_internal.takeForcedMeasurement(false);  // Don't want to wait to finish here
//     _millisMeasurementRequested = millis();
//     return true;
// }


bool MeaSpecMS5803::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a new reading to be available
    waitForMeasurementCompletion();

    // Read values
    float temp = MS5803_internal.readTemperature();
    float press = MS5803_internal.readPressure();
    float alt = MS5803_internal.readAltitude(SEALEVELPRESSURE_HPA);
    float humid = MS5803_internal.readHumidity();

    if (isnan(temp)) temp = -9999;
    if (isnan(press)) press = -9999;
    if (isnan(alt)) alt = -9999;
    if (isnan(humid)) humid = -9999;

    if (temp == -140.85)  // This is the value returned if it's not attached
    {
        temp = press = alt = humid = -9999;
    }

    MS_DBG(F("Temperature: "), temp);
    MS_DBG(F(" Humidity: "), humid);
    MS_DBG(F(" Barometric Pressure: "), press);
    MS_DBG(F(" Calculated Altitude: "), alt, F("\n"));

    verifyAndAddMeasurementResult(MS5803_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(MS5803_HUMIDITY_VAR_NUM, humid);
    verifyAndAddMeasurementResult(MS5803_PRESSURE_VAR_NUM, press);
    verifyAndAddMeasurementResult(MS5803_ALTITUDE_VAR_NUM, alt);

    // Mark that we've already recorded the result of the measurement
    _millisMeasurementRequested = 0;

    // Return true when finished
    return true;
}
