/**
 * @file TEConnectivityMS5837.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the TEConnectivityMS5837 class.
 */

#include "TEConnectivityMS5837.h"


// Primary implementation constructor using type-safe enum
TEConnectivityMS5837::TEConnectivityMS5837(TwoWire* theI2C, int8_t powerPin,
                                           MS5837Model model,
                                           uint8_t     measurementsToAverage,
                                           uint16_t    overSamplingRatio,
                                           float       fluidDensity,
                                           float       airPressure)
    : Sensor("TEConnectivityMS5837", MS5837_NUM_VARIABLES,
             MS5837_WARM_UP_TIME_MS, MS5837_STABILIZATION_TIME_MS,
             MS5837_MEASUREMENT_TIME_MS, powerPin, -1, measurementsToAverage,
             MS5837_INC_CALC_VARIABLES),
      MS5837_internal(theI2C),
      _wire(theI2C),
      _model(static_cast<uint8_t>(model)),
      _fluidDensity(fluidDensity),
      _airPressure(airPressure),
      _overSamplingRatio(overSamplingRatio) {}

// Delegating constructors
TEConnectivityMS5837::TEConnectivityMS5837(TwoWire* theI2C, int8_t powerPin,
                                           uint8_t  model,
                                           uint8_t  measurementsToAverage,
                                           uint16_t overSamplingRatio,
                                           float    fluidDensity,
                                           float    airPressure)
    : TEConnectivityMS5837(theI2C, powerPin, static_cast<MS5837Model>(model),
                           measurementsToAverage, overSamplingRatio,
                           fluidDensity, airPressure) {}

TEConnectivityMS5837::TEConnectivityMS5837(int8_t powerPin, MS5837Model model,
                                           uint8_t  measurementsToAverage,
                                           uint16_t overSamplingRatio,
                                           float    fluidDensity,
                                           float    airPressure)
    : TEConnectivityMS5837(&Wire, powerPin, model, measurementsToAverage,
                           overSamplingRatio, fluidDensity, airPressure) {}

TEConnectivityMS5837::TEConnectivityMS5837(int8_t powerPin, uint8_t model,
                                           uint8_t  measurementsToAverage,
                                           uint16_t overSamplingRatio,
                                           float    fluidDensity,
                                           float    airPressure)
    : TEConnectivityMS5837(&Wire, powerPin, static_cast<MS5837Model>(model),
                           measurementsToAverage, overSamplingRatio,
                           fluidDensity, airPressure) {}

// Destructor
TEConnectivityMS5837::~TEConnectivityMS5837() {}


String TEConnectivityMS5837::getSensorName(void) {
    String modelStr = F("TEConnectivityMS5837_");
    switch (_model) {
        case static_cast<uint8_t>(MS5837Model::MS5837_02BA):
            modelStr += F("02BA");
            break;
        case static_cast<uint8_t>(MS5837Model::MS5837_30BA):
            modelStr += F("30BA");
            break;
        case static_cast<uint8_t>(MS5837Model::MS5803_01BA):
            modelStr += F("01BA");
            break;
        default: modelStr += F("Unknown"); break;
    }
    return modelStr;
}


String TEConnectivityMS5837::getSensorLocation(void) {
    return F("I2C_0x76");
}


bool TEConnectivityMS5837::setup(void) {
    bool success =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    delay(10);
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    // Set the sensor model and initialize the sensor
    success &= MS5837_internal.begin(_model);

    // Validate that the pressure range is reasonable for the sensor model and
    // change the model if possible based on the pressure sensitivity read from
    // the sensor.
    if (validateAndCorrectModel()) {
        // If the model was changed, we need to re-initialize the sensor with
        // the new model.
        success &= MS5837_internal.reset(_model);
    }

    if (success) {
        // Set the fluid density for depth calculations
        MS5837_internal.setDensity(_fluidDensity);
    }

    // Turn the power back off if it had been turned on
    if (!wasOn) { powerDown(); }

    if (!success) {
        MS_DBG(getSensorNameAndLocation(), F("Failed to initialize sensor"));
        // Set the status error bit (bit 7)
        setStatusBit(ERROR_OCCURRED);
        // UN-set the set-up bit (bit 0) since setup failed!
        clearStatusBit(SETUP_SUCCESSFUL);
    }

    return success;
}


