/**
 * @file TurnerTurbidityPlus.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from TurnerCyclops by Matt Barney <mbarney@tu.org>
 * @brief Implements the TurnerTurbidityPlus class.
 */


#include "TurnerTurbidityPlus.h"
#include "TIADS1x15.h"


// The constructor - need the power pin, the data pin, and the calibration info
TurnerTurbidityPlus::TurnerTurbidityPlus(
    int8_t powerPin, int8_t wiperTriggerPin, tiads1x15_adsDiffMux_t adsDiffMux,
    float conc_std, float volt_std, float volt_blank, uint8_t i2cAddress,
    adsGain_t PGA_gain, uint8_t measurementsToAverage,
    float voltageDividerFactor)
    : TIADS1x15(powerPin, adsDiffMux, voltageDividerFactor, PGA_gain,
                i2cAddress, measurementsToAverage),
      _wiperTriggerPin(wiperTriggerPin),
      _conc_std(conc_std),
      _volt_std(volt_std),
      _volt_blank(volt_blank) {
    // Override timing settings for Turner-specific requirements
    // These are protected members from the Sensor base class
    _warmUpTime_ms        = TURBIDITY_PLUS_WARM_UP_TIME_MS;
    _stabilizationTime_ms = TURBIDITY_PLUS_STABILIZATION_TIME_MS;
    _measurementTime_ms   = TURBIDITY_PLUS_MEASUREMENT_TIME_MS;
    // Override variable counts from parent class defaults
    _numReturnedValues = TURBIDITY_PLUS_NUM_VARIABLES;
    _incCalcValues     = TURBIDITY_PLUS_INC_CALC_VARIABLES;
    // Set the sensor name
    _sensorName = "TurnerTurbidityPlus";
}
// Destructor
TurnerTurbidityPlus::~TurnerTurbidityPlus() {}


String TurnerTurbidityPlus::getSensorLocation(void) {
    // Use TIADS1x15's location with Turner-specific identifier
    String sensorLocation = TIADS1x15::getSensorLocation();
    sensorLocation += F("_TurnerTurb");
    return sensorLocation;
}

void TurnerTurbidityPlus::runWiper() {
    // Turner Turbidity Plus wiper requires a 50ms LOW signal pulse to trigger
    // one wiper rotation. Also note: I was unable to trigger multiple rotations
    // without pausing for ~540ms between them.
    MS_DBG(F("Turn TurbidityPlus wiper on"), getSensorLocation());
    digitalWrite(_wiperTriggerPin, LOW);
    delay(50);
    digitalWrite(_wiperTriggerPin, HIGH);
    // It takes ~7.5 sec for a rotation to complete. Wait for that to finish
    // before continuing, otherwise the sensor will get powered off before wipe
    // completes, and any reading taken during wiper cycle is invalid.
    delay(8000);
    MS_DBG(F("TurbidityPlus wiper cycle should be finished"));
}

bool TurnerTurbidityPlus::setup(void) {
    // Set up the wiper trigger pin, which is active-LOW.
    pinMode(_wiperTriggerPin, OUTPUT);
    return TIADS1x15::setup();
}

bool TurnerTurbidityPlus::wake(void) {
    // Set the wiper trigger pin mode.
    // Reset this on every wake because pins are set to tri-state on sleep
    pinMode(_wiperTriggerPin, OUTPUT);
    // Run the wiper before taking a reading
    runWiper();

    return TIADS1x15::wake();
}

void TurnerTurbidityPlus::powerDown(void) {
    // Set the wiper trigger pin LOW to avoid power drain.
    digitalWrite(_wiperTriggerPin, LOW);
    return TIADS1x15::powerDown();
}

void TurnerTurbidityPlus::powerUp(void) {
    // Set the wiper trigger pin HIGH to prepare for wiping.
    digitalWrite(_wiperTriggerPin, HIGH);
    return TIADS1x15::powerUp();
}

bool TurnerTurbidityPlus::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool  success     = false;
    float adcVoltage  = -9999;
    float calibResult = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Print out the calibration curve
    MS_DBG(F("  Input calibration Curve:"), _volt_std, F("V at"), _conc_std,
           F(".  "), _volt_blank, F("V blank."));
    const float epsilon = 1e-4f;  // tune to expected sensor precision
    if (fabs(_volt_std - _volt_blank) < epsilon) {
        MS_DBG(F("Invalid calibration: point voltage equals blank voltage"));
        return bumpMeasurementAttemptCount(false);
    }

    // Use the TIADS1x15 differential voltage reading function
    // The voltage multiplier and gain settings are handled by the parent class
    if (readVoltageDifferential(adcVoltage)) {
        MS_DBG(F("  Differential voltage (after voltage multiplier):"),
               String(adcVoltage, 3), F("V"));
        // Apply the unique calibration curve for the given sensor
        calibResult = (_conc_std / (_volt_std - _volt_blank)) *
            (adcVoltage - _volt_blank);
        MS_DBG(F("  calibResult:"), String(calibResult, 3));
        verifyAndAddMeasurementResult(TURBIDITY_PLUS_VAR_NUM, calibResult);
        verifyAndAddMeasurementResult(TURBIDITY_PLUS_VOLTAGE_VAR_NUM,
                                      adcVoltage);
        success = true;
    } else {
        MS_DBG(F("  Failed to read differential voltage"));
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
