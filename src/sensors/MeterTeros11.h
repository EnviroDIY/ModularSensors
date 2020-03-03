/*
 *MeterTeros11.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Written by Anthony Aufdenkampe <aaufdenkampe@limno.com>, based on
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Meter Teros 11 Advanced Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library and the SDI12Sensors super
 *class.
 *
 *Documentation for the SDI-12 Protocol commands and responses
 *for the Meter Teros 11 can be found at:
 * http://publications.metergroup.com/Integrator%20Guide/18224%20TEROS%2011-12%20Integrator%20Guide.pdf
 *
 * For Ea and VWC:
 *     Resolution is 0.001 m3/m3 (0.1% VWC) from 0 – 70% VWC
 *     Accuracy for Generic calibration equation: ± 0.03 m3/m3 (± 3% VWC) typ
 *     Accuracy for Medium Specific Calibration: ± 0.01 to 0.02 m3/m3 (± 1-2%
 *VWC) Range is 0 – 1 m3/m3 (0 – 100% VWC)
 *
 * For Temperature:
 *     Resolution is 0.1°C
 *     Accuracy is ± 0.5°C, from - 40°C to 0°C
 *     Accuracy is ± 0.3°C, from 0°C to + 60°C
 *
 * Warm-up time in SDI-12 mode: 245ms typical, assume stability at warm-up
 * Measurement duration: 25 ms to 50 ms
 *
 * Supply Voltage (VCC to GND), 4.0 to 15.0 VDC
 * Digital Input Voltage (logic high), 2.8 to 3.9 V (3.6 typical)
 * Digital Output Voltage (logic high), 3.6 typical
 */

// Header Guards
#ifndef SRC_SENSORS_METERTEROS11_H_
#define SRC_SENSORS_METERTEROS11_H_

// Debugging Statement
// #define MS_MeterTeros11_DEBUG

#ifdef MS_METERTEROS11_DEBUG
#define MS_DEBUGGING_STD "MeterTeros11"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines
#define TEROS11_NUM_VARIABLES 3
#define TEROS11_WARM_UP_TIME_MS 250
#define TEROS11_STABILIZATION_TIME_MS 50
#define TEROS11_MEASUREMENT_TIME_MS 50

#define TEROS11_EA_RESOLUTION 5
// adding extra digit to resolution for averaging
#define TEROS11_EA_VAR_NUM 0

#define TEROS11_TEMP_RESOLUTION 2
// adding extra digit to resolution for averaging
#define TEROS11_TEMP_VAR_NUM 1

#define TEROS11_VWC_RESOLUTION 3
// adding extra digit to resolution for averaging
#define TEROS11_VWC_VAR_NUM 2

// The main class for the Decagon 5TM
class MeterTeros11 : public SDI12Sensors {
 public:
    // Constructors with overloads
    MeterTeros11(char SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterTeros11", TEROS11_NUM_VARIABLES,
                       TEROS11_WARM_UP_TIME_MS, TEROS11_STABILIZATION_TIME_MS,
                       TEROS11_MEASUREMENT_TIME_MS) {}
    MeterTeros11(char* SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterTeros11", TEROS11_NUM_VARIABLES,
                       TEROS11_WARM_UP_TIME_MS, TEROS11_STABILIZATION_TIME_MS,
                       TEROS11_MEASUREMENT_TIME_MS) {}
    MeterTeros11(int SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterTeros11", TEROS11_NUM_VARIABLES,
                       TEROS11_WARM_UP_TIME_MS, TEROS11_STABILIZATION_TIME_MS,
                       TEROS11_MEASUREMENT_TIME_MS) {}
    // Destructor
    ~MeterTeros11() {}

    virtual bool addSingleMeasurementResult(void) override;
};


// Defines the Ea/Matric Potential Variable
class MeterTeros11_Ea : public Variable {
 public:
    explicit MeterTeros11_Ea(MeterTeros11* parentSense, const char* uuid = "",
                             const char* varCode = "SoilEa")
        : Variable(parentSense, (const uint8_t)TEROS11_EA_VAR_NUM,
                   (uint8_t)TEROS11_EA_RESOLUTION, "permittivity",
                   "faradPerMeter", varCode, uuid) {}
    MeterTeros11_Ea()
        : Variable((const uint8_t)TEROS11_EA_VAR_NUM,
                   (uint8_t)TEROS11_EA_RESOLUTION, "permittivity",
                   "faradPerMeter", "SoilEa") {}
    ~MeterTeros11_Ea() {}
};


// Defines the Temperature Variable
class MeterTeros11_Temp : public Variable {
 public:
    explicit MeterTeros11_Temp(MeterTeros11* parentSense, const char* uuid = "",
                               const char* varCode = "SoilTemp")
        : Variable(parentSense, (const uint8_t)TEROS11_TEMP_VAR_NUM,
                   (uint8_t)TEROS11_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    MeterTeros11_Temp()
        : Variable((const uint8_t)TEROS11_TEMP_VAR_NUM,
                   (uint8_t)TEROS11_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "SoilTemp") {}
    ~MeterTeros11_Temp() {}
};


// Defines the Volumetric Water Content Variable
class MeterTeros11_VWC : public Variable {
 public:
    explicit MeterTeros11_VWC(MeterTeros11* parentSense, const char* uuid = "",
                              const char* varCode = "SoilVWC")
        : Variable(parentSense, (const uint8_t)TEROS11_VWC_VAR_NUM,
                   (uint8_t)TEROS11_VWC_RESOLUTION, "volumetricWaterContent",
                   "percent", varCode, uuid) {}
    MeterTeros11_VWC()
        : Variable((const uint8_t)TEROS11_VWC_VAR_NUM,
                   (uint8_t)TEROS11_VWC_RESOLUTION, "volumetricWaterContent",
                   "percent", "SoilVWC") {}
    ~MeterTeros11_VWC() {}
};

#endif  // SRC_SENSORS_METERTEROS11_H_
