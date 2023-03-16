/**
 * @file BoschBMP3xx.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the BoschBMP3xx class.
 */

#include "BoschBMP3xx.h"


// The constructor
BoschBMP3xx::BoschBMP3xx(int8_t powerPin, Mode mode,
                         Oversampling pressureOversample,
                         Oversampling tempOversample, IIRFilter filterCoeff,
                         TimeStandby timeStandby, uint8_t i2cAddressHex)
    : Sensor("BoschBMP3xx", BMP3XX_NUM_VARIABLES, BMP3XX_WARM_UP_TIME_MS,
             BMP3XX_STABILIZATION_TIME_MS, BMP3XX_MEASUREMENT_TIME_MS, powerPin,
             -1, 1, BMP3XX_INC_CALC_VARIABLES),
      _mode(mode),
      _pressureOversampleEnum(pressureOversample),
      _tempOversampleEnum(tempOversample),
      _filterCoeffEnum(filterCoeff),
      _standbyEnum(timeStandby),
      _i2cAddressHex(i2cAddressHex) {}
// Destructor
BoschBMP3xx::~BoschBMP3xx() {}


String BoschBMP3xx::getSensorLocation(void) {
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool BoschBMP3xx::setup(void) {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    // The BMP3XX's begin() reads required calibration data from the sensor.
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    // Set the measurement time based on the oversampling settings and the
    // filtering From 3.9.2 of the datasheet
    // In both forced mode and normal mode the pressure and temperature
    // measurement duration follow the equation:
    // *T<sub>conv</sub>* = 234μs + *pres_en* x (392μs + 2<sup>osr_p</sup> x
    // 2020μs) + *temp_en* x (163μs + 2<sup>osr_t</sup> x 2020μs)
    // With:
    // - *T<sub>conv</sub>* = total conversion time in μs
    // - *pres_en* = "0" or "1", depending of the status of the press_en bit (we
    // will always operate with pressure measurement enabled)
    // - *temp_en* = "0" or "1", depending of the status of the temp_en bit (we
    // will always operate with temperature measurement enabled)
    // - 2<sup>osr_p</sup> = amount of pressure oversampling repetitions
    // - 2<sup>osr_t</sup> = amount of temperature oversampling repetitions

    // For safety, we will add an 18% buffer to the time, which is the largest
    // difference between "typical" and "maximum" measurement times given in
    // table 23 of the datasheet
    // The enum values for oversampling match with the values of osr_p and osr_t
    auto typ_measurementTime_us = static_cast<uint32_t>(
        234 +
        1 * (392 + (pow(2, static_cast<int>(_pressureOversampleEnum))) * 2020) +
        1 * (163 + (pow(2, static_cast<int>(_tempOversampleEnum))) * 2020));
    float max_measurementTime_us = static_cast<float>(typ_measurementTime_us) *
        1.18;
    // Set the sensor measurement time to the safety-factored max time
    _measurementTime_ms =
        static_cast<uint32_t>(ceil(max_measurementTime_us / 1000));
    MS_DBG(F("Expected BMP390 typical measurement time is"),
           typ_measurementTime_us, F("µs with possible max of"),
           max_measurementTime_us, F("µs ="), _measurementTime_ms, F("ms"));

    // Check for some invalid, un-supported, or not-recommended settings
    // NOTE:  Technically, of course, it is possible to use the normal sampling
    // mode and the IIR filter without continuous power, but we would have to
    // re-adjust our measurement procedure and times to give enough samples for
    // the filter to be initialized and I'm not going to support that.  It's
    // just not sensible to work that way.
    if (_powerPin >= 0 && _mode == NORMAL_MODE) {
        MS_DBG(F("WARNING:  BMP388/BMP390 will be used in forced mode!  To use "
                 "in 'normal' (continuous sampling) mode the power must be "
                 "continuously on."));
        _mode = FORCED_MODE;
    }
    if ((_powerPin >= 0 /*|| _mode == FORCED_MODE*/) &&
        _filterCoeffEnum != IIR_FILTER_OFF) {
        MS_DBG(F("WARNING:  BMP388/390's IIR filter is only supported with "
                 "continuous power!  The filter will not be used!"));
        _filterCoeffEnum = IIR_FILTER_OFF;
    }

    // Bosch doesn't recommend high temperature sampling
    if (_tempOversampleEnum != OVERSAMPLING_SKIP &&
        _tempOversampleEnum != OVERSAMPLING_X2) {
        MS_DBG(F("BMP388/390 temperature oversampling higher than 2x is not "
                 "recommended"));
    }


    // convert the standby time enum value into the time between readouts from
    // the BMP's
    // ADC NOTE:  The ADC will return repeated values if the ADC's ODR (output
    // data rate) is set faster than the actual measurement time, given
    // oversampling.
    float _timeStandby_ms = 5.0f * pow(2, static_cast<int>(_standbyEnum));
    // warn if an impossible sampling rate is selected
    if ((_timeStandby_ms < max_measurementTime_us / 1000) &&
        _mode == NORMAL_MODE) {
        MS_DBG(F("The selected standby time of"), _timeStandby_ms,
               F("between ADC samples is less than the expected max of"),
               _measurementTime_ms,
               F("ms needed for temperature and pressure oversampling."));
        // bump up the standby time to a possible value
        while (5.0f * pow(2, static_cast<int>(_standbyEnum)) <
               max_measurementTime_us / 1000) {
            _standbyEnum =
                static_cast<TimeStandby>(static_cast<int>(_standbyEnum) + 1);
#if defined(DEBUGGING_SERIAL_OUTPUT) && defined(MS_DEBUGGING_STD)
            _timeStandby_ms = 5.0f * pow(2, static_cast<int>(_standbyEnum));
#endif
            MS_DBG(_standbyEnum, _timeStandby_ms,
                   static_cast<int>(max_measurementTime_us / 1000));
        }
        MS_DBG(F("A standby time of"), _timeStandby_ms,
               F("ms between reading will be used."));
    }

    // print some notes about the filter initialization time
    // the value of the enum is the power of the number of samples
    if (_filterCoeffEnum != IIR_FILTER_OFF && _mode == NORMAL_MODE) {
        MS_DBG(F("BMP388/390's IIR filter will only be fully initialized"),
               pow(2, static_cast<int>(_filterCoeffEnum)) * _timeStandby_ms,
               F("ms after power on"));
    }
    if (_filterCoeffEnum != IIR_FILTER_OFF && _mode == FORCED_MODE) {
        MS_DBG(
            F("BMP388/390's IIR filter will only be fully initialized after"),
            pow(2, static_cast<int>(_filterCoeffEnum)), F("samples"));
    }

    if (_mode == FORCED_MODE) {
        MS_DBG(
            F("BMP388/390's standby time setting is ignored in forced mode."));
    }

    // Run begin fxn because it returns true or false for success in contact
    // Make 5 attempts
    uint8_t ntries  = 0;
    bool    success = false;
    while (!success && ntries < 5) {
        // This will read coefficients and set the sensor up with default values
        // in sleep mode.
        MS_DBG(F("Attempting to connect to BMP3xx to get the floating point "
                 "trim parameters"));
        success = bmp_internal.begin(_i2cAddressHex);

        // Set up oversampling and filter initialization
        // Using the filter selection recommended for "Weather monitoring
        // (lowest power)" in table 10 of the sensor datasheet

        // Oversampling setting
        MS_DBG(F("Sending BMP3xx oversampling settings"));
        bmp_internal.setTempOversampling(_tempOversampleEnum);
        bmp_internal.setPresOversampling(_pressureOversampleEnum);

        // Coefficient of the filter (in samples)
        MS_DBG(F("Sending BMP3xx IIR Filter settings"));
        bmp_internal.setIIRFilter(_filterCoeffEnum);

        MS_DBG(F("Setting sea level atmospheric pressure to"),
               SEALEVELPRESSURE_HPA);
        bmp_internal.setSeaLevelPressure(SEALEVELPRESSURE_HPA);

        // if we plan to operate in normal mode, set that up and begin sampling
        // at the specified intervals
        // if we're going to operate in forced mode, this isn't needed
        if (_mode == NORMAL_MODE) {
            // Standby time between samples in normal sampling mode - doesn't
            // apply in forced mode
            MS_DBG(F(
                "Sending BMP3xx stand-by time and starting normal conversion"));
            bmp_internal.setTimeStandby(_standbyEnum);
            bmp_internal.startNormalConversion();
        }
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


bool BoschBMP3xx::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;

    // if the power has gone off, we need to re-read the coefficients,
    // we don't need to do anything if always powered.
    // NOTE:  only forced sampling is supported with switched power
    if (_powerPin >= 0) {  // Run begin fxn because it returns true or false
                           // for success in contact
        // Make 5 attempts
        uint8_t ntries  = 0;
        bool    success = false;
        while (!success && ntries < 5) {
            MS_DBG(
                F("Attempting to connect to BMP3xx to get the floating point "
                  "trim parameters"));
            success = bmp_internal.begin(_i2cAddressHex);

            // Set up oversampling and filter initialization
            // Using the filter selection recommended for "Weather monitoring
            // (lowest power)" in table 10 of the sensor datasheet

            // Oversampling setting
            MS_DBG(F("Sending BMP3xx oversampling settings"));
            bmp_internal.setTempOversampling(_tempOversampleEnum);
            bmp_internal.setPresOversampling(_pressureOversampleEnum);

            ntries++;
        }
        if (!success) {
            // Set the status error bit (bit 7)
            _sensorStatus |= 0b10000000;
            // UN-set the set-up bit (bit 0) since setup failed!
            _sensorStatus &= 0b11111110;
        }
        return success;
    }

    return true;
}


// To start a measurement we write the command "R" to the sensor
// NOTE:  documentation says to use a capital "R" but the examples provided
// by Atlas use a lower case "r".
bool BoschBMP3xx::startSingleMeasurement(void) {
    // Sensor::startSingleMeasurement() checks that if it's awake/active and
    // sets the timestamp and status bits.  If it returns false, there's no
    // reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    // we only need to start a measurement in forced mode
    // in "normal" mode, the sensor to automatically alternates between
    // measuring and sleeping at the prescribed intervals
    if (_mode == FORCED_MODE) {
        MS_DBG(F("Starting forced measurement on"), getSensorNameAndLocation());
        // unfortunately, there's no return value here
        bmp_internal.startForcedConversion();
        // Update the time that a measurement was requested
        _millisMeasurementRequested = millis();
    }

    return true;
}


bool BoschBMP3xx::addSingleMeasurementResult(void) {
    bool success = false;

    // Initialize float variables
    float temp  = -9999;
    float press = -9999;
    float alt   = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        // Read values
        success = bmp_internal.getMeasurements(temp, press, alt);

        // Assume that if all three are 0, really a failed response
        // May also return a very negative temp when receiving a bad response
        if (!success) {
            temp  = -9999;
            press = -9999;
            alt   = -9999;
        }

        MS_DBG(F("  Temperature:"), temp, F("°C"));
        MS_DBG(F("  Barometric Pressure:"), press, F("Pa"));
        MS_DBG(F("  Calculated Altitude:"), alt, F("m ASL"));
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(BMP3XX_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(BMP3XX_PRESSURE_VAR_NUM, press);
    verifyAndAddMeasurementResult(BMP3XX_ALTITUDE_VAR_NUM, alt);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}
