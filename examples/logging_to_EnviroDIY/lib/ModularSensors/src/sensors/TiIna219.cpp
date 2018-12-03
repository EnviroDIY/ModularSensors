/*
 *TiIna219.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
*/

#include "TiIna219.h"


// The constructor - because this is I2C, only need the power pin
TiIna219::TiIna219(int8_t powerPin, uint8_t i2cAddressHex, uint8_t measurementsToAverage)
     : Sensor("TiIna219", INA219_NUM_VARIABLES,
              INA219_WARM_UP_TIME_MS, INA219_STABILIZATION_TIME_MS, INA219_MEASUREMENT_TIME_MS,
              powerPin, -1, measurementsToAverage)
{
    _i2cAddressHex  = i2cAddressHex;
}
// Destructor
TiIna219::~TiIna219(){};


String TiIna219::getSensorLocation(void)
{
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool TiIna219::setup(void)
{
    bool retVal = Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    // The INA219's begin() reads required calibration data from the sensor.
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    waitForWarmUp();

    // Run begin fxn because it returns true or false for success in contact
    // Make 5 attempts
    uint8_t ntries = 0;
    //bool success = false;
    bool success = true;
    //?? nh while (!success and ntries < 5)
    {
        //success = ina219_phy.begin(_i2cAddressHex);
        //success = ina219_phy.begin();
        ina219_phy.begin();
        ntries++;
    }
    //if (!success)
    {
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
        // UN-set the set-up bit (bit 0) since setup failed!
        _sensorStatus &= 0b11111110;
    }
    retVal &= success;

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    return retVal;
}


bool TiIna219::wake(void)
{
    // Sensor::wake() checks if the power pin is on, setup has been successful,
    // and sets the wake timestamp and status bits.  If it returns false,
    // there's no reason to go on.
    if (!Sensor::wake()) return false;

    // Restart always needed after power-up to set sampling modes
    // As of Adafruit library version 1.0.7, this function includes all of the
    // various delays to allow the chip to wake up, get calibrations, get
    // coefficients, and set sampling modes.
    // This will also restart "Wire"
    // Currently this is using the settings that Adafruit considered to be 'default'
    //  - sensor mode = normal (sensor measures, sleeps for the "standby time" and then automatically remeasures
    //  - temperature oversampling = 16x ??nh
    //  - Power(mW) oversampling = 16x ??nh
    //  - BUS_VOLTAGE oversampling = 16x
    //  - built-in IIR filter = off
    //  - sleep time between measurements = 0.5ms
    // TODO:  Figure out why this is necessary; setSampling should be enough
    // this adds a bunch of small delays...
    //ina219_phy.begin(_i2cAddressHex);
    ina219_phy.begin();
    // When the Adafruit library is updated to remove the built-in delay after
    // forcing a sample, it would be better to operate in forced mode.
    #if 0
    ina219_phy.setGain(Adafruit_INA219::MODE_NORMAL,  // sensor mode
    // ina219_phy.setSampling(Adafruit_INA219::MODE_FORCED,  // sensor mode
                             Adafruit_INA219::SAMPLING_X16,  // Current_mA oversampling ??nh
                             Adafruit_INA219::SAMPLING_X16,  // Power_mW oversampling ??nh
                             Adafruit_INA219::SAMPLING_X16,  //  BUS_VOLTAGE oversampling ??nh
                             Adafruit_INA219::FILTER_OFF, // built-in IIR filter
                             Adafruit_INA219::STANDBY_MS_1000);  // sleep time between measurements (N/A in forced mode)
    INA219_CONFIG_GAIN_1_40MV        =       (0x0000),  // Gain 1, 40mV Range
    INA219_CONFIG_GAIN_2_80MV        =       (0x0800),  // Gain 2, 80mV Range
    INA219_CONFIG_GAIN_4_160MV       =       (0x1000),  // Gain 4, 160mV Range
    INA219_CONFIG_GAIN_8_320MV       =       (0x1800),  // Gain 8, 320mV Range
#endif
    delay(100);  // Need this delay after changing sampling mode

    return true;
}


bool TiIna219::addSingleMeasurementResult(void)
{
    bool success = false;

    // Initialize float variables
    float current_mA = -9999;
    float busV_V = -9999;
    float power_mW = -9999;
    //float alt = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6))
    {
        // Read values
        MS_DBG(F("Getting values from INA219"));
        current_mA = ina219_phy.getCurrent_mA();
        if (isnan(current_mA)) current_mA = -9999;
        busV_V = ina219_phy.getBusVoltage_V();
        if (isnan(busV_V)) busV_V = -9999;
        power_mW = ina219_phy.getPower_mW();
        if (isnan(power_mW)) power_mW = -9999;


        // Assume that if all three are 0, really a failed response
        // May also return a very negative temp when receiving a bad response
#if 0
        if ((current_mA == 0  && busV_V == 0 && power_mW == 0))
        {
            MS_DBG(F("All values 0 or bad, assuming sensor non-response!"));
            current_mA =  -9999;
            power_mW = -9999;
            busV_V = -9999;
            alt = -9999;
        }
        else
#endif //0 
        success = true;

        MS_DBG(F("mA, current: "), current_mA);
        MS_DBG(F(" V, BusV: "), busV_V);
        //MS_DBG(F("mW, Power: "), power_mW);
        //MS_DBG(F(" Pa, Calculated Altitude: "), alt, F("m ASL"));
    }
    else MS_DBG(getSensorNameAndLocation(), F(" is not currently measuring!"));

    verifyAndAddMeasurementResult(INA219_CURRENT_MA_VAR_NUM, current_mA);
    verifyAndAddMeasurementResult(INA219_BUS_VOLTAGE_VAR_NUM, busV_V);
    //verifyAndAddMeasurementResult(INA219_POWER_MW_VAR_NUM, power_mW);


    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}
