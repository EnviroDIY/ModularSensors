/*
 * AtlasScientificDO.h
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * The Atlas Scientifc DO sensor outputs DO in both mg/L and percent saturation
 *     Accuracy is ± 0.05 mg/L
 *     Range is 0.01 − 100+ mg/L (0.1 − 400+ % saturation)
 *     Resolution is 0.01 mg/L or 0.1 % saturation
 */

// Header Guards
#ifndef AtlasScientificDO_h
#define AtlasScientificDO_h

// Debugging Statement
// #define MS_ATLASSCIENTIFICDO_DEBUG

#ifdef MS_ATLASSCIENTIFICDO_DEBUG
#define MS_DEBUGGING_STD "AtlasScientificDO"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "sensors/AtlasParent.h"

// I2C address
#define ATLAS_DO_I2C_ADDR 0x61  // 97

// Sensor Specific Defines
#define ATLAS_DO_NUM_VARIABLES 2

#define ATLAS_DO_WARM_UP_TIME_MS 745  // 737-739 in tests
#define ATLAS_DO_STABILIZATION_TIME_MS 0
// 555 measurement time in tests, but keep the 600 recommended by manual
#define ATLAS_DO_MEASUREMENT_TIME_MS 600

#define ATLAS_DOMGL_RESOLUTION 2
#define ATLAS_DOMGL_VAR_NUM 0

#define ATLAS_DOPCT_RESOLUTION 1
#define ATLAS_DOPCT_VAR_NUM 1

// The main class for the Atlas Scientific DO sensor
// NOTE:  This is a template class!  In order to support either software or
// hardware instances of "Wire" (I2C) we must use a template.
template<typename THEWIRE>
class AtlasScientificDO : public AtlasParent<THEWIRE>
{
public:
    // Constructors
    AtlasScientificDO(THEWIRE theI2C, int8_t powerPin,
                      uint8_t i2cAddressHex = ATLAS_DO_I2C_ADDR,
                      uint8_t measurementsToAverage = 1)
      : AtlasParent<THEWIRE>(theI2C, powerPin, i2cAddressHex, measurementsToAverage,
                    "AtlasScientificDO", ATLAS_DO_NUM_VARIABLES,
                    ATLAS_DO_WARM_UP_TIME_MS, ATLAS_DO_STABILIZATION_TIME_MS,
                    ATLAS_DO_MEASUREMENT_TIME_MS)
    {}
    AtlasScientificDO(int8_t powerPin, uint8_t i2cAddressHex = ATLAS_DO_I2C_ADDR,
                      uint8_t measurementsToAverage = 1)
      : AtlasParent<THEWIRE>(powerPin, i2cAddressHex, measurementsToAverage,
                    "AtlasScientificDO", ATLAS_DO_NUM_VARIABLES,
                    ATLAS_DO_WARM_UP_TIME_MS, ATLAS_DO_STABILIZATION_TIME_MS,
                    ATLAS_DO_MEASUREMENT_TIME_MS)
    {}
    // Destructors
    ~AtlasScientificDO(){}

    // We must be very explicity about which functions/internal variables we're using
    using AtlasParent<THEWIRE>::getSensorLocation;
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


    virtual bool setup(void) override
    {
        bool success = Sensor::setup();  // this will set pin modes and the setup status bit

        // This sensor needs power for setup!
        // We want to turn on all possible measurement parameters
        bool wasOn = checkPowerOn();
        if (!wasOn) {powerUp();}
        waitForWarmUp();

        MS_ATLAS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report O2 concentration"));
        _i2c.beginTransmission(_i2cAddressHex);
        success &= _i2c.write((const uint8_t*)"O,mg,1", 7);  // Enable concentration in mg/L
        success &= !_i2c.endTransmission();
        success &= waitForProcessing();

        MS_ATLAS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report O2 % saturation"));
        _i2c.beginTransmission(_i2cAddressHex);
        success &= _i2c.write((const uint8_t*)"O,%,1", 6);  // Enable percent saturation
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

// The class for the DO Concentration Variable
class AtlasScientificDO_DOmgL : public Variable
{
public:
    AtlasScientificDO_DOmgL(Sensor *parentSense,
                            const char *uuid = "",
                            const char *varCode = "AtlasDOmgL")
      : Variable(parentSense,
                 (const uint8_t)ATLAS_DOMGL_VAR_NUM,
                 (uint8_t)ATLAS_DOMGL_RESOLUTION,
                 "oxygenDissolved", "milligramPerLiter",
                 varCode, uuid)
    {}
    AtlasScientificDO_DOmgL()
      : Variable((const uint8_t)ATLAS_DOMGL_VAR_NUM,
                 (uint8_t)ATLAS_DOMGL_RESOLUTION,
                 "oxygenDissolved", "milligramPerLiter", "AtlasDOmgL")
    {}
    ~AtlasScientificDO_DOmgL(){}
};

// The class for the DO Percent of Saturation Variable
class AtlasScientificDO_DOpct : public Variable
{
public:
    AtlasScientificDO_DOpct(Sensor *parentSense,
                            const char *uuid = "",
                            const char *varCode = "AtlasDOpct")
      : Variable(parentSense,
                 (const uint8_t)ATLAS_DOPCT_VAR_NUM,
                 (uint8_t)ATLAS_DOPCT_RESOLUTION,
                 "oxygenDissolvedPercentOfSaturation", "percent",
                 varCode, uuid)
    {}
    AtlasScientificDO_DOpct()
      : Variable((const uint8_t)ATLAS_DOPCT_VAR_NUM,
                 (uint8_t)ATLAS_DOPCT_RESOLUTION,
                 "oxygenDissolvedPercentOfSaturation", "percent", "AtlasDOpct")
    {}
    ~AtlasScientificDO_DOpct(){}
};

#endif  // Header Guard
