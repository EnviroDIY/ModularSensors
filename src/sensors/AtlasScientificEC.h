/*
 * AtlasScientificEC.h
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * The Atlas Scientifc Conductivity sensor outputs raw conductivity, TDS,
 * salinity, and specific gravity
 *     Accuracy is ± 2%
 *     Range is 0.07 − 500,000+ μS/cm
 *     Resolution is 3 decimal places
 */

// Header Guards
#ifndef AtlasScientificEC_h
#define AtlasScientificEC_h

// Debugging Statement
// #define MS_ATLASSCIENTIFICEC_DEBUG

#ifdef MS_ATLASSCIENTIFICEC_DEBUG
#define MS_DEBUGGING_STD "AtlasScientificEC"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "sensors/AtlasParent.h"

// I2C address
#define ATLAS_COND_I2C_ADDR 0x64  // 100

// Sensor Specific Defines
#define ATLAS_COND_NUM_VARIABLES 4

#define ATLAS_COND_WARM_UP_TIME_MS 745  // 739-740 in tests
#define ATLAS_COND_STABILIZATION_TIME_MS 0
// 555 measurement time in tests, but keep the 600 recommended by manual
#define ATLAS_COND_MEASUREMENT_TIME_MS 600

#define ATLAS_COND_RESOLUTION 3
#define ATLAS_COND_VAR_NUM 0

#define ATLAS_TDS_RESOLUTION 3
#define ATLAS_TDS_VAR_NUM 1

#define ATLAS_SALINITY_RESOLUTION 3
#define ATLAS_SALINITY_VAR_NUM 2

#define ATLAS_SG_RESOLUTION 3
#define ATLAS_SG_VAR_NUM 3

// The main class for the Atlas Scientific Conductivity sensor
// NOTE:  This is a template class!  In order to support either software or
// hardware instances of "Wire" (I2C) we must use a template.
template<typename THEWIRE>
class AtlasScientificEC : public AtlasParent<THEWIRE>
{
public:
    // Constructors
    AtlasScientificEC(THEWIRE theI2C, int8_t powerPin,
                      uint8_t i2cAddressHex = ATLAS_COND_I2C_ADDR,
                      uint8_t measurementsToAverage = 1)
      : AtlasParent<THEWIRE>(theI2C, powerPin, i2cAddressHex, measurementsToAverage,
                    "AtlasScientificEC", ATLAS_COND_NUM_VARIABLES,
                    ATLAS_COND_WARM_UP_TIME_MS, ATLAS_COND_STABILIZATION_TIME_MS,
                    ATLAS_COND_MEASUREMENT_TIME_MS)
    {}
    AtlasScientificEC(int8_t powerPin, uint8_t i2cAddressHex = ATLAS_COND_I2C_ADDR,
                      uint8_t measurementsToAverage = 1)
      : AtlasParent<THEWIRE>(powerPin, i2cAddressHex, measurementsToAverage,
                    "AtlasScientificEC", ATLAS_COND_NUM_VARIABLES,
                    ATLAS_COND_WARM_UP_TIME_MS, ATLAS_COND_STABILIZATION_TIME_MS,
                    ATLAS_COND_MEASUREMENT_TIME_MS)
    {}
    // Destructor
    using AtlasParent<THEWIRE>::getSensorLocation;

    // We must be very explicity about which functions/internal variables we're using
    using Sensor::getSensorName;
    using Sensor::getSensorNameAndLocation;
    using Sensor::getPowerPin;
    using Sensor::setNumberMeasurementsToAverage;
    using Sensor::getNumberMeasurementsToAverage;
    using Sensor::getStatus;
    using Sensor::update;
    using Sensor::powerUp;
    using Sensor::powerDown;
    using Sensor::wake;
    using AtlasParent<THEWIRE>::sleep;
    using AtlasParent<THEWIRE>::startSingleMeasurement;
    using AtlasParent<THEWIRE>::addSingleMeasurementResult;
    using Sensor::clearValues;
    using Sensor::verifyAndAddMeasurementResult;
    using Sensor::averageMeasurements;
    using Sensor::registerVariable;
    using Sensor::notifyVariables;
    using Sensor::checkPowerOn;
    using Sensor::isWarmedUp;
    using Sensor::waitForWarmUp;
    using Sensor::isStable;
    using Sensor::waitForStability;
    using Sensor::isMeasurementComplete;
    using Sensor::waitForMeasurementCompletion;
    using AtlasParent<THEWIRE>::waitForProcessing;

    using AtlasParent<THEWIRE>::_i2c;
    using AtlasParent<THEWIRE>::_i2cAddressHex;

    using Sensor::_sensorStatus;
    using Sensor::sensorValues;
    using Sensor::_dataPin;
    using Sensor::_powerPin;
    using Sensor::_sensorName;
    using Sensor::_numReturnedVars;
    using Sensor::_measurementsToAverage;
    using Sensor::numberGoodMeasurementsMade;
    using Sensor::_warmUpTime_ms;
    using Sensor::_millisPowerOn;
    using Sensor::_stabilizationTime_ms;
    using Sensor::_millisSensorActivated;
    using Sensor::variables;

