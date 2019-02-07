/*
 *CampbellOBS3.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Campbell Scientific OBS-3+
 *This is dependent on the soligen2010 fork of the Adafruit ADS1015 library.
 *
 * Ranges: (depends on sediment size, particle shape, and reflectivity)
 *  Turbidity (low/high): 250/1000 NTU; 500/2000 NTU; 1000/4000 NTU
 *  Mud: 5000 to 10,000 mg L–1
 *  Sand: 50,000 to 100,000 mg L–1
 * Accuracy: (whichever is larger)
 *  Turbidity: 2% of reading or 0.5 NTU
 *  Mud: 2% of reading or 1 mg L–1
 *  Sand: 4% of reading or 10 mg L–1
 * Resolution:
 *  16-bit ADC
 *      Turbidity: 0.03125/0.125 NTU; 0.0625/0.25 NTU; 0.125/0.5 NTU
 *  12-bit ADC
 *      Turbidity: 0.5/2.0 NTU; 1.0/4.0 NTU; 2.0/8.0 NTU
 *
 * Minimum stabilization time: 2s
 * Maximum data rate = 10Hz (100ms/sample)
*/

// Header Guards
#ifndef CampbellOBS3_h
#define CampbellOBS3_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
#define ADS1115_ADDRESS 0x48
// 1001 000 (ADDR = GND)

// low and high range are treated as completely independent, so only 2 "variables"
// One for the raw voltage and another for the calibrated turbidity.
// To get both high and low range values, create two sensor objects!
#define OBS3_NUM_VARIABLES 2
// Using the warm-up time of the ADS1115
#define OBS3_WARM_UP_TIME_MS 2
#define OBS3_STABILIZATION_TIME_MS 2000
#define OBS3_MEASUREMENT_TIME_MS 100

#define OBS3_TURB_VAR_NUM 0
#ifdef MS_USE_ADS1015
#define OBS3_RESOLUTION 1
#else
#define OBS3_RESOLUTION 5
#endif

#define OBS3_VOLTAGE_VAR_NUM 1
#ifdef MS_USE_ADS1015
#define OBS3_VOLT_RESOLUTION 1
#else
#define OBS3_VOLT_RESOLUTION 4
#endif

// The main class for the Campbell OBS3
class CampbellOBS3 : public Sensor
{
public:
    // The constructor - need the power pin, the data pin, and the calibration info
    CampbellOBS3(int8_t powerPin, int8_t dataPin,
                 float x2_coeff_A, float x1_coeff_B, float x0_coeff_C,
                 uint8_t i2cAddress = ADS1115_ADDRESS, uint8_t measurementsToAverage = 1);
    // Destructor
    ~CampbellOBS3();

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

protected:
    float _x2_coeff_A, _x1_coeff_B, _x0_coeff_C;
    uint8_t _i2cAddress;
};


// The main variable returned is turbidity
// To utilize both high and low gain turbidity, you must create *two* sensor
// objects on two different data pins and then create two variable objects, one
// tied to each sensor.
class CampbellOBS3_Turbidity : public Variable
{
public:
    CampbellOBS3_Turbidity(Sensor *parentSense,
                           const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, OBS3_TURB_VAR_NUM,
                 "turbidity", "nephelometricTurbidityUnit",
                 OBS3_RESOLUTION,
                 "OBS3Turbidity", UUID, customVarCode)
    {}
    ~CampbellOBS3_Turbidity(){}
};


// Also returning raw voltage
// This could be helpful if the calibration equation was typed incorrectly
class CampbellOBS3_Voltage : public Variable
{
public:
    CampbellOBS3_Voltage(Sensor *parentSense,
                         const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, OBS3_VOLTAGE_VAR_NUM,
                 "voltage", "volt",
                 OBS3_VOLT_RESOLUTION,
                 "OBS3Voltage", UUID, customVarCode)
    {}
    ~CampbellOBS3_Voltage(){}
};

#endif  // Header Guard
