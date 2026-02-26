/*
 *PaleoTerraRedox.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library development done by Sara Damiano (sdamiano@stroudcenter.org).
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
#if defined(MS_PALEOTERRA_SOFTWAREWIRE)
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
#if defined(MS_PALEOTERRA_SOFTWAREWIRE)
// If we created a new SoftwareWire instance, we need to destroy it or
// there will be a memory leak
PaleoTerraRedox::~PaleoTerraRedox() {
    if (createdSoftwareWire) delete _i2c;
}
#else
PaleoTerraRedox::~PaleoTerraRedox() {}
#endif


String PaleoTerraRedox::getSensorLocation(void) {
#if defined(MS_PALEOTERRA_SOFTWAREWIRE)
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
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool    success  = false;
    byte    config   = 0;        // Returned config
    int32_t adcValue = 0;        // Raw ADC value from the sensor
    float   res      = -9999.0;  // Calculated voltage in uV

    byte i2c_status;

    _i2c->beginTransmission(_i2cAddressHex);
    // When writing config:
    // - Bit 7: RDY (**1** = initiate conversion, 0 = no effect)
    // - Bit 6-5: No effect on the MCP3421 (set both to 0)
    // - Bit 4: O/C (1 = Continuous mode, **0** = One-Shot mode)
    // - Bits 3-2: S1, S0 (00 = 12-bit/240 SPS, 01 = 14-bit/60 SPS, 10 =
    //   16-bit/15 SPS, **11** = 18-bit/3.75 SPS)
    // - Bits 1-0: G1, G0 (**00** = PGA x1, 01 = PGA x2, 10 = PGA x4, 11 = PGA
    //   x8)
    _i2c->write(
        0b10001100);  // initiate conversion, One-Shot mode, 18 bits, PGA x1
    i2c_status = _i2c->endTransmission();
    // fail if transmission error
    if (i2c_status != 0) { return bumpMeasurementAttemptCount(false); }

    // wait for the conversion to complete
    delay(PTR_CONVERSION_WAIT_TIME_MS);

    // Get 4 bytes from device
    if (_i2c->requestFrom(int(_i2cAddressHex), 4) != 4) {
        return bumpMeasurementAttemptCount(false);
    }
    // per the datasheet, in 18 bit mode:
    // byte 1: [MMMMMM D17 D16 (1st data byte]
    // byte 2: [ D15 ~ D8 (2nd data byte)]
    // byte 3: [ D7 ~ D0 (3rd data byte)]
    // byte 4: [config byte: RDY, C1, C0, O/C, S1, S0, G1, G0]
    // NOTE: D17 is MSB (= sign bit), M is repeated MSB of the data byte.
    byte res1 = _i2c->read();  // byte 1: [MMMMMM D17 D16]
    byte res2 = _i2c->read();  // byte 2: [ D15 ~ D8 ]
    byte res3 = _i2c->read();  // byte 3: [ D7 ~ D0 ]
    config    = _i2c->read();  // byte 4: [config byte]

    res = 0;
    // Assemble the 18-bit raw sample from the three bytes
    // Only use the lower 2 bits of res1 (D17 D16), ignore sign-extension bits
    // Cast to uint32_t to ensure sufficient bit width for left shift operations
    adcValue = static_cast<int32_t>(
        (((uint32_t)(res1 & 0x03))
         << 16)                    // extract D17 D16 and shift to position
        | (((uint32_t)res2) << 8)  // shift res2 up to middle byte
        | ((uint32_t)res3));  // res3 is already in the right place as the LSB

    // Check if this is a negative value (sign bit 17 is set)
    if (res1 & 0x02) {  // Test bit 17
        // Sign-extend the 18-bit value to get correct negative magnitude
        adcValue |= 0xFFFC0000;  // Sign extend from bit 17 (set all bits 18-31)
    }

    // convert the raw ADC value to voltage in microvolts (uV)
    res = adcValue * 0.015625;  // 15.625 uV per LSB

    MS_DBG(F("Raw ADC reading in bits:"), adcValue);
    MS_DBG(F("Config byte:"), config);
    MS_DBG(F("Calculated voltage in uV:"), res);

    success = (!isnan(res)) &&
        !(adcValue == 0 && i2c_status == 0 && config == 0);
    if (success) {
        // Store the results in the sensorValues array
        verifyAndAddMeasurementResult(PTR_VOLTAGE_VAR_NUM, res);
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
