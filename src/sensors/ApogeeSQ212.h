/*
 *ApogeeSQ212.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Written By:  Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Adapted from CampbellOBS3.h by Sara Damiano (sdamiano@stroudcenter.org)
 *
 * This file is for the Apogee SQ-212 Quantum Light sensor
 * This is dependent on the soligen2010 fork of the Adafruit ADS1015 library.
 *
 * Apogee SQ-212 Quantum Light sensor measures photosynthetically active
 radiation (PAR) and is typically defined as total radiation across a range of
 400 to 700 nm. PAR is often expressed as photosynthetic photon flux density
 (PPFD): photon flux in units of micromoles per square meter per second (μmol
 m-2 s-1, equal to microEinsteins per square meter per second) summed from 400
 to 700 nm.
 *
 * Range is 0 to 2500 µmol m-2 s-1
 * Accuracy is ± 0.5%
 * Resolution:
 *  16-bit ADC: 0.3125 µmol m-2 s-1
 *  12-bit ADC: 5 µmol m-2 s-1
 *
 * Technical specifications for the Apogee SQ-212 can be found at:
 * https://www.apogeeinstruments.com/sq-212-amplified-0-2-5-volt-sun-calibration-quantum-sensor/
 *
 * Power supply: 5-24 V DC with a nominal current draw of 300 μA
 *
 * Response time: < 1ms
 * Resample time: max of ADC (860/sec)
*/

// Header Guards
#ifndef SRC_SENSORS_APOGEESQ212_H_
#define SRC_SENSORS_APOGEESQ212_H_

// Debugging Statement
// #define MS_APOGEESQ212_DEBUG

#ifdef MS_APOGEESQ212_DEBUG
#define MS_DEBUGGING_STD "ApogeeSQ212"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
#define ADS1115_ADDRESS 0x48
// 1001 000 (ADDR = GND)

#define SQ212_NUM_VARIABLES 2
// Using the warm-up time of the ADS1115
#define SQ212_WARM_UP_TIME_MS 2
// These times are not known!
#define SQ212_STABILIZATION_TIME_MS 2
#define SQ212_MEASUREMENT_TIME_MS 2

#define SQ212_PAR_VAR_NUM 0
#ifdef MS_USE_ADS1015
#define SQ212_PAR_RESOLUTION 0
#else
#define SQ212_PAR_RESOLUTION 4
#endif

#define SQ212_VOLTAGE_VAR_NUM 1
#ifdef MS_USE_ADS1015
#define SQ212_VOLT_RESOLUTION 1
#else
#define SQ212_VOLT_RESOLUTION 4
#endif

// The main class for the Apogee SQ-212 sensor
class ApogeeSQ212 : public Sensor {
 public:
    // The constructor - need the power pin and the data channel on the ADS1x15
    ApogeeSQ212(int8_t powerPin, uint8_t adsChannel,
                uint8_t i2cAddress            = ADS1115_ADDRESS,
                uint8_t measurementsToAverage = 1);
    // Destructor
    ~ApogeeSQ212();

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 protected:
    uint8_t _adsChannel;
    uint8_t _i2cAddress;
};


// Defines the PAR variable output from the Apogee SQ-212 sensor
class ApogeeSQ212_PAR : public Variable {
 public:
    ApogeeSQ212_PAR(Sensor* parentSense, const char* uuid = "",
                    const char* varCode = "photosyntheticallyActiveRadiation")
        : Variable(parentSense, (const uint8_t)SQ212_PAR_VAR_NUM,
                   (uint8_t)SQ212_PAR_RESOLUTION, "radiationIncomingPAR",
                   "microeinsteinPerSquareMeterPerSecond", varCode, uuid) {}
    ApogeeSQ212_PAR()
        : Variable((const uint8_t)SQ212_PAR_VAR_NUM,
                   (uint8_t)SQ212_PAR_RESOLUTION, "radiationIncomingPAR",
                   "microeinsteinPerSquareMeterPerSecond",
                   "photosyntheticallyActiveRadiation") {}
    ~ApogeeSQ212_PAR() {}
};


// Defines the PAR variable output from the Apogee SQ-212 sensor
class ApogeeSQ212_Voltage : public Variable {
 public:
    ApogeeSQ212_Voltage(Sensor* parentSense, const char* uuid = "",
                        const char* varCode = "SQ212Voltage")
        : Variable(parentSense, (const uint8_t)SQ212_VOLTAGE_VAR_NUM,
                   (uint8_t)SQ212_VOLT_RESOLUTION, "voltage", "volt", varCode,
                   uuid) {}
    ApogeeSQ212_Voltage()
        : Variable((const uint8_t)SQ212_VOLTAGE_VAR_NUM,
                   (uint8_t)SQ212_VOLT_RESOLUTION, "voltage", "volt",
                   "SQ212Voltage") {}
    ~ApogeeSQ212_Voltage() {}
};

#endif  // SRC_SENSORS_APOGEESQ212_H_