bool TEConnectivityMS5837::wake(void) {
    // Run the parent wake function
    if (!Sensor::wake()) return false;

    bool success = true;
    // Re-initialize the sensor communication, if the sensor was powered down
    if (_powerPin >= 0) {
        success = MS5837_internal.begin(_model);
        // There's no need to validate the model or change the fluid density or
        // other parameters. Those are not affected by power cycling the sensor.
    }
    if (!success) {
        MS_DBG(getSensorNameAndLocation(),
               F("Wake failed - sensor re-initialization failed"));
        // Set the status error bit (bit 7)
        setStatusBit(ERROR_OCCURRED);
        // Make sure that the wake time and wake success bit (bit 4) are
        // unset
        _millisSensorActivated = 0;
        clearStatusBit(WAKE_SUCCESSFUL);
    }

    return success;
}


bool TEConnectivityMS5837::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    // Validate configuration parameters
    if (_fluidDensity <= 0.0 || _fluidDensity > 5.0) {
        MS_DBG(F("Invalid fluid density:"), _fluidDensity,
               F("g/cm³. Expected range: 0.0-5.0"));
        return bumpMeasurementAttemptCount(false);
    }
    if (_airPressure < 500.0 || _airPressure > 1200.0) {
        MS_DBG(F("Invalid air pressure:"), _airPressure,
               F("mBar. Expected range: 500-1200"));
        return bumpMeasurementAttemptCount(false);
    }
    if (_overSamplingRatio != 256 && _overSamplingRatio != 512 &&
        _overSamplingRatio != 1024 && _overSamplingRatio != 2048 &&
        _overSamplingRatio != 4096 && _overSamplingRatio != 8192) {
        MS_DBG(F("Invalid oversampling ratio:"), _overSamplingRatio,
               F(". Valid values: 256, 512, 1024, 2048, 4096, 8192"));
        return bumpMeasurementAttemptCount(false);
    }

    float temp  = -9999;
    float press = -9999;
    float depth = -9999;
    float alt   = -9999;


    // Read values from the sensor - returns 0 on success
    int OSR;
    switch (_overSamplingRatio) {
        case 256: OSR = 8; break;
        case 512: OSR = 9; break;
        case 1024: OSR = 10; break;
        case 2048: OSR = 11; break;
        case 4096: OSR = 12; break;
        case 8192: OSR = 13; break;
        default:
            OSR = 12;
            break;  // fallback, though validation above should prevent this
    }
    MS_DBG(F("  Requesting OSR:"), OSR, F("for oversampling ratio:"),
           _overSamplingRatio);
    // Convert oversampling ratio to the value expected by the MS5837 library
    // (8-13 for oversampling ratios 256-8192)
    int  read_return = MS5837_internal.read(OSR);
    bool success     = read_return == 0;
    if (success) {
        // Get temperature in Celsius
        temp = MS5837_internal.getTemperature();

        // Get pressure in millibar
        press = MS5837_internal.getPressure();
    } else {
        MS_DBG(F("  Read failed, error:"), MS5837_internal.getLastError(),
               F("Return value from read():"), read_return);
        return bumpMeasurementAttemptCount(false);
    }

    // Validate the readings
    float maxPressure = 0.0f;
    switch (_model) {
        case static_cast<uint8_t>(MS5837Model::MS5803_01BA):
            maxPressure = 1000.0f;
            break;  // 1 bar = 1000 mbar
        case static_cast<uint8_t>(MS5837Model::MS5837_02BA):
            maxPressure = 2000.0f;
            break;  // 2 bar = 2000 mbar
        case static_cast<uint8_t>(MS5837Model::MS5837_30BA):
            maxPressure = 30000.0f;
            break;  // 30 bar = 30000 mbar
        default: maxPressure = 30000.0f; break;
    }

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Pressure returns 0 when disconnected, which is highly unlikely to be
    // a real value.
    // Pressure range depends on the model; allow 5% over max pressure
    if (!isnan(press) && press > 0.0f && press <= maxPressure * 1.05f) {
        MS_DBG(F("  Pressure:"), press);
        verifyAndAddMeasurementResult(MS5837_PRESSURE_VAR_NUM, press);
    } else if (!isnan(press)) {
        MS_DBG(F("  Pressure out of range:"), press);
        success = false;
    } else {
        MS_DBG(F("  Pressure is NaN"));
        success = false;
    }

    // Temperature Range is -40°C to +85°C
    if (!isnan(temp) && temp >= -40.0f && temp <= 85.0f) {
        MS_DBG(F("  Temperature:"), temp);
        verifyAndAddMeasurementResult(MS5837_TEMP_VAR_NUM, temp);
    } else if (!isnan(temp)) {
        MS_DBG(F("  Temperature out of range:"), temp);
        success = false;
    } else {
        MS_DBG(F("  Temperature is NaN"));
        success = false;
    }

    if (success) {
        // Calculate and store depth and altitude only if input temperature and
        // depth are are valid
        // If the temperature and pressure are valid - and we've already checked
        // for reasonable air pressure and fluid density, then the altitude and
        // depth will be valid.

        // Calculate altitude in meters using configured air pressure
        alt = MS5837_internal.getAltitude(_airPressure);
        MS_DBG(F("  Altitude:"), alt);
        verifyAndAddMeasurementResult(MS5837_ALTITUDE_VAR_NUM, alt);

        // Calculate depth in meters
        // Note: fluidDensity is set in the MS5837_internal object at setup and
        // used in the getDepth() function. The fluidDensity is only set in the
        // constructor and further setters and getters are not provided, so
        // there's no reason to re-pass the value to the internal object here.
        depth = MS5837_internal.getDepth();
        MS_DBG(F("  Depth:"), depth);
        verifyAndAddMeasurementResult(MS5837_DEPTH_VAR_NUM, depth);
    } else {
        MS_DBG(
            F("  Invalid readings, skipping depth and altitude calculations"));
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}


