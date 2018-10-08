/*
 *ZebraTechDOpto.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the ZebraTech D-Opto digital dissolved oxygen sensor
 *This sensor communicates via SDI-12
 *
 *The manual for this sensor is available at:
 * www.zebra-tech.co.nz/wp-content/uploads/2014/04/D-Opto-Sensor-manual-A4-ver-2.pdf
 *
 * For Dissolved Oxygen:
 *     Accuracy is 1% of reading or 0.02PPM, whichever is greater
 *     Resolution is 0.01% / 0.001 PPM
 *
 * For Temperature:
 *     Accuracy is ± 0.1°C
 *     Resolution is 0.01°C
 *
 * Maximum warm-up time in SDI-12 mode: ~250ms
 * Excitiation time before measurement: ~5225ms
 * Maximum measurement duration: ~110ms
 *
 * Obnoxiously, the sensor will not take a "concurrent" measurement and leave
 * the sensor powered on, so we must wait entire ~5200ms exitation time and the
 * 110ms measurement time each time.
 * There is the ability to do a non-concurrent measurement and leave the
 * sensor powered on, in which case the re-measurement takes ~110ms, but doing
 * it that way the sensor would send an interruption when it was finished,
 * possibly colliding with and confusing other sensor results
 *
*/

#include "sensors/ZebraTechDOpto.h"


// Constructors with overloads
ZebraTechDOpto::ZebraTechDOpto(char SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage)
 : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS, DOPTO_MEASUREMENT_TIME_MS)
{}
ZebraTechDOpto::ZebraTechDOpto(char *SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage)
 : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS, DOPTO_MEASUREMENT_TIME_MS)
{}
ZebraTechDOpto::ZebraTechDOpto(int SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage)
 : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                "ZebraTech D-Opto", DOPTO_NUM_VARIABLES,
                DOPTO_WARM_UP_TIME_MS, DOPTO_STABILIZATION_TIME_MS, DOPTO_MEASUREMENT_TIME_MS)
{}
ZebraTechDOpto::~ZebraTechDOpto(){}


/***
// Defines the Temperature Variable
ZebraTechDOpto_Temp::ZebraTechDOpto_Temp(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, DOPTO_TEMP_VAR_NUM,
                "temperature", "degreeCelsius",
                DOPTO_TEMP_RESOLUTION,
                "DOtempC", UUID, customVarCode)
    {}
    ~ZebraTechDOpto_Temp(){}
};


// Defines the Dissolved Oxygen Percent Saturation
class ZebraTechDOpto_DOpct : public Variable
{
public:
    ZebraTechDOpto_DOpct(Sensor *parentSense,
                         const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, DOPTO_DOPCT_VAR_NUM,
                "oxygenDissolvedPercentOfSaturation", "percent",
                DOPTO_DOPCT_RESOLUTION,
                "DOpercent", UUID, customVarCode)
    {}
    ~ZebraTechDOpto_DOpct(){}
};


// Defines the Dissolved Oxygen Concentration
class ZebraTechDOpto_DOmgL : public Variable
{
public:
    ZebraTechDOpto_DOmgL(Sensor *parentSense,
                         const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, DOPTO_DOMGL_VAR_NUM,
                "oxygenDissolved", "milligramPerLiter",
                DOPTO_DOMGL_RESOLUTION,
                "DOppm", UUID, customVarCode)
    {}
    ~ZebraTechDOpto_DOmgL(){}
};
***/
