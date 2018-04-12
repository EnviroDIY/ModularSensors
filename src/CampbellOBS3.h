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
 *  16-bit ADC - This is what is supported!
 *      Turbidity: 0.004/0.01 NTU; 0.008/0.03 NTU; 0.01/0.06 NTU
 *  12-bit ADC
 *      Turbidity: 0.06/0.2 NTU; 0.1/0.5 NTU; 0.2/1.0 NTU
 *
 * Minimum stabilization time: 2s
 * Maximum data rate = 10Hz (100ms/sample)
*/

#ifndef CampbellOBS3_h
#define CampbellOBS3_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"

#define ADS1115_ADDRESS (0x48) // 1001 000 (ADDR = GND)

#define OBS3_NUM_VARIABLES 1  // low and high range are treated as completely independent
#define OBS3_WARM_UP_TIME_MS 2  // Actually warm-up time of ADC
#define OBS3_STABILIZATION_TIME_MS 2000
#define OBS3_MEASUREMENT_TIME_MS 100

#define OBS3_TURB_VAR_NUM 0
#define OBS3_RESOLUTION 3
#define OBS3_HR_RESOLUTION 2

// The main class for the Campbell OBS3
class CampbellOBS3 : public Sensor
{
public:
    // The constructor - need the power pin, the data pin, and the calibration info
    CampbellOBS3(int8_t powerPin, int8_t dataPin,
                 float x2_coeff_A, float x1_coeff_B, float x0_coeff_C,
                 uint8_t i2cAddress = ADS1115_ADDRESS, uint8_t measurementsToAverage = 1);

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

protected:
    float _x2_coeff_A, _x1_coeff_B, _x0_coeff_C;
    uint8_t _i2cAddress;
};


// The single available variable is turbidity
class CampbellOBS3_Turbidity : public Variable
{
public:
    CampbellOBS3_Turbidity(Sensor *parentSense,
                           String UUID = "", String customVarCode = "")
      : Variable(parentSense, OBS3_TURB_VAR_NUM,
                 F("turbidity"), F("nephelometricTurbidityUnit"),
                 OBS3_RESOLUTION,
                 F("Turbidity"), UUID, customVarCode)
    {}
};

#endif
