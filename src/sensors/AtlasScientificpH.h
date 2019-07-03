/*
 * AtlasScientificpH.h
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * The output from the Atlas Scientifc pH is the temperature in degrees C.
 *     Accuracy is ± 0.002
 *     Range is 0.001 − 14.000
 *     Resolution is 0.001
 */

// Header Guards
#ifndef AtlasScientificpH_h
#define AtlasScientificpH_h

// Included Dependencies
#include "sensors/AtlasParent.h"

// I2C address
#define ATLAS_PH_I2C_ADDR 0x63  // 99

// Sensor Specific Defines
#define ATLAS_PH_NUM_VARIABLES 1

#define ATLAS_PH_WARM_UP_TIME_MS 850  // 846 in SRGD Tests
#define ATLAS_PH_STABILIZATION_TIME_MS 0
// NOTE:  Manual says measurement takes 900 ms, but in SRGD tests, no result was
// available until 1656 ms
#define ATLAS_PH_MEASUREMENT_TIME_MS 1660

#define ATLAS_PH_RESOLUTION 3
#define ATLAS_PH_VAR_NUM 0

// The main class for the Atlas Scientific pH temperature sensor
// NOTE:  This is a template class!  In order to support either software or
// hardware instances of "Wire" (I2C) we must use a template.
template<typename THEWIRE>
class AtlasScientificpH : public AtlasParent<THEWIRE>
{
public:
    // Constructors
    AtlasScientificpH(THEWIRE theI2C, int8_t powerPin,
                      uint8_t i2cAddressHex = ATLAS_PH_I2C_ADDR,
                      uint8_t measurementsToAverage = 1)
     : AtlasParent<THEWIRE>(theI2C, powerPin, i2cAddressHex, measurementsToAverage,
                   "AtlasScientificpH", ATLAS_PH_NUM_VARIABLES,
                   ATLAS_PH_WARM_UP_TIME_MS, ATLAS_PH_STABILIZATION_TIME_MS,
                   ATLAS_PH_MEASUREMENT_TIME_MS)
    {}
    AtlasScientificpH(int8_t powerPin, uint8_t i2cAddressHex = ATLAS_PH_I2C_ADDR,
                      uint8_t measurementsToAverage = 1)
     : AtlasParent<THEWIRE>(powerPin, i2cAddressHex, measurementsToAverage,
                   "AtlasScientificpH", ATLAS_PH_NUM_VARIABLES,
                   ATLAS_PH_WARM_UP_TIME_MS, ATLAS_PH_STABILIZATION_TIME_MS,
                   ATLAS_PH_MEASUREMENT_TIME_MS)
    {}
    // Destructor
    ~AtlasScientificpH(){}

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

// The class for the pH variable
class AtlasScientificpH_pH : public Variable
{
public:
    AtlasScientificpH_pH(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "AtlaspH")
      : Variable(parentSense,
                 (const uint8_t)ATLAS_PH_VAR_NUM,
                 (uint8_t)ATLAS_PH_RESOLUTION,
                 "pH", "pH",
                 varCode, uuid)
    {}
    AtlasScientificpH_pH()
      : Variable((const uint8_t)ATLAS_PH_VAR_NUM,
                 (uint8_t)ATLAS_PH_RESOLUTION,
                 "pH", "pH", "AtlaspH")
    {}
    ~AtlasScientificpH_pH(){}
};

#endif  // Header Guard
