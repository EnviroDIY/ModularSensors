/*
 *ApogeeSQ212.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Written By:  Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Adapted from CampbellOBS3.h by Sara Damiano (sdamiano@stroudcenter.org)

 * This file is for the Apogee SQ-212 Quantum Light sensor
 * This is dependent on the Adafruit ADS1015 library.
 *
 * Apogee SQ-212 Quantum Light sensor measures photosynthetically active radiation (PAR)
 and is typically defined as total radiation across a range of 400 to 700 nm.
 PAR is often expressed as photosynthetic photon flux density (PPFD):
 photon flux in units of micromoles per square meter per second (μmol m-2 s-1,
 equal to microEinsteins per square meter per second) summed from 400 to 700 nm.

 * Technical specifications for the Apogee SQ-212 can be found at:
 * https://www.apogeeinstruments.com/sq-212-amplified-0-2-5-volt-sun-calibration-quantum-sensor/
 *
 * Power supply: 5-24 V DC with a nominal current draw of 300 μA

 * Response time: <1ms

*/

#ifndef ApogeeSQ212_h
#define ApogeeSQ212_h

#include "SensorBase.h"
#include "VariableBase.h"

// #define MODULES_DBG Serial
#include "ModSensorDebugger.h"

#define ADS1015_ADDRESS (0x48) // 1001 000 (ADDR = GND)

#define SQ212_NUM_VARIABLES 1
#define SQ212_WARM_UP 2     // Time in ms
#define SQ212_PAR_VAR_NUM 0
#define SQ212_PAR_RESOLUTION 2

// The main class for the Apogee SQ-212 sensor
class ApogeeSQ212 : public Sensor
{
public:
    // The constructor - need the power pin and the data pin
    ApogeeSQ212(int powerPin, int dataPin, uint8_t i2cAddress = ADS1015_ADDRESS);

    String getSensorLocation(void) override;

    bool update(void) override;

protected:
    uint8_t _i2cAddress;

};


// Defines the PAR variable output from the Apogee SQ-212 sensor
class ApogeeSQ212_PAR : public Variable
{
public:
    ApogeeSQ212_PAR(Sensor *parentSense, String customVarCode = "")
      : Variable(parentSense, SQ212_PAR_VAR_NUM,
                 F("radiationIncomingPAR"), F("microeinsteinPerSquareMeterPerSecond"),
                 SQ212_PAR_RESOLUTION,
                 F("photosyntheticallyActiveRadiation"), customVarCode)
    {}
};

#endif