    // Setup
    virtual bool setup(void) override
    {
        bool success = Sensor::setup();  // this will set pin modes and the setup status bit

        // This sensor needs power for setup!
        // We want to turn on all possible measurement parameters
        bool wasOn = checkPowerOn();
        if (!wasOn) {powerUp();}
        waitForWarmUp();

        MS_ATLAS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report conductivity"));
        _i2c.beginTransmission(_i2cAddressHex);
        success &= _i2c.write((const uint8_t*)"O,EC,1", 7);  // Enable conductivity
        success &= !_i2c.endTransmission();
        success &= waitForProcessing();

        MS_ATLAS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report total dissolved solids"));
        _i2c.beginTransmission(_i2cAddressHex);
        success &= _i2c.write((const uint8_t*)"O,TDS,1", 8);  // Enable total dissolved solids
        success &= !_i2c.endTransmission();
        success &= waitForProcessing();

        MS_ATLAS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report salinity"));
        _i2c.beginTransmission(_i2cAddressHex);
        success &= _i2c.write((const uint8_t*)"O,S,1", 6);  // Enable salinity
        success &= !_i2c.endTransmission();
        success &= waitForProcessing();

        MS_ATLAS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report specific gravity"));
        _i2c.beginTransmission(_i2cAddressHex);
        success &= _i2c.write((const uint8_t*)"O,SG,1", 7);  // Enable specific gravity
        success &= !_i2c.endTransmission();
        success &= waitForProcessing();

        if (!success)
        {
            // Set the status error bit (bit 7)
            _sensorStatus |= 0b10000000;
            // UN-set the set-up bit (bit 0) since setup failed!
            _sensorStatus &= 0b11111110;
        }

        // Turn the power back off it it had been turned on
        if (!wasOn) {powerDown();}

        return success;
    }

};

// The class for the Conductivity Variable
class AtlasScientificEC_Cond : public Variable
{
public:
    AtlasScientificEC_Cond(Sensor *parentSense,
                           const char *uuid = "",
                           const char *varCode = "AtlasCond")
      : Variable(parentSense,
                 (const uint8_t)ATLAS_COND_VAR_NUM,
                 (uint8_t)ATLAS_COND_RESOLUTION,
                 "electricalConductivity", "microsiemenPerCentimeter",
                 varCode, uuid)
    {}
    AtlasScientificEC_Cond()
      : Variable((const uint8_t)ATLAS_COND_VAR_NUM,
                 (uint8_t)ATLAS_COND_RESOLUTION,
                 "electricalConductivity", "microsiemenPerCentimeter", "AtlasCond")
    {}
    ~AtlasScientificEC_Cond(){}
};

// The class for the Total Dissolved Solids Variable
class AtlasScientificEC_TDS : public Variable
{
public:
    AtlasScientificEC_TDS(Sensor *parentSense,
                          const char *uuid = "",
                          const char *varCode = "AtlasTDS")
      : Variable(parentSense,
                 (const uint8_t)ATLAS_TDS_VAR_NUM,
                 (uint8_t)ATLAS_TDS_RESOLUTION,
                 "solidsTotalDissolved", "partPerMillion",
                 varCode, uuid)
    {}
    AtlasScientificEC_TDS()
      : Variable((const uint8_t)ATLAS_TDS_VAR_NUM,
                 (uint8_t)ATLAS_TDS_RESOLUTION,
                 "solidsTotalDissolved", "partPerMillion", "AtlasTDS")
    {}
    ~AtlasScientificEC_TDS(){}
};

// The class for the Salinity Variable
class AtlasScientificEC_Salinity : public Variable
{
public:
    AtlasScientificEC_Salinity(Sensor *parentSense,
                               const char *uuid = "",
                               const char *varCode = "AtlasSalinity")
      : Variable(parentSense,
                 (const uint8_t)ATLAS_SALINITY_VAR_NUM,
                 (uint8_t)ATLAS_SALINITY_RESOLUTION,
                 "salinity", "practicalSalinityUnit",
                 varCode, uuid)
    {}
    AtlasScientificEC_Salinity()
      : Variable((const uint8_t)ATLAS_SALINITY_VAR_NUM,
                 (uint8_t)ATLAS_SALINITY_RESOLUTION,
                 "salinity", "practicalSalinityUnit", "AtlasSalinity")
    {}
    ~AtlasScientificEC_Salinity(){}
};

// The class for the Specific Gravity Variable
class AtlasScientificEC_SpecificGravity : public Variable
{
public:
    AtlasScientificEC_SpecificGravity(Sensor *parentSense,
                                      const char *uuid = "",
                                      const char *varCode = "AtlasSpecGravity")
      : Variable(parentSense,
                 (const uint8_t)ATLAS_SG_VAR_NUM,
                 (uint8_t)ATLAS_SG_RESOLUTION,
                 "specificGravity", "dimensionless",
                 varCode, uuid)
    {}
    AtlasScientificEC_SpecificGravity()
      : Variable((const uint8_t)ATLAS_SG_VAR_NUM,
                 (uint8_t)ATLAS_SG_RESOLUTION,
                 "specificGravity", "dimensionless", "AtlasSpecGravity")
    {}
    ~AtlasScientificEC_SpecificGravity(){}
};

#endif  // Header Guard
