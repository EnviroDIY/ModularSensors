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

#ifndef BatteryMonitor_h
#define BatteryMonitor_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"

#define ADS1115_ADDRESS (0x48) // 1001 000 (ADDR = GND)

#define BAT_NUM_VARIABLES 1
#define BAT_WARM_UP_TIME_MS 1  // Unknown!
#define BAT_STABILIZATION_TIME_MS 1  // Unknown!
#define BAT_MEASUREMENT_TIME_MS 1  // Unknown!

#define BAT_VOLT_VAR_NUM 0
#define BAT_VOLT_RESOLUTION 2 //Is this correct? 

// The main class for the Apogee SQ-212 sensor
class BatteryMonitor : public Sensor
{

public:
    // The constructor - need the power pin and the data pin
    BatteryMonitor(int8_t powerPin, int8_t dataPin, uint8_t i2cAddress = ADS1115_ADDRESS, uint8_t measurementsToAverage = 1);

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

protected:
    uint8_t _i2cAddress;

};


//Dont this we need this for batter monitor??
// // Defines the PAR variable output from the Apogee SQ-212 sensor
// class ApogeeSQ212_PAR : public Variable
// {
// public:
//     ApogeeSQ212_PAR(Sensor *parentSense,
//                     String UUID = "", String customVarCode = "")
//       : Variable(parentSense, SQ212_PAR_VAR_NUM,
//                  F("radiationIncomingPAR"), F("microeinsteinPerSquareMeterPerSecond"),
//                  SQ212_PAR_RESOLUTION,
//                  F("photosyntheticallyActiveRadiation"), UUID, customVarCode)
//     {}
// };

class BatteryMonitor_Volt : public Variable
{
public:
    BatteryMonitor_Volt(Sensor *parentSense,
                     String UUID = "", String customVarCode = "")
      : Variable(parentSense, BAT_VOLT_VAR_NUM,
               F("voltage"), F("volts"),
               BAT_VOLT_RESOLUTION,
               F("voltage divider"), UUID, customVarCode)
    {}
};

#endif
