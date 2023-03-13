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


// Constructors
#if defined MS_PALEOTERRA_SOFTWAREWIRE
PaleoTerraRedox::PaleoTerraRedox(SoftwareWire* theI2C, int8_t powerPin,
                                 uint8_t i2cAddressHex,
                                 uint8_t measurementsToAverage)
    : Sensor("PaleoTerraRedox", PTR_NUM_VARIABLES, PTR_WARM_UP_TIME_MS,
             PTR_STABILIZATION_TIME_MS, PTR_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage) {
    _i2cAddressHex      = i2cAddressHex;
    _i2c                = theI2C;
    createdSoftwareWire = false;
}
PaleoTerraRedox::PaleoTerraRedox(int8_t powerPin, int8_t dataPin,
                                 int8_t clockPin, uint8_t i2cAddressHex,
                                 uint8_t measurementsToAverage)
    : Sensor("PaleoTerraRedox", PTR_NUM_VARIABLES, PTR_WARM_UP_TIME_MS,
             PTR_STABILIZATION_TIME_MS, PTR_MEASUREMENT_TIME_MS, powerPin,
             dataPin, measurementsToAverage) {
    _i2cAddressHex      = i2cAddressHex;
    _i2c                = new SoftwareWire(dataPin, clockPin);
    createdSoftwareWire = true;
}
#else
PaleoTerraRedox::PaleoTerraRedox(TwoWire* theI2C, int8_t powerPin,
                                 uint8_t i2cAddressHex,
                                 uint8_t measurementsToAverage)
    : Sensor("PaleoTerraRedox", PTR_NUM_VARIABLES, PTR_WARM_UP_TIME_MS,
             PTR_STABILIZATION_TIME_MS, PTR_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage),
      _i2cAddressHex(i2cAddressHex),
      _i2c(theI2C) {}
PaleoTerraRedox::PaleoTerraRedox(int8_t powerPin, uint8_t i2cAddressHex,
                                 uint8_t measurementsToAverage)
    : Sensor("PaleoTerraRedox", PTR_NUM_VARIABLES, PTR_WARM_UP_TIME_MS,
             PTR_STABILIZATION_TIME_MS, PTR_MEASUREMENT_TIME_MS, powerPin,
             measurementsToAverage, PTR_INC_CALC_VARIABLES),
      _i2cAddressHex(i2cAddressHex),
      _i2c(&Wire) {}
#endif


// Destructors
#if defined MS_PALEOTERRA_SOFTWAREWIRE
// If we created a new SoftwareWire instance, we need to destroy it or
// there will be a memory leak
PaleoTerraRedox::~PaleoTerraRedox() {
    if (createdSoftwareWire) delete _i2c;
}
#else
PaleoTerraRedox::~PaleoTerraRedox() {}
#endif


String      PaleoTerraRedox::getSensorLocation(void) {
#if defined MS_PALEOTERRA_SOFTWAREWIRE
    String address = F("SoftwareWire");
    if (_dataPin >= 0) address += _dataPin;
    address += F("_0x");
#else
    String address = F("I2C_0x");
#endif
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool PaleoTerraRedox::setup(void) {
    _i2c->begin();  // Start the wire library (sensor power not required)
    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    _i2c->setTimeout(0);
    return Sensor::setup();  // this will set pin modes and the setup status
                             // bit
}


bool PaleoTerraRedox::addSingleMeasurementResult(void) {
    bool success = false;

    byte config = 0;  // Data transfer values

    float res = 0;  // Calculated voltage in uV

    byte i2c_status = -1;
    if (_millisMeasurementRequested > 0) {
        _i2c->beginTransmission(_i2cAddressHex);
        _i2c->write(0b10001100);  // initiate conversion, One-Shot mode, 18
                                  // bits, PGA x1
        i2c_status = _i2c->endTransmission();

        delay(300);

        _i2c->requestFrom(int(_i2cAddressHex),
                          4);  // Get 4 bytes from device
        byte res1 = _i2c->read();
        byte res2 = _i2c->read();
        byte res3 = _i2c->read();
        config    = _i2c->read();

        res      = 0;
        int sign = bitRead(res1, 1);  // one but least significant bit
        if (sign == 1) {
            res1 = ~res1;
            res2 = ~res2;
            res3 = ~res3;  // two's complements
            res  = bitRead(res1, 0) *
                -1024;  // 256 * 256 * 15.625 uV per LSB = 16
            res -= res2 * 4;
            res -= res3 * 0.015625;
            res -= 0.015625;
        } else {
            res = bitRead(res1, 0) *
                1024;  // 256 * 256 * 15.625 uV per LSB = 16
            res += res2 * 4;
            res += res3 * 0.015625;
        }
    } else {
        MS_DBG(F("Sensor is not currently measuring!\n"));
    }

    // ADD FAILURE CONDITIONS!!
    if (isnan(res))
        res = -9999;  // list a failure if the sensor returns nan (not sure
                      // how this would happen, keep to be safe)
    else if (res == 0 && i2c_status == 0 && config == 0)
        res = -9999;  // List a failure when the sensor is not connected
    // Store the results in the sensorValues array
    verifyAndAddMeasurementResult(PTR_VOLTAGE_VAR_NUM, res);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    return success;
}
