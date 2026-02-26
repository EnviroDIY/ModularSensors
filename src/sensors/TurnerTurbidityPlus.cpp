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
    int8_t powerPin, int8_t wiperTriggerPin, int8_t analogChannel,
    int8_t analogReferenceChannel, float conc_std, float volt_std,
    float volt_blank, uint8_t measurementsToAverage,
    AnalogVoltageBase* analogVoltageReader)
    : Sensor("TurnerTurbidityPlus", TURBIDITY_PLUS_NUM_VARIABLES,
             TURBIDITY_PLUS_WARM_UP_TIME_MS,
             TURBIDITY_PLUS_STABILIZATION_TIME_MS,
             TURBIDITY_PLUS_MEASUREMENT_TIME_MS, powerPin, analogChannel,
             measurementsToAverage),
      _wiperTriggerPin(wiperTriggerPin),
      _conc_std(conc_std),
      _volt_std(volt_std),
      _volt_blank(volt_blank),
      _analogReferenceChannel(analogReferenceChannel),
      _analogVoltageReader(analogVoltageReader),
      _ownsAnalogVoltageReader(analogVoltageReader == nullptr) {
    // If no analog voltage reader was provided, create a default one
    if (analogVoltageReader == nullptr) {
        _analogVoltageReader = createTIADS1x15Base(_ownsAnalogVoltageReader);
    }
}

// Destructor
TurnerTurbidityPlus::~TurnerTurbidityPlus() {
    // Clean up the analog voltage reader if we created it
    if (_ownsAnalogVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
    }
}


String TurnerTurbidityPlus::getSensorLocation(void) {
    if (_analogVoltageReader != nullptr) {
        return _analogVoltageReader->getAnalogLocation(_dataPin,
                                                       _analogReferenceChannel);
    } else {
        return String(F("Unknown_AnalogVoltageReader"));
    }
}

void TurnerTurbidityPlus::runWiper() {
    // Turner Turbidity Plus wiper requires a 50ms LOW signal pulse to trigger
    // one wiper rotation. Also note: I was unable to trigger multiple rotations
    // without pausing for ~540ms between them.
    MS_DBG(F("Turn TurbidityPlus wiper on"), getSensorLocation());
    digitalWrite(_wiperTriggerPin, LOW);
    delay(TURBIDITY_PLUS_WIPER_TRIGGER_PULSE_MS);
    digitalWrite(_wiperTriggerPin, HIGH);
    // It takes ~7.5 sec for a rotation to complete. Wait for that to finish
    // before continuing, otherwise the sensor will get powered off before wipe
    // completes, and any reading taken during wiper cycle is invalid.
    delay(TURBIDITY_PLUS_WIPER_ROTATION_WAIT_MS);
    MS_DBG(F("TurbidityPlus wiper cycle should be finished"));
}

bool TurnerTurbidityPlus::setup(void) {
    // Set up the wiper trigger pin, which is active-LOW.
    pinMode(_wiperTriggerPin, OUTPUT);
    bool sensorSetupSuccess = Sensor::setup();
    bool analogVoltageReaderSuccess = false;
    
    if (_analogVoltageReader != nullptr) {
        analogVoltageReaderSuccess = _analogVoltageReader->begin();
        if (!analogVoltageReaderSuccess) {
            MS_DBG(getSensorNameAndLocation(),
                   F("Analog voltage reader initialization failed"));
        }
    } else {
        MS_DBG(getSensorNameAndLocation(),
               F("No analog voltage reader to initialize"));
    }
    
    return sensorSetupSuccess && analogVoltageReaderSuccess;
}

bool TurnerTurbidityPlus::wake(void) {
    // Set the wiper trigger pin mode.
    // Reset this on every wake because pins are set to tri-state on sleep
    pinMode(_wiperTriggerPin, OUTPUT);
    // Run the wiper before taking a reading
    runWiper();

    return Sensor::wake();
}

void TurnerTurbidityPlus::powerDown(void) {
    // Set the wiper trigger pin LOW to avoid power drain.
    digitalWrite(_wiperTriggerPin, LOW);
    Sensor::powerDown();
}

void TurnerTurbidityPlus::powerUp(void) {
    // Set the wiper trigger pin HIGH to prepare for wiping.
    digitalWrite(_wiperTriggerPin, HIGH);
    Sensor::powerUp();
}

bool TurnerTurbidityPlus::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    // Check if we have a valid analog voltage reader
    if (_analogVoltageReader == nullptr) {
        MS_DBG(getSensorNameAndLocation(),
               F("No analog voltage reader available"));
        return bumpMeasurementAttemptCount(false);
    }

    // Print out the calibration curve
    MS_DBG(F("  Input calibration Curve:"), _volt_std, F("V at"), _conc_std,
           F(".  "), _volt_blank, F("V blank."));
    if (fabsf(_volt_std - _volt_blank) < TURBIDITY_PLUS_CALIBRATION_EPSILON) {
        MS_DBG(F("Invalid calibration: point voltage equals blank voltage"));
        return bumpMeasurementAttemptCount(false);
    }

    float adcVoltage = -9999.0f;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Read the differential voltage using the AnalogVoltageBase interface.
    // NOTE: All implementations of the AnalogVoltageBase class validate both
    // the input channel and the resulting voltage, so we can trust that a
    // successful read will give us a valid voltage value to work with.
    bool success = _analogVoltageReader->readVoltageDifferential(
        _dataPin, _analogReferenceChannel, adcVoltage);

    if (success) {
        // Apply the unique calibration curve for the given sensor
        float calibResult = (_conc_std / (_volt_std - _volt_blank)) *
            (adcVoltage - _volt_blank);
        MS_DBG(F("  calibResult:"), String(calibResult, 3));
        verifyAndAddMeasurementResult(TURBIDITY_PLUS_VOLTAGE_VAR_NUM,
                                      adcVoltage);
        verifyAndAddMeasurementResult(TURBIDITY_PLUS_VAR_NUM, calibResult);
    } else {
        MS_DBG(F("  Failed to read differential voltage from analog reader"));
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
