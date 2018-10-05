/*
 *ApogeeSQ212.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Written By:  Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Adapted from CampbellOBS3.h by Sara Damiano (sdamiano@stroudcenter.org)

 * This file is for the Apogee SQ-212 Quantum Light sensor
 * This is dependent on the soligen2010 fork of the Adafruit ADS1015 library.
 *
 * Apogee SQ-212 Quantum Light sensor measures photosynthetically active radiation (PAR)
 and is typically defined as total radiation across a range of 400 to 700 nm.
 PAR is often expressed as photosynthetic photon flux density (PPFD):
 photon flux in units of micromoles per square meter per second (μmol m-2 s-1,
 equal to microEinsteins per square meter per second) summed from 400 to 700 nm.
 *
 * Range is 0 to 2500 µmol m-2 s-1
 * Accuracy is ± 0.5%
 * Resolution:
 *  16-bit ADC: 0.04 µmol m-2 s-1 - This is what is supported!
 *  12-bit ADC: 2.44 µmol m-2 s-1
 *
 * Technical specifications for the Apogee SQ-212 can be found at:
 * https://www.apogeeinstruments.com/sq-212-amplified-0-2-5-volt-sun-calibration-quantum-sensor/
 *
 * Power supply: 5-24 V DC with a nominal current draw of 300 μA
 *
 * Response time: < 1ms
 * Resample time: max of ADC (860/sec)
*/

#ifndef ApogeeSQ212_h
#define ApogeeSQ212_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "../SensorBase.h"
#include "../VariableBase.h"

#define ADS1115_ADDRESS (0x48) // 1001 000 (ADDR = GND)

#define SQ212_NUM_VARIABLES 1
// Using the warm-up time of the ADS1115
#define SQ212_WARM_UP_TIME_MS 2
// These times are not known!
#define SQ212_STABILIZATION_TIME_MS 2
#define SQ212_MEASUREMENT_TIME_MS 2

#define SQ212_PAR_VAR_NUM 0
#define SQ212_PAR_RESOLUTION 2

// The main class for the Apogee SQ-212 sensor
class ApogeeSQ212 : public Sensor
{

public:
    // The constructor - need the power pin and the data pin  ON THE ADC
    ApogeeSQ212(int8_t powerPin, int8_t dataPin,
                uint8_t i2cAddress = ADS1115_ADDRESS, uint8_t measurementsToAverage = 1);
    // Destructor
    ~ApogeeSQ212();

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

protected:
    uint8_t _i2cAddress;

};


// Defines the PAR variable output from the Apogee SQ-212 sensor
class ApogeeSQ212_PAR : public Variable
{
public:
    ApogeeSQ212_PAR(Sensor *parentSense,
                    const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, SQ212_PAR_VAR_NUM,
                 "radiationIncomingPAR", "microeinsteinPerSquareMeterPerSecond",
                 SQ212_PAR_RESOLUTION,
                 "photosyntheticallyActiveRadiation", UUID, customVarCode)
    {}
    ~ApogeeSQ212_PAR(){};
};

#endif
