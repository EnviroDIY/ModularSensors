/**
 * @file TurnerTurbidityPlus.cpp
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from TurnerCyclops by Matt Barney <mbarney@tu.org>
 * @brief Implements the TurnerTurbidityPlus class.
 */


#include "TurnerTurbidityPlus.h"


// The constructor - need the power pin, the data pin, and the calibration info
TurnerTurbidityPlus::TurnerTurbidityPlus(
    int8_t powerPin, int8_t wiperTriggerPin, ttp_adsDiffMux_t adsDiffMux,
    float conc_std, float volt_std, float volt_blank, uint8_t i2cAddress,
    adsGain_t PGA_gain, uint8_t measurementsToAverage,
    float voltageDividerFactor)
    : Sensor("TurnerTurbidityPlus", TURBIDITY_PLUS_NUM_VARIABLES,
             TURBIDITY_PLUS_WARM_UP_TIME_MS,
             TURBIDITY_PLUS_STABILIZATION_TIME_MS,
             TURBIDITY_PLUS_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage),
      _wiperTriggerPin(wiperTriggerPin),
      _adsDiffMux(adsDiffMux),
      _conc_std(conc_std),
      _volt_std(volt_std),
      _volt_blank(volt_blank),
      _i2cAddress(i2cAddress),
      _PGA_gain(PGA_gain),
      _voltageDividerFactor(voltageDividerFactor) {}
// Destructor
TurnerTurbidityPlus::~TurnerTurbidityPlus() {}


String TurnerTurbidityPlus::getSensorLocation(void) {
#ifndef MS_USE_ADS1015
    String sensorLocation = F("ADS1115_0x");
#else
    String sensorLocation = F("ADS1015_0x");
#endif
    sensorLocation += String(_i2cAddress, HEX);
    sensorLocation += F("_adsDiffMux");
    sensorLocation += String(_adsDiffMux);
    return sensorLocation;
}

void TurnerTurbidityPlus::runWiper() {
    // Turner Tubidity Plus wiper requires a 50ms LOW signal pulse to trigger
    // one wiper rotation. Also note: I was unable to trigger multiple rotations
    // without pausing for ~540ms between them.
    MS_DBG(F("Initate TurbidityPlus wiper on"), getSensorLocation());
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
    return Sensor::setup();
}

bool TurnerTurbidityPlus::wake(void) {
    // Run the wiper before taking a reading
    runWiper();

    return Sensor::wake();
}

void TurnerTurbidityPlus::powerDown(void) {
    // Set the wiper trigger pin LOW to avoid power drain.
    digitalWrite(_wiperTriggerPin, LOW);
    return Sensor::powerDown();
}

void TurnerTurbidityPlus::powerUp(void) {
    // Set the wiper trigger pin HIGH to prepare for wiping.
    digitalWrite(_wiperTriggerPin, HIGH);
    return Sensor::powerUp();
}

bool TurnerTurbidityPlus::addSingleMeasurementResult(void) {
    // Variables to store the results in
    int16_t adcCounts   = -9999;
    float   adcVoltage  = -9999;
    float   calibResult = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

// Create an Auxillary ADD object
// We create and set up the ADC object here so that each sensor using
// the ADC may set the gain appropriately without effecting others.
#ifndef MS_USE_ADS1015
        Adafruit_ADS1115 ads;  // Use this for the 16-bit version
#else
        Adafruit_ADS1015 ads;  // Use this for the 12-bit version
#endif
        // ADS Library default settings:
        //  - TI1115 (16 bit)
        //    - single-shot mode (powers down between conversions)
        //    - 128 samples per second (8ms conversion time)
        //    - 2/3 gain +/- 6.144V range (limited to VDD +0.3V max)
        //  - TI1015 (12 bit)
        //    - single-shot mode (powers down between conversions)
        //    - 1600 samples per second (625µs conversion time)
        //    - 2/3 gain +/- 6.144V range (limited to VDD +0.3V max)

        ads.setGain(_PGA_gain);
        // Begin ADC
        ads.begin(_i2cAddress);

        // Print out the calibration curve
        MS_DBG(F("  Input calibration Curve:"), _volt_std, F("V at"), _conc_std,
               F(".  "), _volt_blank, F("V blank."));

        // Read Analog to Digital Converter (ADC)
        // Taking this reading includes the 8ms conversion delay.
        // Measure the voltage differential across the two voltage pins
        switch (_adsDiffMux) {
            case DIFF_MUX_0_1: {
                adcCounts = ads.readADC_Differential_0_1();
                break;
            }
            case DIFF_MUX_0_3: {
                adcCounts = ads.readADC_Differential_0_3();
                break;
            }
            case DIFF_MUX_1_3: {
                adcCounts = ads.readADC_Differential_1_3();
                break;
            }
            case DIFF_MUX_2_3: {
                adcCounts = ads.readADC_Differential_2_3();
                break;
            }
        }
        // Convert ADC counts value to voltage (V)
        adcVoltage = ads.computeVolts(adcCounts);
        MS_DBG(F("  ads.readADC_Differential("), _adsDiffMux, F("):"),
               String(adcVoltage, 3));

        // The ADS1X15 outputs a max value corresponding to Vcc + 0.3V
        if (adcVoltage < 5.3 && adcVoltage > -0.3) {
            // Skip results out of range
            // Apply the unique calibration curve for the given sensor
            calibResult = (_conc_std / (_volt_std - _volt_blank)) *
                (adcVoltage - _volt_blank);
            MS_DBG(F("  calibResult:"), String(calibResult, 3));
        } else {  // set invalid voltages back to -9999
            adcVoltage = -9999;
        }
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(TURBIDITY_PLUS_VAR_NUM, calibResult);
    verifyAndAddMeasurementResult(TURBIDITY_PLUS_VOLTAGE_VAR_NUM, adcVoltage);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    if (adcVoltage < 5.3 && adcVoltage > -0.3) {
        return true;
    } else {
        return false;
    }
}
