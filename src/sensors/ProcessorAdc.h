/*
 *processorAdc.h
 *This file is part of the EnviroDIY modular sensors library 
 *
 * Written By:  Neil Hanccok
 * Adapted from ExternalVoltage Bobby Schulz <schu3119@umn.edu>
 *
 * Software License: BSD-3.
 * Copyright (c) 2019, Neil Hancock
 * Stroud Water Research Center (SWRC)
 * and the EnviroDIY Development Team
 * Range:
 *   NOTE:  ADC Range is determined by supply voltage - No more than VDD + 0.3 V or
 *          5.5 V (whichever is smaller) must be applied to this device.
 *   Input range is dependent on external voltage divider
 *   Scaling is performed to transform ADC digital reading to a value with units.
 * 
 * Accuracy: depends on processors ADC and Vref
 *   SAMD21/51
 *   12-bit ADC: < 0.15% (gain error), <3 LSB (offset errror)
 * Resolution:
 *   NOTE:  1 bit of resolution is lost in single-ended reading.  The maximum
 *          possible resolution is over the differential range from
 *          negative to positive full scale, a single ended reading is only over
 *          the range from 0 to positive full scale)
 *
 *
 * Technical specifications for the Grove Voltage Divider can be found at:
 * http://wiki.seeedstudio.com/Grove-Voltage_Divider
 *
 * Technical specifications for the processors ADC are found in the processor manuals
*/

// Header Guards
#ifndef processorAdc_h
#define processorAdc_h

// Debugging Statement
// #define MS_PROCESSORADC_DEBUG

#ifdef MS_PROCESSORADC_DEBUG
#define MS_DEBUGGING_STD "ProcessorAdc"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"


#define PROC_ADC_NUM_VARIABLES 1
// Using the warm-up time of the ADS1115
#define PROC_ADC_WARM_UP_TIME_MS 2
// Assume a voltage is instantly ready
#define PROC_ADC_STABILIZATION_TIME_MS 0
#define PROC_ADC_MEASUREMENT_TIME_MS 0

#define PROC_ADC_VAR_NUM 0
#ifdef MS_USE_ADS1015
#define PROC_ADC_RESOLUTION 1
#else
#define PROC_ADC_RESOLUTION 4
#endif

#if !defined ProcAdcDef_Resolution
#define ProcAdcDef_Resolution 10
#endif //ProcAdcDef_Resolution
#if !defined ProcAdc_Max
#define ProcAdc_Max ((1<<ProcAdcDef_Resolution)-1)
#endif //ProcAdc_Max
#if !defined ProcAdcDef_Reference
// one of eAnalogReference 
#define ProcAdcDef_Reference AR_DEFAULT 
#endif //ProcAdcDef_Reference

// The main class for the external votlage monitor
class processorAdc : public Sensor
{

public:
    // The constructor - need the power pin and the data channel on the ADS1x15
    // The gain value, and number of measurements to average are optional
    // If nothing is given a 1x gain is used.
    processorAdc(int8_t powerPin, uint8_t adcChannel, float gain = 1,
                    uint8_t measurementsToAverage = 1);
    // Destructor
    ~processorAdc();

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

protected:
    uint8_t _adcChannel;
    float _gain;
    //uint8_t _i2cAddress;
};


// The single available variable is voltage
class processorAdc_Volt : public Variable
{
public:
    processorAdc_Volt(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "adcProc")
      : Variable(parentSense,
                 (const uint8_t)PROC_ADC_VAR_NUM,
                 (uint8_t)PROC_ADC_RESOLUTION,
                 "voltage", "volt",
                 varCode, uuid)
    {}
    processorAdc_Volt()
      : Variable((const uint8_t)PROC_ADC_VAR_NUM,
                 (uint8_t)PROC_ADC_RESOLUTION,
                 "voltage", "volt", "adcProc")
    {}
    ~processorAdc_Volt(){}
};

#endif  // Header Guard
