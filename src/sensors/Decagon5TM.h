/*
 *Decagon5TM.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Decagon Devices 5TM Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library and the SDI12Sensors super class.
 *
 *Documentation for the SDI-12 Protocol commands and responses
 *for the Decagon 5TM can be found at:
 * http://manuals.decagon.com/Integration%20Guides/5TM%20Integrators%20Guide.pdf
 *
 * For Ea and VWC:
 *     Resolution is 0.0008 m3/m3 (0.08% VWC) from 0 – 50% VWC
 *     Accuracy for Generic calibration equation: ± 0.03 m3/m3 (± 3% VWC) typ
 *     Accuracy for Medium Specific Calibration: ± 0.02 m3/m3 (± 2% VWC)
 *     Range is 0 – 1 m3/m3 (0 – 100% VWC)
 *
 * For Temperature:
 *     Resolution is 0.1°C
 *     Accuracy is ± 1°C
 *     Range is - 40°C to + 50°C
 *
 * Maximum warm-up time in SDI-12 mode: 200ms, assume stability at warm-up
 * Maximum measurement duration: 200ms
*/

// Header Guards
#ifndef Decagon5TM_h
#define Decagon5TM_h

// Included Dependencies
#include "VariableBase.h"
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines
#define TM_NUM_VARIABLES 3
#define TM_WARM_UP_TIME_MS 200
#define TM_STABILIZATION_TIME_MS 0
#define TM_MEASUREMENT_TIME_MS 200

#define TM_EA_RESOLUTION 5
// adding extra digit to resolution for averaging
#define TM_EA_VAR_NUM 0

#define TM_TEMP_RESOLUTION 2
// adding extra digit to resolution for averaging
#define TM_TEMP_VAR_NUM 1

#define TM_VWC_RESOLUTION 3
// adding extra digit to resolution for averaging
#define TM_VWC_VAR_NUM 2

// The main class for the Decagon 5TM
class Decagon5TM : public SDI12Sensors
{
public:
    // Constructors with overloads
    Decagon5TM(char SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage = 1)
     : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                    "Decagon5TM", TM_NUM_VARIABLES,
                    TM_WARM_UP_TIME_MS, TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS)
    {}
    Decagon5TM(char *SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage = 1)
     : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                    "Decagon5TM", TM_NUM_VARIABLES,
                    TM_WARM_UP_TIME_MS, TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS)
    {}
    Decagon5TM(int SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage = 1)
     : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                    "Decagon5TM", TM_NUM_VARIABLES,
                    TM_WARM_UP_TIME_MS, TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS)
    {}
    // Destructor
    ~Decagon5TM(){}

    virtual bool addSingleMeasurementResult(void) override;
};


// Defines the Ea/Matric Potential Variable
class Decagon5TM_Ea : public Variable
{
public:
    Decagon5TM_Ea()
      : Variable(TM_EA_VAR_NUM, TM_EA_RESOLUTION,
                 "permittivity", "faradPerMeter", "SoilEa")

    {}
    ~Decagon5TM_Ea(){}
};


// Defines the Temperature Variable
class Decagon5TM_Temp : public Variable
{
public:
    Decagon5TM_Temp()
      : Variable(TM_TEMP_VAR_NUM, TM_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius", "SoilTemp")

    {}
    ~Decagon5TM_Temp(){}
};


// Defines the Volumetric Water Content Variable
class Decagon5TM_VWC : public Variable
{
public:
    Decagon5TM_VWC()
      : Variable(TM_VWC_VAR_NUM, TM_VWC_RESOLUTION,
                 "volumetricWaterContent", "percent", "SoilVWC")

    {}
    ~Decagon5TM_VWC(){}
};

// Undefine debugging macro, if applicable, so we don't have macro "leaks"
#ifdef DEBUGGING_SERIAL_OUTPUT
#undef DEBUGGING_SERIAL_OUTPUT
#endif

#endif  // Header Guard
