/*
 * AtlasScientificORP.h
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * The output from the Atlas Scientifc ORP is the temperature in degrees C.
 *     Accuracy is ± 1 mV
 *     Range is -1019.9mV − 1019.9mV
 *     Resolution is 0.1 mV
 */

// Header Guards
#ifndef AtlasScientificORP_h
#define AtlasScientificORP_h

// Included Dependencies
#include "sensors/AtlasParent.h"

// I2C address
#define ATLAS_ORP_I2C_ADDR 0x62  // 98

// Sensor Specific Defines
#define ATLAS_ORP_NUM_VARIABLES 1

#define ATLAS_ORP_WARM_UP_TIME_MS 850  // 846 in SRGD tests
#define ATLAS_ORP_STABILIZATION_TIME_MS 0
// NOTE:  Manual says measurement takes 900 ms, but in SRGD tests, no result was
// available until 1577 ms
#define ATLAS_ORP_MEASUREMENT_TIME_MS 1580

#define ATLAS_ORP_RESOLUTION 1
#define ATLAS_ORP_VAR_NUM 0

// The main class for the Atlas Scientific ORP temperature sensor
// NOTE:  This is a template class!  In order to support either software or
// hardware instances of "Wire" (I2C) we must use a template.
template<typename THEWIRE>
class AtlasScientificORP : public AtlasParent<THEWIRE>
{
public:
    // Constructors
    AtlasScientificORP(THEWIRE theI2C, int8_t powerPin,
                       uint8_t i2cAddressHex = ATLAS_ORP_I2C_ADDR,
                       uint8_t measurementsToAverage = 1)
     : AtlasParent<THEWIRE>(theI2C, powerPin, i2cAddressHex, measurementsToAverage,
                   "AtlasScientificORP", ATLAS_ORP_NUM_VARIABLES,
                   ATLAS_ORP_WARM_UP_TIME_MS, ATLAS_ORP_STABILIZATION_TIME_MS,
                   ATLAS_ORP_MEASUREMENT_TIME_MS)
    {}
    AtlasScientificORP(int8_t powerPin, uint8_t i2cAddressHex = ATLAS_ORP_I2C_ADDR,
                       uint8_t measurementsToAverage = 1)
     : AtlasParent<THEWIRE>(powerPin, i2cAddressHex, measurementsToAverage,
                   "AtlasScientificORP", ATLAS_ORP_NUM_VARIABLES,
                   ATLAS_ORP_WARM_UP_TIME_MS, ATLAS_ORP_STABILIZATION_TIME_MS,
                   ATLAS_ORP_MEASUREMENT_TIME_MS)
    {}
    // Destructor
    ~AtlasScientificORP(){}

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

// The class for the oxidation/reduction potential Variable
class AtlasScientificORP_Potential : public Variable
{
public:
    AtlasScientificORP_Potential(Sensor *parentSense,
                                 const char *uuid = "",
                                 const char *varCode = "AtlasORP")
      : Variable(parentSense,
                 (const uint8_t)ATLAS_ORP_VAR_NUM,
                 (uint8_t)ATLAS_ORP_RESOLUTION,
                 "reductionPotential", "millivolt",
                 varCode, uuid)
    {}
    AtlasScientificORP_Potential()
      : Variable((const uint8_t)ATLAS_ORP_VAR_NUM,
                 (uint8_t)ATLAS_ORP_RESOLUTION,
                 "reductionPotential", "millivolt", "AtlasORP")
    {}
    ~AtlasScientificORP_Potential(){}
};

#endif  // Header Guard
