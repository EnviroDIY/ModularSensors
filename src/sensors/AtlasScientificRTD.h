/*
 * AtlasScientificRTD.h
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * The output from the Atlas Scientifc RTD is the temperature in degrees C.
 *     Accuracy is ± (0.10°C + 0.0017 x °C)
 *     Range is -126.000 °C − 1254 °C
 *     Resolution is 0.001 °C
 */

// Header Guards
#ifndef AtlasScientificRTD_h
#define AtlasScientificRTD_h

// Included Dependencies
#include "sensors/AtlasParent.h"

// I2C address
#define ATLAS_RTD_I2C_ADDR 0x66  // 102

// Sensor Specific Defines
#define ATLAS_RTD_NUM_VARIABLES 1

#define ATLAS_RTD_WARM_UP_TIME_MS 740  // 731-735 in tests
#define ATLAS_RTD_STABILIZATION_TIME_MS 0
// NOTE:  Manual says measurement time is 600, but in SRGD tests, didn't get a
// result until after 643 ms; AG got results as soon as 393ms.
#define ATLAS_RTD_MEASUREMENT_TIME_MS 650

#define ATLAS_RTD_RESOLUTION 3
#define ATLAS_RTD_VAR_NUM 0

// The main class for the Atlas Scientific RTD temperature sensor
// NOTE:  This is a template class!  In order to support either software or
// hardware instances of "Wire" (I2C) we must use a template.
template<typename THEWIRE>
class AtlasScientificRTD : public AtlasParent<THEWIRE>
{
public:
    // Constructors
    AtlasScientificRTD(THEWIRE theI2C, int8_t powerPin,
                       uint8_t i2cAddressHex = ATLAS_RTD_I2C_ADDR,
                       uint8_t measurementsToAverage = 1)
     : AtlasParent<THEWIRE>(theI2C, powerPin, i2cAddressHex, measurementsToAverage,
                   "AtlasScientificRTD", ATLAS_RTD_NUM_VARIABLES,
                   ATLAS_RTD_WARM_UP_TIME_MS, ATLAS_RTD_STABILIZATION_TIME_MS,
                   ATLAS_RTD_MEASUREMENT_TIME_MS)
    {}
    AtlasScientificRTD(int8_t powerPin, uint8_t i2cAddressHex = ATLAS_RTD_I2C_ADDR,
                       uint8_t measurementsToAverage = 1)
     : AtlasParent<THEWIRE>(powerPin, i2cAddressHex, measurementsToAverage,
                   "AtlasScientificRTD", ATLAS_RTD_NUM_VARIABLES,
                   ATLAS_RTD_WARM_UP_TIME_MS, ATLAS_RTD_STABILIZATION_TIME_MS,
                   ATLAS_RTD_MEASUREMENT_TIME_MS)
    {}
    // Destructor
    ~AtlasScientificRTD(){}

    // We must be very explicity about which functions/internal variables we're using
    // using AtlasParent<THEWIRE>::getSensorLocation;
    // using Sensor::getSensorName;
    // using Sensor::getSensorNameAndLocation;
    // using Sensor::getPowerPin;
    // using Sensor::setNumberMeasurementsToAverage;
    // using Sensor::getNumberMeasurementsToAverage;
    // using Sensor::getStatus;
    // using AtlasParent<THEWIRE>::setup;
    // using Sensor::update;
    // using Sensor::powerUp;
    // using Sensor::powerDown;
    // using Sensor::wake;
    // using AtlasParent<THEWIRE>::sleep;
    // using AtlasParent<THEWIRE>::startSingleMeasurement;
    // using AtlasParent<THEWIRE>::addSingleMeasurementResult;
    // using Sensor::clearValues;
    // using Sensor::verifyAndAddMeasurementResult;
    // using Sensor::averageMeasurements;
    // using Sensor::registerVariable;
    // using Sensor::notifyVariables;
    // using Sensor::checkPowerOn;
    // using Sensor::isWarmedUp;
    // using Sensor::waitForWarmUp;
    // using Sensor::isStable;
    // using Sensor::waitForStability;
    // using Sensor::isMeasurementComplete;
    // using Sensor::waitForMeasurementCompletion;
    // using AtlasParent<THEWIRE>::waitForProcessing;

    // using AtlasParent<THEWIRE>::_i2c;
    // using AtlasParent<THEWIRE>::_i2cAddressHex;

    // using Sensor::_sensorStatus;
    // using Sensor::sensorValues;
    // using Sensor::_dataPin;
    // using Sensor::_powerPin;
    // using Sensor::_sensorName;
    // using Sensor::_numReturnedVars;
    // using Sensor::_measurementsToAverage;
    // using Sensor::numberGoodMeasurementsMade;
    // using Sensor::_warmUpTime_ms;
    // using Sensor::_millisPowerOn;
    // using Sensor::_stabilizationTime_ms;
    // using Sensor::_millisSensorActivated;
    // using Sensor::variables;
};

// The class for the Temp Variable
class AtlasScientificRTD_Temp : public Variable
{
public:
    AtlasScientificRTD_Temp(Sensor *parentSense,
                            const char *uuid = "",
                            const char *varCode = "AtlasTemp")
      : Variable(parentSense,
                 (const uint8_t)ATLAS_RTD_VAR_NUM,
                 (uint8_t)ATLAS_RTD_RESOLUTION,
                 "temperature", "degreeCelsius",
                 varCode, uuid)
    {}
    AtlasScientificRTD_Temp()
      : Variable((const uint8_t)ATLAS_RTD_VAR_NUM,
                 (uint8_t)ATLAS_RTD_RESOLUTION,
                 "temperature", "degreeCelsius", "AtlasTemp")
    {}
    ~AtlasScientificRTD_Temp(){}
};

#endif  // Header Guard
