/**
 * @file BoschBME280.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the BoschBME280 class.
 */

#include "BoschBME280.h"


// The constructors
BoschBME280::BoschBME280(TwoWire* theI2C, int8_t powerPin,
                         uint8_t i2cAddressHex, uint8_t measurementsToAverage)
    : Sensor("BoschBME280", BME280_NUM_VARIABLES, BME280_WARM_UP_TIME_MS,
             BME280_STABILIZATION_TIME_MS, BME280_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage),
      _i2cAddressHex(i2cAddressHex),
      _i2c(theI2C) {}

BoschBME280::BoschBME280(int8_t powerPin, uint8_t i2cAddressHex,
                         uint8_t measurementsToAverage)
    : Sensor("BoschBME280", BME280_NUM_VARIABLES, BME280_WARM_UP_TIME_MS,
             BME280_STABILIZATION_TIME_MS, BME280_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage, BME280_INC_CALC_VARIABLES),
      _i2cAddressHex(i2cAddressHex),
      _i2c(&Wire) {}

// Destructor
BoschBME280::~BoschBME280() {}


String BoschBME280::getSensorLocation(void) {
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool BoschBME280::setup(void) {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    // The BME280's begin() reads required calibration data from the sensor.
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    // Run begin fxn because it returns true or false for success in contact
    // Make 5 attempts
    uint8_t ntries  = 0;
    bool    success = false;
    while (!success && ntries < 5) {
        success = bme_internal.begin(_i2cAddressHex, _i2c);
        ntries++;
    }
    if (!success) {
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
        // UN-set the set-up bit (bit 0) since setup failed!
        _sensorStatus &= 0b11111110;
    }
    retVal &= success;

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    return retVal;
}


bool BoschBME280::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;

    // Restart always needed after power-up to set sampling modes
    // As of Adafruit library version 1.0.7, this function includes all of the
    // various delays to allow the chip to wake up, get calibrations, get
    // coefficients, and set sampling modes.
    // This will also restart "Wire"
    // Currently this is using the settings that Adafruit considered to be
    // 'default'
    //  - sensor mode = normal (sensor measures, sleeps for the "standby time"
    //  and then automatically remeasures
    //  - temperature oversampling = 16x
    //  - pressure oversampling = 16x
    //  - humidity oversampling = 16x
    //  - built-in IIR filter = off
    //  - sleep time between measurements = 0.5ms
    // TODO(SRGDamia1):  Figure out why this is necessary; setSampling should be
    // enough and this adds a bunch of small delays...
    bme_internal.begin(_i2cAddressHex, _i2c);

    // When the Adafruit library is updated to remove the built-in delay after
    // forcing a sample, it would be better to operate in forced mode.
    bme_internal.setSampling(
        Adafruit_BME280::MODE_NORMAL,  // sensor mode
        // bme_internal.setSampling(Adafruit_BME280::MODE_FORCED,  // sensor
        // mode
        Adafruit_BME280::SAMPLING_X16,      // temperature oversampling
        Adafruit_BME280::SAMPLING_X16,      //  pressure oversampling
        Adafruit_BME280::SAMPLING_X16,      //  humidity oversampling
        Adafruit_BME280::FILTER_OFF,        // built-in IIR filter
        Adafruit_BME280::STANDBY_MS_1000);  // sleep time between measurements
                                            // (N/A in forced mode)
    delay(100);  // Need this delay after changing sampling mode

    return true;
}


bool BoschBME280::addSingleMeasurementResult(void) {
    bool success = false;

    // Initialize float variables
    float temp  = -9999;
    float humid = -9999;
    float press = -9999;
    float alt   = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        // Read values
        temp = bme_internal.readTemperature();
        if (isnan(temp)) temp = -9999;
        humid = bme_internal.readHumidity();
        if (isnan(humid)) humid = -9999;
        press = bme_internal.readPressure();
        if (isnan(press)) press = -9999;
        alt = bme_internal.readAltitude(SEALEVELPRESSURE_HPA);
        if (isnan(alt)) alt = -9999;

        // Assume that if all three are 0, really a failed response
        // May also return a very negative temp when receiving a bad response
        if ((temp == 0 && press == 0 && humid == 0) || temp < -40) {
            MS_DBG(F("All values 0 or bad, assuming sensor non-response!"));
            temp  = -9999;
            press = -9999;
            humid = -9999;
            alt   = -9999;
        } else {
            success = true;
        }

        MS_DBG(F("  Temperature:"), temp, F("°C"));
        MS_DBG(F("  Humidity:"), humid, F("%RH"));
        MS_DBG(F("  Barometric Pressure:"), press, F("Pa"));
        MS_DBG(F("  Calculated Altitude:"), alt, F("m ASL"));
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(BME280_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(BME280_HUMIDITY_VAR_NUM, humid);
    verifyAndAddMeasurementResult(BME280_PRESSURE_VAR_NUM, press);
    verifyAndAddMeasurementResult(BME280_ALTITUDE_VAR_NUM, alt);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}