bool TEConnectivityMS5837::validateAndCorrectModel() {
    MS_DBG(F("Attempting to read SENS_T1 from PROM of sensor at address"),
           String(MS5837_internal.getAddress(), HEX));

    uint8_t address = MS5837_internal.getAddress();

    // Verify I2C connectivity with a lightweight probe
    _wire->beginTransmission(address);
    if (_wire->endTransmission() != 0) {
        MS_DBG(F("  I2C communication failed at 0x"), String(address, HEX));
        return false;  // can't change the model since we can't communicate with
                       // the sensor at all
    }

    // MS5837_CMD_READ_PROM  = 0xA0
    // Read SENS_T1 from PROM 1 [0xA0 + (1*2)]
    _wire->beginTransmission(address);
    _wire->write(0xA0 + (1 * 2));
    if (_wire->endTransmission() != 0) {
        MS_DBG(F("Failed to request SENS_T1 from PROM. Unable to validate "
                 "pressure range."));
        return false;  // can't change the model since we can't request the
                       // calibration value
    }

    uint8_t length = 2;
    if (_wire->requestFrom(address, length) != length) {
        MS_DBG(F("Failed to retrieve SENS_T1 from PROM. Unable to validate "
                 "pressure range."));
        return false;  // can't change the model since we can't retrieve the
                       // calibration value
    }
    uint16_t SENS_T1 = (_wire->read() << 8) | _wire->read();
    MS_DBG(F("SENS_T1 value:"), SENS_T1);

    // Values from
    // https://github.com/ArduPilot/ardupilot/pull/29122#issuecomment-2877269114
    const uint16_t MS5837_02BA_MAX_SENSITIVITY = 49000;
    const uint16_t MS5837_02BA_30BA_SEPARATION = 37000;
    const uint16_t MS5837_30BA_MIN_SENSITIVITY = 26000;
    // PROM Word 1 represents the sensor's pressure sensitivity calibration
    // NOTE: The calibrated pressure sensitivity value (SENS_T1) is **not** the
    // same as the as the pressure range from the datasheet!
    //  Set _model according to the experimental pressure sensitivity thresholds
    if (SENS_T1 > MS5837_30BA_MIN_SENSITIVITY &&
        SENS_T1 < MS5837_02BA_30BA_SEPARATION &&
        _model == static_cast<uint8_t>(MS5837Model::MS5837_02BA)) {
        MS_DBG(
            F("SENS_T1 value indicates 30BA model, but model is set to 02BA"));
        MS_DBG(F("Changing model to 30BA"));
        _model = static_cast<uint8_t>(MS5837Model::MS5837_30BA);
        return true;  // Return true to indicate that the model was changed
    } else if (SENS_T1 > MS5837_02BA_30BA_SEPARATION &&
               SENS_T1 < MS5837_02BA_MAX_SENSITIVITY &&
               _model == static_cast<uint8_t>(MS5837Model::MS5837_30BA)) {
        MS_DBG(
            F("SENS_T1 value indicates 02BA model, but model is set to 30BA"));
        MS_DBG(F("Setting model to 02BA"));
        _model = static_cast<uint8_t>(MS5837Model::MS5837_02BA);
        return true;  // Return true to indicate that the model was changed
    }
    return false;  // Model was not changed
}
